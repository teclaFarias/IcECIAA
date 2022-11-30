/**********************************************************************************************
 * Trabajo Práctico - Electrónica Digital III
 * Profesores: Ing. Marcelo Romeo
 * Autor: Farías Darío Emmanuel
 * Email: defarias@gmail.com
 * 
 * Proyecto: LCDGrafico-TouchScreen
 * Procesador utilizado: Cortex-M4
 * Descripción:

 *** En M0: El cortexM0 se encarga de cargar el array con los valores de entrada a la Red Neuronal
para ser testeados.
	  Implementa comunicación SPI para el control del touch screen. 

 *** En M4: Inicialmente el cortexM4 entrena la Red Neuronal y actualiza los pesos. Luego esperará la
orden de leer el array de datos de entrada a la Red para procesar la salida.

 * Fecha: 18-01-2018
 * Documentacion:
 *    - UM10503 (LPC43xx ARM Cortex-M4/M0 multi-core microcontroller User Manual)
 *    - PINES UTILIZADOS DEL NXP LPC4337 JBD144 (Ing. Eric Pernia)
 *    - Apuntes del Ing. Nicolás Alvarez
 *    - Repositorio Pablo Ridolfi
 **********************************************************************************************/

#define __IRQs_M4__   //Para compilar interrupciones del Cortex M4
#define __Cfg_M4__	  //Para compilar funciones de configuración del Cortex M0

#define BUFFER_RX_UART2  3    // 3 Caracteres por Comando o Valor. Se contemplan hasta 256 Instrucciones con 1 argumento de 8 bits.


#include <stdint.h>
#include <string.h>
#include <math.h>
#include "board.h"
#include "Headers/main_m4.h"
#include "cr_start_m0.h"
#include "sys_config.h"
#include "cmsis.h"

// Sensor de Color RGB
#define CANT_MEDICIONES		1

// Red Neuronal
#define CANT_ELEMENTOS		7
#define CANT_ELEMENTOS_PRUEBA	1  //Tamaño del array con datos RGB

#define LONGITUD_MENSAJE	200
#define VALOR_MAXIMO			255
#define ATENUACION			(int)255
#define TASA_APRENDIZAJE	0.25
#define ERROR_MINIMO			0.01
#define NIVEL_ACTIVACION	0.5
#define BIAS					0
#define LIMITE_PROCESO		500
#define FUNC_ACTIVACION		0


#define CANT_ENTRADAS			3
#define CANT_CAPAS	   		2
#define CANT_NEURONAS_CAPA0	CANT_ENTRADAS
#define CANT_NEURONAS_CAPA1  	3		// Capa Oculta más próxima a la entrada
#define CANT_NEURONAS_CAPA2  	1		// Capa de salida
#define CANT_ENTRADAS_CAPA1  	(CANT_NEURONAS_CAPA1 * CANT_ENTRADAS) 
#define CANT_ENTRADAS_CAPA2  	(CANT_NEURONAS_CAPA2 * CANT_NEURONAS_CAPA1) 
#define CANT_NEURONAS			CANT_NEURONAS_CAPA2 + CANT_NEURONAS_CAPA1


__attribute__((section(".misvariables.MemoriaIPC"))) ipc_t MemoriaIPC; 

bool tec1= false;
bool tec2= false;
bool tec3= false;
bool tec4= false;
bool signal_M0= false;
unsigned int demora=0;
static char aux[LONGITUD_MENSAJE];		// para debugging a través de USART 2 (115200 Baudios-8N1)
int pesoDesconocido=0; //variable para indicar si ocurrió overflow en el calculo de los pesos

char dataRx_UART2[BUFFER_RX_UART2];  // Buffer para armar el comando o valor recibido.
char * pRx_UART2= dataRx_UART2;      // Puntero para recorrer la cola.
int contadorRx_UART2=0;				    // Variable para recorrer con el puntero.
bool instruccionPendiente= false;    // Flag para ejecutar instrucción. True: Pendiente de atención.
unsigned char indiceFunciones=0;     // Selección de función a ejecutar.
unsigned char valorFuncion=0;        // Argumento de la función a ejecutar.


#include "funciones.c"
#include "config.c"
#include "interrupt.c"

