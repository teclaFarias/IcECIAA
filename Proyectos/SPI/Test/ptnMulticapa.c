/* ***************************************************************************
*	********************* ptnMulticapa.c **************************************
* ****************************************************************************
*
* Este programa entrena una Red Neuronal Multicapa con los valores listados 
* en el archivo detallado como argumento 0.
* Los datos del archivo deben estar separados por espacios. Rango de valores 
* de entrada [0... 255]. Posibles valores de Salida Deseada [0/1].
* Formato de los Datos: R G B SalidaDeseada
* Luego de entrenar la red, ejecuta el test con los 3 siguientes argumentos
* correspondiente a niveles R G B y devuelve el resultado.
* Autor: Dario E. Farías
* Fecha: 24/09/2018
**************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//#define DEBUG_PERCEPTRON
//#define DEBUG_PERCEPTRON_RESULTADO
//#define DEBUG_PERCEPTRON_ENTRADA
//#define DEBUG_PERCEPTRON_ERROR
//#define DEBUG_PERCEPTRON_SALIDA
//#define DEBUG_PERCEPTRON_FILE
//#define DEBUG_PERCEPTRON_RESULTADO


#define CANT_ELEMENTOS		10
#define CANT_ELEMENTOS_PRUEBA	1  //Tamaño del array con datos RGB

#define VALOR_MAXIMO			255
#define ATENUACION			(int)255
#define TASA_APRENDIZAJE	0.25
#define ERROR_MINIMO			0.01
#define NIVEL_ACTIVACION	0.5
#define BIAS					0
#define LIMITE_PROCESO		300
#define FUNC_ACTIVACION		0


#define CANT_ENTRADAS			3
#define CANT_CAPAS	   		2
#define CANT_NEURONAS_CAPA0	CANT_ENTRADAS
#define CANT_NEURONAS_CAPA1  	3		// Capa Oculta más próxima a la entrada
#define CANT_NEURONAS_CAPA2  	1		// Capa de salida
#define CANT_ENTRADAS_CAPA1  	(CANT_NEURONAS_CAPA1 * CANT_ENTRADAS) 
#define CANT_ENTRADAS_CAPA2  	(CANT_NEURONAS_CAPA2 * CANT_NEURONAS_CAPA1) 
#define CANT_NEURONAS			CANT_NEURONAS_CAPA2 + CANT_NEURONAS_CAPA1


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
struct training_data *datosNeuronales= &datos[0];
struct training_data *datoNeuronal= &datosPrueba;

struct prop_data_Capa1 propCapa1;
struct prop_data_Capa2 propCapa2;

float salida[CANT_NEURONAS];



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

int procesarDatoNeuronal(char modo, int index, float dato0, float dato1, float dato2, int salidaDeseada){
	switch(modo){
		case 'E':
			actualizarSalida(0,0, obtenerElementoEntrada(index, 0));
			actualizarSalida(0,1, obtenerElementoEntrada(index, 1));
			actualizarSalida(0,2, obtenerElementoEntrada(index, 2));
			actualizarSalidaDeseada(obtenerElementoSalidaDeseada(index));
			break;
		case 'P':
			actualizarSalida(0,0, (float) dato0/ATENUACION);
			actualizarSalida(0,1, (float) dato1/ATENUACION);
			actualizarSalida(0,2, (float) dato2/ATENUACION);
			actualizarSalidaDeseada(salidaDeseada);
			#ifdef DEBUG_PERCEPTRON_RESULTADO
			printf("\n\n Datos Manuales: \n");
			printf("\n Dato[0]: %0.4f \n", obtenerSalida(0,0));
			printf("\n Dato[1]: %0.4f \n", obtenerSalida(0,1));
			printf("\n Dato[2]: %0.4f \n", obtenerSalida(0,2));
			#endif
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
				#ifdef DEBUG_PERCEPTRON
				printf("\nPeso[Capa: %d] [fila: %d] [columna: %d]: %0.5f \n",capa +1, fila, columna, obtenerPeso(capa +1, fila, columna));
				#endif
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

	#ifdef DEBUG_PERCEPTRON_SALIDA
	for(int dato=0; dato < CANT_NEURONAS_CAPA0; dato++){
		printf("\nPropagación de dato[%d]: %0.4f \n",dato, obtenerSalida(0,dato));
	}
	printf("\nSalida Deseada: %0.4f \n",obtenerSalidaDeseada());
	#endif


	for(capa=0; capa < CANT_CAPAS; capa++){  
		for(neuronaOut=0; neuronaOut < neuronasXcapa[capa +1]; neuronaOut++){
			for(neuronaIn=0; neuronaIn < neuronasXcapa[capa]; neuronaIn++){
				acumulador += (obtenerSalida(capa, neuronaIn) * obtenerPeso(capa +1, neuronaIn, neuronaOut));
			 }
			actualizarSalida(capa+1, neuronaOut, funcionActivacion(0,acumulador));
			acumulador=0;
			#ifdef DEBUG_PERCEPTRON_SALIDA
			printf("\nFuncion de Salida[Capa: %d] [neurona: %d]: %0.5f \n",capa +1,neuronaOut, obtenerSalida(capa +1, neuronaOut));
			#endif
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
	#ifdef DEBUG_PERCEPTRON_ERROR
	printf("\nCálculo de Error: %0.8f  SalidaDeseada: %0.5f  SalidaObtenida: %0.5f  \n",valor,obtenerSalidaDeseada(), obtenerSalida(CANT_CAPAS,0));
	#endif
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
	#ifdef DEBUG_PERCEPTRON_ERROR
	printf("\nError Cuadratico Medio: %0.8f \n",obtenerError(CANT_CAPAS, 0));
	#endif
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
			#ifdef DEBUG_PERCEPTRON
			printf("\nPropagación de Error [Capa: %d] [neurona: %d]: %0.4f \n",capa,neuronaOut, obtenerError(capa, neuronaOut));
			#endif
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
			#ifdef DEBUG_PERCEPTRON
			printf("\nNuevo Peso[Capa: %d] [neurona: %d]: %0.5f \n",capa +1,neuronaOut, nuevoPeso);
			#endif
			}
		}
	}
	return 1;
}



// Función: listarPesos
// Esta función visualiza el peso actual de cada entrada neuronal. 

int listarPesos(){
	int capa=0;
	int neuronaIn=0;
	int neuronaOut=0;

	for(capa=0; capa < CANT_CAPAS; capa++){  
		for(neuronaOut=0; neuronaOut < neuronasXcapa[capa +1]; neuronaOut++){
			for(neuronaIn=0; neuronaIn < neuronasXcapa[capa]; neuronaIn++){
				printf("\n -> Peso[neuronaIN: %d][neuronaOUT: %d]= %0.5f \n",neuronaIn, neuronaOut, obtenerPeso(capa +1, neuronaIn, neuronaOut));
			}
		}
	}
	return 1;
}


//Función: entrenarRedNeuronal
// Esta función realiza el entrenamiento de la Red Neuronal con el dato RGB seleccionado
// desde el array de datos trainning a través del argumento "indice".

float entrenarRedNeuronal(){
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
	return contador;
}



// *************************************************************************************
// *****************************  Programa Principal  **********************************
// *************************************************************************************

int main(int argc, char *argv[]){
	int i=0;
	int a=0;
	float fpstring=0;
	int indice=0;
	int contador=0;
	float valor[CANT_ENTRADAS];
	float salida=0;

// *************************************************************************************
// **************** Lectura y Procesamiento de Archivo de Entrenamiento *****************

   FILE *fp = fopen(argv[1], "r");
#ifdef DEBUG_PERCEPTRON_FILE
	printf("Inicio Perceptrón...\n\n");
#endif
   if (fp == NULL){
		fprintf(stderr, "No se ha podido abrir el archivo de Entrenamiento.\n");
      exit(1);
   }
	else{
#ifdef DEBUG_PERCEPTRON_FILE
		printf("Leyendo %s...\n",argv[1]);
#endif
	}
	a=0;
	i=0;
	while (fscanf(fp,"%f",&fpstring) != EOF){
		if(a < 3){
			actualizarElementoEntrenamiento(i,a,0,fpstring, 0);
			a++;
		}
		else{
			actualizarElementoEntrenamiento(i,a,1,fpstring, 0);
			a=0;
			i++;
		 }
	}
	fclose(fp);

// ***************************************************************************************
// *********************** Impresión de Datos de Entrenamiento ***************************

#ifdef DEBUG_PERCEPTRON_ENTRADA
	for(i=0; i < CANT_ELEMENTOS; i++){
		printf("\n\n- Elemento: %d \n",i);
		for(unsigned char a=0; a< CANT_ENTRADAS; a++)
			printf("Dato [%d][%d]: %0.5f   ",i, a,obtenerElementoEntrada(i,a));
		printf("Salida Deseada [%d]: %0.3f",i, obtenerElementoSalidaDeseada(i));
	}
#endif

// ***************************************************************************************
// ************************ Entrenamiento de la RED NEURONAL *****************************

#ifdef DEBUG_PERCEPTRON
printf("\n Inicializo Pesos con valores aleatorios... \n");
#endif

inicializarPesos();

#ifdef DEBUG_PERCEPTRON
printf("\n Carga de Neuronas de entrada... \n");
#endif

for(indice=0; indice < CANT_ELEMENTOS ; indice++){
	procesarDatoNeuronal('E', indice, 9999, 9999, 9999, 9999);
	contador= entrenarRedNeuronal();
#ifdef DEBUG_PERCEPTRON
	printf("\n\n *** Error Cuadratico Medio [indice: %d ]= %0.5f ", indice, obtenerErrorCM(CANT_CAPAS, 0));
	printf("\n  -- Entrenamiento cíclico en: %d Repeticiones. \n\n", contador);
#endif
}

#ifdef DEBUG_PERCEPTRON
printf("\n *_*_*  FIN de Entrenamiento *_*_* \n");
#endif

// ***************************************************************************************
// ****************************** Testing de la RED NEURONAL *****************************

//Test con valores enviados como argumentos

	if(argc == 5){
		for(i=0; i< (3); i++)
			valor[i]= (float) (atoi(argv[i+2]));
		procesarDatoNeuronal('P', 9999, valor[0], valor[1], valor[2], 9999);	
		propagarCapa0();
		salida= obtenerSalida(CANT_CAPAS,0);
		#ifdef DEBUG_PERCEPTRON_RESULTADO
		listarPesos();
		printf("\n Salida del sistema: %0.4f  \n\n",salida);
		#endif		
	}
	else
		printf("\n\nNo se recibieron correctamente los argumentos: 'ejecutable + archivo.csv + R + G + B' \n");

#ifdef DEBUG_PERCEPTRON
	printf("Finalizó el proceso de la Red Neuronal.\n");
#endif
	if(salida > (float)0.5)
   	printf("1 \n");
	else
		printf("0 \n");
	return 1;
}
