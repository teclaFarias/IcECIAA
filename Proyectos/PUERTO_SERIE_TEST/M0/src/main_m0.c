/**********************************************************************************************
 * Trabajo Práctico - Electrónica Digital II
 * Profesores: Ing.   e   Ing. Nicolás Alvarez
 * Autor: Farías Darío Emmanuel
 * Email: defarias@gmail.com
 *
 * Proyecto: TxRxUARTx
 * Procesador utilizado: Cortex-M0 en modo APP
 * Descripción:

 *** En M0: se generan 2 patrones de señal (senoidal y rampa) que son transmitidas por UART2.
Tiene incorporado un protocolo de comunicación para ejecutar funciones. Dichas funciones poseen
el prototipo: int nombre (unsigned char)
También posee una comunicación con el Cortex M4 a través de la estructura reservada en memoria 
y accedida por ambos procesadores, con la que se realiza la oscilacion de los Leds R y 3.


 *** En M4: Se leerán los datos y le enviará una señal al M0 para tooglear los Leds R y 3.
Se configuró la interrupción en TEC1 y TEC2 para encender y apagar el Led 2

 * Fecha: 13-04-2018
 *
 * Documentacion:
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


#include "math.h"
#include "board.h"
#include <string.h>
#include "cmsis_43xx_m0app.h"
#include "Headers/main_m0.h"

__attribute__((section(".misvariables.MemoriaIPC"))) ipc_t MemoriaIPC;    //Estructura para comunicación entre procesadores

bool signal_M4= false;
char aux[256];								 // Para debugging a través de USART 2 (115200 Baudios-8N1)
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
	for(int a=0; a< 256; a++)
		aux[a]= "";
}

void limpiaBufferUART2(){
	for(int a=0; a< BUFFER_RX_UART2; a++)
		dataRx_UART2[a]= "0";
}

// Funciones Vectorizadas

int encenderLed(unsigned char led){
	Led_ON(led);
	return(1);
}

int apagarLed(unsigned char led){
	Led_OFF(led);
	return(2);
}

int funcionNula(int info){
	return (-2);
}

// FIn de Funciones Vectorizadas

#define  FUNCIONES_VECTORIZADAS   2

int(*pInstrucciones[FUNCIONES_VECTORIZADAS])(unsigned char) = { \
apagarLed, 			// Funcion[indiceFuncion=0]
encenderLed			// Funcion[indiceFuncion=1]
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


#define DIGITOS_CANAL0  3
#define PERIODOS_SIERRA 4

int main(void)
{

	int tiempo=4;
	unsigned char contador=0;
	unsigned int i=0,j=0,k=0;
	int valor=0;
	float a=0; 
	int data_0[MUESTRAS];
	int data_1[MUESTRAS];
	char valorCanal0[DIGITOS_CANAL0];
	unsigned char comando2_M0=0;
	bool flag= true;
	unsigned char informacion=0;

	Cfg_Led();
	cfgRITTimer();
	Cfg_UART2();
	InicializarIPC();

	delay_rit_ms(3000);	//Demora para asegurar que inicie correctamente el cortex M4

// Señal Senoidal - CANAL: 0
	for(i=0; i < MUESTRAS; i++){
		data_0[i] = (400* sin(a)) + 400;
		a+= (6.28*10/MUESTRAS);
		}	

// Señal Rampa - CANAL: 1
	k=0;
	for(j=0; j < PERIODOS_SIERRA; j++)
		for(i=0; i < (int)MUESTRAS/PERIODOS_SIERRA; i++){
			data_1[k] = (i*6);
			k++;
		}

	i=0; 

	while(1){

		ejecutarInstruccion();

		sprintf_mio(aux,"%d|%d \n", data_0[i],data_1[i]);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
			

		if(data_0[i] > 128)
				MemoriaIPC.comando2_M0=18;
   	   else
				MemoriaIPC.comando2_M0=16;

		i++;
		if(i== MUESTRAS)
			i=0;
		delay_rit_ms(40);
	}

}

/* ************************* EOF M0 *************************** */