int arrayTraining[CANT_ELEMENTOS][CANT_ENTRADAS+1] ={
{4,2,2,0},
{245,2,245,0},
{12,12,235,1},
{245,245,2,0},
{245,245,245,0},
{2,2,2,0},
{12,1,195,1}};



// ********************************************************************************************
// **************************************** Funciones *****************************************
// ********************************************************************************************


struct training_data{
	float entrada[CANT_ENTRADAS];
	float salidaDeseada;
	float trainingError;
};


int entradasXcapa[CANT_CAPAS]={CANT_ENTRADAS_CAPA1, CANT_ENTRADAS_CAPA2};

int neuronasXcapa[3]={CANT_ENTRADAS, CANT_NEURONAS_CAPA1, CANT_NEURONAS_CAPA2};


struct prop_data_Capa1{
	float peso[CANT_NEURONAS_CAPA0][CANT_NEURONAS_CAPA1];
	float error[CANT_NEURONAS_CAPA1];
	float errorCM[CANT_NEURONAS_CAPA1];
	float delta[CANT_NEURONAS_CAPA1];
	float bias[CANT_NEURONAS_CAPA1];
	float salida[CANT_NEURONAS_CAPA1];
};


struct prop_data_Capa2{
	float peso[CANT_NEURONAS_CAPA1][CANT_NEURONAS_CAPA2];
	float error[CANT_NEURONAS_CAPA2];
	float errorCM[CANT_NEURONAS_CAPA2];
	float delta[CANT_NEURONAS_CAPA2];
	float bias[CANT_NEURONAS_CAPA2];
	float salida[CANT_NEURONAS_CAPA2];
};


struct training_data datos[CANT_ELEMENTOS];
struct training_data datosPrueba;
struct training_data testNeurona;
struct training_data *datosNeuronales= &datos[0];
struct training_data *datoNeuronal= &datosPrueba;

struct prop_data_Capa1 propCapa1;
struct prop_data_Capa2 propCapa2;


//Función: mensaje
// Esta función carga el string en el array de la estructura compartida por los cores y genera una
// interrupción para que el cortexM0 envíe el string a la USART.

int mensaje(char *texto){
	int i=0;
	for(i=0; i< LONGITUD_MENSAJE; i++)
		MemoriaIPC.mensaje[i]= *(texto +i);  //Copio el texto al array compartido

	MemoriaIPC.comando2_M0=NUEVO_MENSAJE;	//Especifico que la interrupción será por un string a visualizar
	signal_m4();	// Genero la Interrupción
	delay_systick_ms(5);	
}



//Función: obtenerPeso
// Esta función extrae el elemento "peso" entre las neuronas deseadas.

float obtenerPeso( int capa, int neuronaIn, int neuronaOut){
	float valor=0;
	switch(capa){
		case 1:
				valor = propCapa1.peso[neuronaIn][neuronaOut];
				break;
		case 2:
				valor = propCapa2.peso[neuronaIn][neuronaOut];
				break;
		default:
				break;
	}
	return valor;
}



//Función: actualizarPeso
// Esta función modifica el elemento "peso" entre las neuronas deseadas.

int actualizarPeso(int capa, int neuronaIn, int neuronaOut, float valor){
	switch(capa){
		case 1:
				propCapa1.peso[neuronaIn][neuronaOut] = valor;
				break;
		case 2:
				propCapa2.peso[neuronaIn][neuronaOut] = valor;
				break;
		default:
				break;
	}
	return 1;
}



//Función: obtenerError
// Esta función extrae el elemento "error" de la neurona de salida.

float obtenerError(int capa, int neurona){
	float valor=0;
	switch(capa){
		case 1:
				valor = propCapa1.error[neurona];
				break;
		case 2:
				valor = propCapa2.error[neurona];
				break;
		default:
				break;
	}
	return valor;
}



//Función: actualizarError
// Esta función modifica el elemento "error" de la neurona deseada.

int actualizarError(int capa, int neurona, float valor){
	switch(capa){
		case 1:
				propCapa1.error[neurona] = valor;
				break;
		case 2:
				propCapa2.error[neurona] = valor;
				break;
		default:
				break;
	}
	return 1;
}



