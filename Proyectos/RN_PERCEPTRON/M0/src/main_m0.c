/**********************************************************************************************
 * Trabajo Práctico - Electrónica Digital III
 * Profesores: Ing. Marcelo Romeo
 * Autor: Farías Darío Emmanuel
 * Email: defarias@gmail.com
 *
 * Proyecto: Perceptron
 * Procesador utilizado: Cortex-M0 en modo APP
 * Descripción:

 *** En M0: El cortexM0 tiene implementado un protocolo de comunicación para ejecutar funciones. 
Dichas funciones poseen el prototipo: int nombre (unsigned char)
También posee una comunicación con el Cortex M4 a través de la estructura reservada en memoria 
y accedida por ambos procesadores.
Se comunica con el CortexM4 para enviarle la orden de procesar el dato RGB.


 *** En M4: recibirá la orden de procesar el dato y devolverá la salida de la Red Neuronal a 
través de la estructura implementada.

 * Fecha: 18-10-2018
 *
 * Documentacion:
 *    - PDFs varios sobre Redes Neuronales 
 *    - UM10503 (LPC43xx ARM Cortex-M4/M0 multi-core microcontroller User Manual)
 *    - PINES UTILIZADOS DEL NXP LPC4337 JBD144 (Ing. Eric Pernia)
 *    - Apuntes del Ing. Nicolás Alvarez
 *    - Repositorio Ing. Pablo Ridolfi (Modificado)
 **********************************************************************************************/

#define __IRQs_M0__    //Para compilar interrupciones del Cortex M0
#define __Cfg_M0__	  //Para compilar funciones de configuración del Cortex M0

#define MUESTRAS   		256

#define COMANDO			36		// ASCII de "#" para definir Comando
#define VALOR				35		// ASCII de "$" para definir Valor
#define NULO				0		// Utilizado para el Tipo de Dato Recibido NULO
#define LIMITE_INF_ASCII  39  // Corresponde al símbolo "'" Comilla simple o apóstrofe
#define LIMITE_SUP_ASCII  125 // Corresponde al símbolo "}" cierra llave
#define BUFFER_RX_UART2  3    // 3 Caracteres por Comando o Valor. Se contemplan hasta 256 Instrucciones con 1 argumento de 8 bits.

#define LONGITUD_MENSAJE	200

#include "math.h"
#include "board.h"
#include <string.h>
#include "cmsis_43xx_m0app.h"
#include "Headers/main_m0.h"

__attribute__((section(".misvariables.MemoriaIPC"))) ipc_t MemoriaIPC;    //Estructura para comunicación entre procesadores

bool signal_M4= false;
char aux[LONGITUD_MENSAJE];								 // Para debugging a través de USART 2 (115200 Baudios-8N1)
bool gpio3= false;
bool tec1= false;
unsigned char tipo_dato_UART2=3;


char dataRx_UART2[BUFFER_RX_UART2];  // Buffer para armar el comando o valor recibido.
char * pRx_UART2= dataRx_UART2;      // Puntero para recorrer la cola.
int contadorRx_UART2=0;				    // Variable para recorrer con el puntero.
bool instruccionPendiente= false;    // Flag para ejecutar instrucción. True: Pendiente de atención.
unsigned char indiceFunciones=0;     // Selección de función a ejecutar.
unsigned char valorFuncion=0;        // Argumento de la función a ejecutar.


#include "funciones.c"
#include "config.c"
#include "interrupt.c"

void limpiaBufferAUX(){
	for(int a=0; a< LONGITUD_MENSAJE; a++)
		aux[a]= "";
}

void limpiaBufferUART2(){
	for(int a=0; a< BUFFER_RX_UART2; a++)
		dataRx_UART2[a]= "0";
}

// Funciones Vectorizadas

int SinOperar(unsigned char i){
	return(0);
}

int encenderLed(unsigned char led){
	Led_ON(led);
	return(1);
}

int apagarLed(unsigned char led){
	Led_OFF(led);
	return(2);
}

int nuevoElementoR(unsigned char valor){
	MemoriaIPC.rgb[0]= valor;	
			sprintf_mio(aux,"Color Rojo: %d \n\r", valor);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 3;
}

int nuevoElementoG(unsigned char valor){
	MemoriaIPC.rgb[1]= valor;	
			sprintf_mio(aux,"Color Verde: %d \n\r", valor);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 4;
}

int nuevoElementoB(unsigned char valor){
	MemoriaIPC.rgb[2]= valor;	
			sprintf_mio(aux,"Color Azul: %d \n\r", valor);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 5;
}

int procesarRGB(unsigned char valor){
	MemoriaIPC.comando2_M4= ELEMENTO_RGB;	
			sprintf_mio(aux,"Procesando datos... \n\r");
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 6;
}

// FIn de Funciones Vectorizadas

#define  FUNCIONES_VECTORIZADAS   7

int(*pInstrucciones[FUNCIONES_VECTORIZADAS])(unsigned char) = { \
SinOperar,				// Funcion[indiceFuncion=0]
apagarLed, 				// Funcion[indiceFuncion=1]
encenderLed,			// Funcion[indiceFuncion=2]
nuevoElementoR,  		// Funcion[indiceFuncion=3]
nuevoElementoG,  		// Funcion[indiceFuncion=4]
nuevoElementoB,  		// Funcion[indiceFuncion=5]
procesarRGB				// Funcion[indiceFuncion=6]
};


void ejecutarInstruccion(){
	int resultado=-999; 												// es para saber que no se ejecutó la instrucción, es casi imposible recibir ese valor de una función.
	if(instruccionPendiente && (indiceFunciones < FUNCIONES_VECTORIZADAS)){
		resultado=-1;  												//es para saber que se ejecutó la instrucción pero no devolvió nada.
		resultado= pInstrucciones[indiceFunciones](valorFuncion);
		instruccionPendiente= false;
		}
	if(indiceFunciones >= FUNCIONES_VECTORIZADAS)
		instruccionPendiente= false;
	return(resultado);
}


int main(void)
{

	int tiempo=4;
	unsigned char contador=0;
	unsigned int i=0,j=0,k=0;
	int valor=0;
	float a=0; 
	bool flag= true;
	float respuesta_M4=0;
	char rta[2];

	Cfg_Led();
	cfgRITTimer();
	Cfg_UART2();
	InicializarIPC();

	delay_rit_ms(1000);	//Demora para asegurar que inicie correctamente el cortex M4

	sprintf_mio(aux,"\n\r *** INICIANDO CORTEX M0 *** ");
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	i=0; 

	while(1){

		ejecutarInstruccion();

		if(MemoriaIPC.comando2_M0 == PENDIENTE && MemoriaIPC.estado_M4 == NO_BUSY){
			MemoriaIPC.comando2_M0= STOP;
			respuesta_M4= (int)MemoriaIPC.valor_M0;
			if(respuesta_M4 == 1)
				sprintf_mio(aux,"\n\r Respuesta de la Red Neuronal en Cortex M0: SI \n\r\n\r");
			else
				sprintf_mio(aux,"\n\r Respuesta de la Red Neuronal en Cortex M0: NO \n\r\n\r");
			
			DEBUGSTR(aux);	
			limpiaBufferAUX();
		}

		delay_rit_ms(40);
	}

}

/* ************************* EOF M0 *************************** */
