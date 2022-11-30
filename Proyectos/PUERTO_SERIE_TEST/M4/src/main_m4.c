/**********************************************************************************************
 * Trabajo Práctico - Electrónica Digital II
 * Profesores: Ing.   e   Ing. Nicolás Alvarez
 * Autor: Farías Darío Emmanuel
 * Email: defarias@gmail.com
 * 
 * Proyecto: TxRxUARTx
 * Procesador utilizado: Cortex-M4
 * Descripción:

 *** En M0: se generan 2 patrones de señal (senoidal y rampa) que son transmitidas por UART2.
Tiene incorporado un protocolo de comunicación para ejecutar funciones. Dichas funciones poseen
el prototipo: int nombre (unsigned char)
También posee una comunicación con el Cortex M4 a través de la estructura reservada en memoria 
y accedida por ambos procesadores, con la que se realiza la oscilacion de los Leds R y 3.


 *** En M4: Se leerán los datos y le enviará una señal al M0 para tooglear los Leds R y 3.
Se configuró la interrupción en TEC1 y TEC2 para encender y apagar el Led 2

 * Fecha: 13-04-2018
 * Documentacion:
 *    - UM10503 (LPC43xx ARM Cortex-M4/M0 multi-core microcontroller User Manual)
 *    - PINES UTILIZADOS DEL NXP LPC4337 JBD144 (Ing. Eric Pernia)
 *    - Apuntes del Ing. Nicolás Alvarez
 *    - Repositorio Pablo Ridolfi
 **********************************************************************************************/

#define __IRQs_M4__   //Para compilar interrupciones del Cortex M4
#define __Cfg_M4__	  //Para compilar funciones de configuración del Cortex M0

#include <stdint.h>
#include "board.h"
#include "Headers/main_m4.h"
#include "cr_start_m0.h"
#include "sys_config.h"
#include "cmsis.h"


__attribute__((section(".misvariables.MemoriaIPC"))) ipc_t MemoriaIPC; 

bool tec1= false;
bool tec2= false;
bool tec3= false;
bool tec4= false;
bool signal_M0= false;
unsigned int demora=0;
static char aux[256];		// para debugging a través de USART 2 (115200 Baudios-8N1)


#include "funciones.c"
#include "config.c"
#include "interrupt.c"


// Programa principal
int main(void){

	unsigned char cont=0;
	unsigned char cont_old=0;
	int dato=0;

	Cfg_Pulsador();
	Cfg_SysTick();
	Cfg_Led();

	delay_systick_ms(7);	
	InicializaIPC();

	tec1= false;	//El reset eléctrico no deja los flags en True, por lo tanto las reseteo.
	tec2= false;
	tec3= false;
	tec4= false;
	MemoriaIPC.comando2_M0=0;

	while(1){
		if(MemoriaIPC.comando2_M0 == 18){
			Led_ON(2);
			Led_OFF(3);
		}
		else{
			Led_ON(3);
			Led_OFF(2);
		}
	}
}