//Función: obtenerErrorCM
// Esta función extrae el elemento "errorCM" de la neurona de salida.

float obtenerErrorCM(int capa, int neurona){
	float valor=0;
	switch(capa){
		case 1:
				valor = propCapa1.errorCM[neurona];
				break;
		case 2:
				valor = propCapa2.errorCM[neurona];
				break;
		default:
				break;
	}
	return valor;
}



//Función: actualizarErrorCM
// Esta función modifica el elemento "errorCM" de la neurona deseada.

int actualizarErrorCM(int capa, int neurona, float valor){
	switch(capa){
		case 1:
				propCapa1.errorCM[neurona] = valor;
				break;
		case 2:
				propCapa2.errorCM[neurona] = valor;
				break;
		default:
				break;
	}
	return 1;
}



//Función: obtenerSalida
// Esta función extrae el elemento "salida" de la neurona deseada.

float obtenerSalida( int capa, int neurona){
	float valor=0;
	switch(capa){
		case 0:
				valor = datoNeuronal->entrada[neurona];
				break;
		case 1:
				valor = propCapa1.salida[neurona];
				break;
		case 2:
				valor = propCapa2.salida[neurona];
				break;
		default:
				break;
	}
	return valor;
}



//Función: actualizarSalida
// Esta función modifica el elemento "salida" de la neurona deseada.

int actualizarSalida(int capa, int neurona, float valor){
	switch(capa){
		case 0:
				datoNeuronal->entrada[neurona] = valor;
				break;
		case 1:
				propCapa1.salida[neurona] = valor;
				break;
		case 2:
				propCapa2.salida[neurona] = valor;
				break;
		default:
				break;
	}
	return 1;
}



//Función: obtenerSalidaDeseada
// Esta función extrae el elemento "salidaDeseada" de la neurona de entrada.

float obtenerSalidaDeseada(){
	float valor=0;
	valor = datoNeuronal->salidaDeseada;
	return valor;
}



//Función: actualizarSalidaDeseada
// Esta función modifica el elemento "salidaDeseada" de la neurona de entrada.

int actualizarSalidaDeseada(float valor){
	datoNeuronal->salidaDeseada= valor;
	return 1;
}



//Función: obtenerElementoEntrenamiento
// Esta función extrae valor de entrada de un elemento de entrenamiento indexado.

float obtenerElementoEntrada(int index, int a){
	return (datosNeuronales +index)->entrada[a];
}



//Función: obtenerElementoSalidaDeseada
// Esta función extrae valor de Salida Deseada de un elemento de entrenamiento indexado.

float obtenerElementoSalidaDeseada(int index){
	return (datosNeuronales +index)->salidaDeseada;
}



//Función: obtenerElementoTrainingError
// Esta función extrae valor de Training Error de un elemento de entrenamiento indexado.

float obtenerElementoTrainingError(int index){
	return (datosNeuronales +index)->trainingError;
}



//Función: procesarDatoNeuronal
// Esta función carga las entradas y el valor deseado de salida para su posterior procesamiento.

int procesarDatoNeuronal(char modo, int index,float dato0, float dato1, float dato2, int salidaDeseada){
	switch(modo){
		case 'E':
			actualizarSalida(0,0, obtenerElementoEntrada(index, 0));
			actualizarSalida(0,1, obtenerElementoEntrada(index, 1));
			actualizarSalida(0,2, obtenerElementoEntrada(index, 2));
			actualizarSalidaDeseada(obtenerElementoSalidaDeseada(index));
			break;
		case 'P':
			actualizarSalida(0,0, (float) dato0);
			actualizarSalida(0,1, (float) dato1); 
			actualizarSalida(0,2, (float) dato2); 
			actualizarSalidaDeseada(salidaDeseada);
			break;
		default:
			break;
	}
	return 1;
}



//Función: actualizarElementoEntrenamiento
// Esta función modifica valor de entrada, salida deseada, o error obtenido de un elemento de entrenamiento indexado.
// Al valor de entrada lo atenua, para normalizarlo entre [0...1].

int actualizarElementoEntrenamiento(int index, int a, int parametro, int valor, float error){
	if(parametro == 0)
		(datosNeuronales +index)->entrada[a] = (float)valor/ATENUACION;
	if(parametro == 1)
		(datosNeuronales +index)->salidaDeseada = valor;
	if(parametro == 2)
		(datosNeuronales +index)->trainingError = error;
	return 1;
}
 

int cargarArrayTraining(){
	int b=0;
	int a=0;
	int cont=0;
	for(a=0; a< CANT_ELEMENTOS; a++){
		for(b=0; b < CANT_ENTRADAS; b++){
			actualizarElementoEntrenamiento(a,cont,0, (unsigned char) arrayTraining[a][cont], 9999);
			cont++;
		}
		actualizarElementoEntrenamiento(a,0,1, (unsigned char) arrayTraining[a][cont], 9999);
		cont=0;
	}
	return 1;
}


//Función: obtenerDelta
// Esta función extrae el elemento "delta" de la neurona deseada.

float obtenerDelta( int capa, int neurona){
	float valor=0;
	switch(capa){
		case 1:
				valor = propCapa1.delta[neurona];
				break;
		case 2:
				valor = propCapa2.delta[neurona];
				break;
		default:
				break;
	}
	return valor;
}



//Función: actualizarDelta
// Esta función modifica el elemento "delta" de la neurona deseada.

int actualizarDelta(int capa, int neurona, float valor){
	switch(capa){
		case 1:
				propCapa1.delta[neurona] = valor;
				break;
		case 2:
				propCapa2.delta [neurona] = valor;
				break;
		default:
				break;
	}
	return 1;
}



//Función: inicializarPesos
// Esta función genera un valor aleatorio para cada peso de la Red Neuronal

int inicializarPesos(){
	for(int capa=0; capa < CANT_CAPAS; capa++){  //Empieza en 1 para no utilizar la capa 0 que es la RGB
		for(int fila=0; fila < neuronasXcapa[capa]; fila++){
			for(int columna=0; columna < neuronasXcapa[capa +1]; columna++){
				actualizarPeso(capa+1,fila,columna,(float)(rand() %99+1)/100);
			}
		}
	}
	return 1;
}


// Función: derivaSigmoide
// Esta función aplica la derivada de la función sigmoide con la salida de la neurona deseada.

float derivaSigmoide(int capa, int neurona){
	float valor=0;
	float salida_tmp=0;
	salida_tmp= obtenerSalida(capa, neurona);
	valor= salida_tmp*(1- salida_tmp);
	return valor;
}



// Función: funcionActivacion
// Esta función recibe el tipo de función de activación y el valor como argumento, devuelve el 
// calculo de la imagen en la función seleccionada.
// Tipo de función de activación implementada: 
// 0 -> Función Sigmoide
// 1 -> Función lineal
// 2 -> Función Escalón

float funcionActivacion(int tipoFuncion,float valor){
	float exp_valor=0;
	float resultado=0;
	if(tipoFuncion == 0){
		exp_valor = exp((-1) * valor);
		resultado = 1 / (1 + exp_valor);
		return resultado;
	}
	if(tipoFuncion == 1){
		resultado = valor;
		return resultado;
	}
	if(tipoFuncion == 2){
		if(valor > 0.5)
			return 1;
		else
			return 0;
	}
	return -2;
}


// Función: derivarFuncionActivacion
// Esta función recibe el tipo de función de activación y el valor como argumento, devuelve el 
// calculo de la derivada de la función seleccionada en el punto dado.
// Tipo de finción de activación implementada: 
// 0 -> Función Sigmoide

float derivarfuncionActivacion(int tipoFuncion,float valor){
	float resultado=0;
	if(tipoFuncion == 0){
		resultado = funcionActivacion(tipoFuncion, valor) * (1 - funcionActivacion(tipoFuncion, valor));
	}
	return resultado;
}



//Función: propagarCapa0
// Esta función calcula la suma de los productos entre la entrada de las neuronas de entrada con su
// respectivo peso.

int propagarCapa0(){
	int capa=0;
	int neuronaIn=0;
	int neuronaOut=0;
	float acumulador=0;

	for(capa=0; capa < CANT_CAPAS; capa++){  
		for(neuronaOut=0; neuronaOut < neuronasXcapa[capa +1]; neuronaOut++){
			for(neuronaIn=0; neuronaIn < neuronasXcapa[capa]; neuronaIn++){
				acumulador += (obtenerSalida(capa, neuronaIn) * obtenerPeso(capa +1, neuronaIn, neuronaOut));
			 }
			actualizarSalida(capa+1, neuronaOut, funcionActivacion(0,acumulador));
			acumulador=0;
		}
	}
	return 1;
}



//Función: calcularError
// Esta función calcula el error de la capa de salida y neurona de salida.

float calcularError(){
	float valor=0;
	valor= obtenerSalidaDeseada() - obtenerSalida(CANT_CAPAS,0);
	actualizarError(CANT_CAPAS, 0, valor);
	return valor;
}



//Función: errorCuadraticoMedio
// Esta función calcula el error cuadrático medio.

int errorCuadraticoMedio(){
	float valor=0;
	float error=0;

	error= calcularError();
	valor = ((error * error) / 2);
	actualizarErrorCM(CANT_CAPAS, 0, valor);
	return 1;
}



//Función: propagarError
// Esta función calcula en cada neurona el producto del error con el peso correspondiente,
// llevando el valor del error cuadratico medio de la neurona de salida hacia las entradas.

int propagarError(){
	int capa=0;
	int neuronaIn=0;
	int neuronaOut=0;
	float acumulador=0;
	for(capa=CANT_CAPAS -1; capa > 0; capa--){  //Empieza en 1 para no utilizar la capa 0 que es la RGB
		for(neuronaOut=0; neuronaOut < neuronasXcapa[capa -1]; neuronaOut++){
			for(neuronaIn=0; neuronaIn < neuronasXcapa[capa]; neuronaIn++){
				acumulador += obtenerError(capa+1, neuronaIn) * obtenerPeso(capa, neuronaOut, neuronaIn);
			}
			actualizarError(capa, neuronaOut, acumulador);
			acumulador=0;
		}
	}
	return 1;
}


//Función: actualizarPesos
// Esta función calcula desde las neuronas de entrada los nuevos pesos y los actualiza.

int actualizarPesos(){
	int capa=0;
	int neuronaIn=0;
	int neuronaOut=0;
	float nuevoPeso=0;
	float errorTmp=0;
	float error=0;

	for(capa=0; capa < CANT_CAPAS; capa++){  
		for(neuronaOut=0; neuronaOut < neuronasXcapa[capa +1]; neuronaOut++){
			for(neuronaIn=0; neuronaIn < neuronasXcapa[capa]; neuronaIn++){
				error= obtenerError(capa+1, neuronaOut);
				errorTmp= obtenerErrorCM(capa+1, neuronaOut);
				nuevoPeso= obtenerPeso(capa +1, neuronaIn, neuronaOut) + (TASA_APRENDIZAJE * error * derivarfuncionActivacion(0,errorTmp)* obtenerSalida(capa, neuronaIn));
				actualizarPeso(capa+1, neuronaIn, neuronaOut, nuevoPeso);
			}
		}
	}
	return 1;
}



//Función: entrenarRedNeuronal
// Esta función realiza el entrenamiento de la Red Neuronal con el dato RGB seleccionado
// desde el array de datos trainning a través del argumento "indice".

int entrenarRedNeuronal(){
int contador=0;
float error=1;
	while( (contador < LIMITE_PROCESO) && (error > ERROR_MINIMO) ){
		propagarCapa0();
		calcularError();
		errorCuadraticoMedio();
		propagarError();
		actualizarPesos();
		error= obtenerErrorCM(CANT_CAPAS,0);
		contador++;
	}
/*	sprintf_mio(aux,"\n\r Contador: %d \n\r\n\r", contador);
	mensaje(&aux);
	delay_systick_ms(15);	Observando si el contador es menor que LIMITE_PROCESO
 	puedo verificar si llegó a minimizar el error o no lo logró. */
	return contador;
}



//Función: obtenerRGB
// Esta función toma de la estructura compartida en memoria entre el cortex M0 y M4 los valores RGB
// y los copia al array destinado para el procesamiento de datos

int obtenerRGB(){
	for(int i=0; i< CANT_ENTRADAS; i++){
		testNeurona.entrada[i]= (((float)MemoriaIPC.rgb[i]) / ATENUACION);
	}
	procesarDatoNeuronal('P', 9999, testNeurona.entrada[0], testNeurona.entrada[1], testNeurona.entrada[2], 9999);	
	return 1;
}


//Función: testeaNeurona
//Esta función testea la neurona con los pesos encontrados en el entrenamiento.
int ensayaNeurona(){
	float suma=0;
	int i=0;
	float salida=-1;

	obtenerRGB();
	propagarCapa0();
	salida= obtenerSalida(2,0);
	if(salida > NIVEL_ACTIVACION){
		MemoriaIPC.valor_M0= 1;  //Resultado del procesamiento de la Red
		sprintf_mio(aux,"\n\rPOSITIVO\n\r");
	}
	else{
		MemoriaIPC.valor_M0= 3;
		sprintf_mio(aux,"\n\rNEGATIVO\n\r");	
	}
	mensaje(&aux);	
	MemoriaIPC.comando2_M0= PENDIENTE;
	MemoriaIPC.estado_M4= NO_BUSY;
	return 1;
}


void demoraLogica20uS(){
	int variable=0;
	for(int a=0; a< 266; a++){
		variable++;
	}
}

void demoraLogica500nS(){
	int variable=0;
	for(int a=0; a< 1; a++){
		variable++;
	}
}


int inicializarEstructura(){
	MemoriaIPC.comando2_M0=0;      // Comando enviado al Cortex M0
	MemoriaIPC.comando2_M4=0;      // Comando enviado al Cortex M4
	MemoriaIPC.estado_M0=0;        // Cortex M0:  0= no está funcionando, 1= se encuentra funcionando
	MemoriaIPC.estado_M4=0;        // Cortex M4:  0= no está funcionando, 1= se encuentra funcionando
	MemoriaIPC.valor_M0=0;         // Variable datos de 8 bits
	MemoriaIPC.valor_M4=0;         // Variable datos de 8 bits
	MemoriaIPC.rgb[0]=0;		 // Variable para enviar valores R
	MemoriaIPC.rgb[1]=0;		 // Variable para enviar valores G
	MemoriaIPC.rgb[2]=0;		 // Variable para enviar valores B
	return 1;
}


unsigned char obtenerAnchoPulso(){
	int contador=0;
	int mediciones=0;
	int anchoPulso=0;
	int acumuladorEstadoBajo=0;
	int acumuladorEstadoAlto=0;
	int contNivelBajo=0;
	int contNivelAlto=0;	
	int multContNivelAlto=0;
	int multContNivelBajo=0;


	if( Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 1){
	mensaje("\n\rEntro 1...");
		while( Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 1); //espero a que comience un nuevo ciclo de clock
		//	mensaje("\n\rPaso el primer WHILE...");
		//comienzo medición del ancho del pulso del sensor
		while(mediciones < CANT_MEDICIONES){		
			//mensaje("\n\rEntro en el bucle de CANT_MEDICIONES...");
			while( Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 0){ // Cuento mientras se encuentra en estado bajo
				contNivelBajo++;
				//demoraLogica();
			}
			while( Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 1){ // Cuento mientras se encuentra en estado alto
				contNivelAlto++;
				//demoraLogica();
			}
			acumuladorEstadoBajo += contNivelBajo;	//Preparo varios conteos para promediar		
			acumuladorEstadoAlto += contNivelAlto;			
			contNivelBajo=0;
			contNivelAlto=0;	
			mediciones++;	
		}
	}
	else{   //( Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 0){
		mensaje("\n\rEntro 2...");
		while( Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 0); //espero a que comience un nuevo ciclo de clock
		//comienzo medición del ancho del pulso del sensor
		while(mediciones < CANT_MEDICIONES){		
			while(Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 1){ // Cuento mientras se encuentra en estado alto
				contNivelAlto++;
				//demoraLogica();
			}
			while( Chip_GPIO_GetPinState( GPIO_PORT, 3, 3) == 0){ // Cuento mientras se encuentra en estado bajo
				contNivelBajo++;
				//demoraLogica();
			}
			acumuladorEstadoBajo += contNivelBajo;	//Preparo varios conteos para promediar		
			acumuladorEstadoAlto += contNivelAlto;		
			contNivelBajo=0;
			contNivelAlto=0;	
			mediciones++;	
		}
	}
	// Promedio de cuentas
	acumuladorEstadoBajo= (int)acumuladorEstadoBajo/mediciones;
	acumuladorEstadoAlto= (int)acumuladorEstadoAlto/mediciones;
/*
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 0, true);  // GPIO0
	demoraLogica500nS();
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 0, false);  // GPIO0
	demoraLogica500nS();
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 0, true);  // GPIO0
	demoraLogica500nS();
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 0, false);  // GPIO0
	demoraLogica500nS();
*/
mensaje("\n\rCalculando promedio...");
	anchoPulso= (int)(acumuladorEstadoAlto*100)/(acumuladorEstadoBajo + acumuladorEstadoAlto); // ancho de pulso: 0 - 100
//	anchoPulso= (int)(anchoPulso*255)/100;
		sprintf_mio(aux,"\n\rValor ALTO: %d  valor BAJO: %d  dutyCycle: %d \n\r", acumuladorEstadoAlto, acumuladorEstadoBajo, anchoPulso );
		mensaje(&aux);
	anchoPulso= 50; //simulo duty cycle 50% para probar las otras funciones
	return anchoPulso;

}

// ********************************************************************************************
// ************************************ Programa Principal ************************************
// ********************************************************************************************



// Programa principal
int main(void){

	unsigned char cont=0;
	unsigned char cont_old=0;
	int dato=0;
	int repeticiones=0;
	int contador=0;
	int indice=0;
	int i=0;
	char i2a[5];
	
	Cfg_Pulsador();
	Cfg_SysTick();
	Cfg_Led();
	inicializarEstructura();	//Inicializamos estructura de comunicacion entre procesadores

	delay_systick_ms(7);	
	InicializaIPC();

	tec1= false;	//El reset eléctrico no deja los flags en True, por lo tanto las reseteo.
	tec2= false;
	tec3= false;
	tec4= false;

	mensaje("\n\r *** INICIANDO CORTEX M4 *** ");

	mensaje("\n\rInicializando Pesos Aleatorios...");
	inicializarPesos();
	mensaje("  Ok\n\r");

	mensaje("\n\rCargando datos de entrenamiento en memoria...");
	cargarArrayTraining();
	mensaje("  Ok\n\r");

	mensaje("\n\rIniciando entrenamiento... ");

for(repeticiones=0; repeticiones< 2; repeticiones++){

	for(indice=0; indice < CANT_ELEMENTOS ; indice++){
		procesarDatoNeuronal('E', indice, 9999, 9999, 9999, 9999);
		contador= entrenarRedNeuronal();
	}
}
	sprintf_mio(aux," Reprocesamiento: %d Ok \n\r", repeticiones-1);
	mensaje(&aux);

	mensaje("\n\rEntrada de datos disponible: \n\r");


	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 0, true);  // GPIO0
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 4, true);	// GPIO2
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 5, 15, true); // GPIO3
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 5, true);  // GPIO5


	while(1){

		if(MemoriaIPC.comando2_M4 == ELEMENTO_RGB){
			MemoriaIPC.estado_M4= BUSY;

			ensayaNeurona();

			MemoriaIPC.estado_M4= NO_BUSY;
			MemoriaIPC.comando2_M4 =STOP;
		} 


		if(MemoriaIPC.comando2_M4 == OBTENER_COLOR){
			MemoriaIPC.estado_M4= BUSY;

			for(int color=0; color < 1; color++){
				MemoriaIPC.mensaje[color] = obtenerAnchoPulso();
				//mensaje[color]=decodificar(duracion);
			}
			MemoriaIPC.comando2_M0 = NUEVA_LECTURA;
			__DSB();
			__SEV();
mensaje("\n\r Envio Senal al M0");
			MemoriaIPC.estado_M4= NO_BUSY;
			MemoriaIPC.comando2_M4 =STOP;
			//avisar con una señal
		} 

		delay_systick_ms(5);	
	}
	
}
