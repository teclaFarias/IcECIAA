/**********************************************************************************************
 * Trabajo Práctico - Electrónica Digital III
 * Profesores: Ing. Marcelo Romeo
 * Autor: Farías Darío Emmanuel
 * Email: defarias@gmail.com
 *
 * Proyecto: LCDGrafico-MicroSD
 * Procesador utilizado: Cortex-M0 en modo APP
 * Descripción:

 *** En M0: El cortexM0 tiene implementado un protocolo de comunicación para ejecutar funciones. 
Dichas funciones poseen el prototipo: int nombre (unsigned char)
También posee una comunicación con el Cortex M4 a través de la estructura reservada en memoria 
y accedida por ambos procesadores.
Se comunica con el CortexM4 para enviarle la orden de procesar el dato RGB.
Implementa comunicación SPI con el XPT2046 (controlador touchscreen de 4 hilos) para el control
ON/OFF del led verde de la Edu-Ciaa. 
Implementa funciones para el control de display gráfico 320x240, dichas funciones se habilitan
desde el define: DISPLAY_GRAFICO_CONECTADO


 *** En M4: recibirá la orden de procesar el dato y devolverá la salida de la Red Neuronal a 
través de la estructura implementada.

 * Fecha: 18-01-2019
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
#define BUFFER_RX_UART2  8    // 3 Caracteres por Comando o Valor. Se contemplan hasta 256 Instrucciones con 1 argumento de 8 bits.

#define LONGITUD_MENSAJE	200

#define DISPLAY_GRAFICO_CONECTADO


// ***************************************************************************

#define LEFT 				0
#define RIGHT 				9999
#define CENTER 			9998
#define PORTRAIT 			0
#define LANDSCAPE 		1
#define PCF8833			10
#define ILI9481			21
#define CPLD				29
#define NOTINUSE			255

// ********** Definiciones para los Pines del Display Gráfico ***************

#define D0(I0)  	((I0)==(1)?(GPIO_PORT->SET[3] |= (1 << 0)):(GPIO_PORT->CLR[3] |= (1 << 0)))
#define D1(I1)		((I1)==(1)?(GPIO_PORT->SET[2] |= (1 << 4)):(GPIO_PORT->CLR[2] |= (1 << 4)))
#define D2(I2)		((I2)==(1)?(GPIO_PORT->SET[3] |= (1 << 4)):(GPIO_PORT->CLR[3] |= (1 << 4)))
#define D3(I3)		((I3)==(1)?(GPIO_PORT->SET[5] |= (1 << 15)):(GPIO_PORT->CLR[5] |= (1 << 15)))
#define D4(I4)		((I4)==(1)?(GPIO_PORT->SET[5] |= (1 << 16)):(GPIO_PORT->CLR[5] |= (1 << 16)))
#define D5(I5)		((I5)==(1)?(GPIO_PORT->SET[3] |= (1 << 5)):(GPIO_PORT->CLR[3] |= (1 << 5)))
#define D6(I6)		((I6)==(1)?(GPIO_PORT->SET[3] |= (1 << 6)):(GPIO_PORT->CLR[3] |= (1 << 6)))
#define D7(I7)		((I7)==(1)?(GPIO_PORT->SET[3] |= (1 << 7)):(GPIO_PORT->CLR[3] |= (1 << 7)))
#define D8(I8)		((I8)==(1)?(GPIO_PORT->SET[2] |= (1 << 8)):(GPIO_PORT->CLR[2] |= (1 << 8)))
#define D9(I9)		((I9)==(1)?(GPIO_PORT->SET[2] |= (1 << 0)):(GPIO_PORT->CLR[2] |= (1 << 0)))
#define D10(I10)	((I10)==(1)?(GPIO_PORT->SET[2] |= (1 << 1)):(GPIO_PORT->CLR[2] |= (1 << 1)))
#define D11(I11)	((I11)==(1)?(GPIO_PORT->SET[2] |= (1 << 2)):(GPIO_PORT->CLR[2] |= (1 << 2)))
#define D12(I12)	((I12)==(1)?(GPIO_PORT->SET[2] |= (1 << 3)):(GPIO_PORT->CLR[2] |= (1 << 3)))
#define D13(I13)	((I13)==(1)?(GPIO_PORT->SET[1] |= (1 << 8)):(GPIO_PORT->CLR[1] |= (1 << 8)))
#define D14(I14)	((I14)==(1)?(GPIO_PORT->SET[3] |= (1 << 12)):(GPIO_PORT->CLR[3] |= (1 << 12)))
#define D15(I15)	((I15)==(1)?(GPIO_PORT->SET[3] |= (1 << 13)):(GPIO_PORT->CLR[3] |= (1 << 13)))


#define RS(I)		if(I == 1){GPIO_PORT->SET[2] |= (1 << 5);}else{GPIO_PORT->CLR[2] |= (1 << 5);}
#define WR(I)		if(I == 1){GPIO_PORT->SET[2] |= (1 << 6);}else{GPIO_PORT->CLR[2] |= (1 << 6);}
#define RD(I)		if(I == 1){GPIO_PORT->SET[5] |= (1 << 14);}else{GPIO_PORT->CLR[5] |= (1 << 14);}
#define RESET(I)	if(I == 1){GPIO_PORT->SET[5] |= (1 << 12);}else{GPIO_PORT->CLR[5] |= (1 << 12);}
#define CS(I)		if(I == 1){GPIO_PORT->SET[5] |= (1 << 13);}else{GPIO_PORT->CLR[5] |= (1 << 13);}



// *********** Seteo de Entradas/Salidas de pines para lectura/escritura de Display ****************

#define DIR0(I0)  	((I0)==(1)?(GPIO_PORT->DIR[3] |= (1 << 0)):(GPIO_PORT->DIR[3] &= ~(1 << 0)))
#define DIR1(I1)		((I1)==(1)?(GPIO_PORT->DIR[2] |= (1 << 4)):(GPIO_PORT->DIR[2] &= ~(1 << 4)))
#define DIR2(I2)		((I2)==(1)?(GPIO_PORT->DIR[3] |= (1 << 4)):(GPIO_PORT->DIR[3] &= ~(1 << 4)))
#define DIR3(I3)		((I3)==(1)?(GPIO_PORT->DIR[5] |= (1 << 15)):(GPIO_PORT->DIR[5] &= ~(1 << 15)))
#define DIR4(I4)		((I4)==(1)?(GPIO_PORT->DIR[5] |= (1 << 16)):(GPIO_PORT->DIR[5] &= ~(1 << 16)))
#define DIR5(I5)		((I5)==(1)?(GPIO_PORT->DIR[3] |= (1 << 5)):(GPIO_PORT->DIR[3] &= ~(1 << 5)))
#define DIR6(I6)		((I6)==(1)?(GPIO_PORT->DIR[3] |= (1 << 6)):(GPIO_PORT->DIR[3] &= ~(1 << 6)))
#define DIR7(I7)		((I7)==(1)?(GPIO_PORT->DIR[3] |= (1 << 7)):(GPIO_PORT->DIR[3] &= ~(1 << 7)))
#define DIR8(I8)		((I8)==(1)?(GPIO_PORT->DIR[2] |= (1 << 8)):(GPIO_PORT->DIR[2] &= ~(1 << 8)))
#define DIR9(I9)		((I9)==(1)?(GPIO_PORT->DIR[2] |= (1 << 0)):(GPIO_PORT->DIR[2] &= ~(1 << 0)))
#define DIR10(I10)	((I10)==(1)?(GPIO_PORT->DIR[2] |= (1 << 1)):(GPIO_PORT->DIR[2] &= ~(1 << 1)))
#define DIR11(I11)	((I11)==(1)?(GPIO_PORT->DIR[2] |= (1 << 2)):(GPIO_PORT->DIR[2] &= ~(1 << 2)))
#define DIR12(I12)	((I12)==(1)?(GPIO_PORT->DIR[2] |= (1 << 3)):(GPIO_PORT->DIR[2] &= ~(1 << 3)))
#define DIR13(I13)	((I13)==(1)?(GPIO_PORT->DIR[1] |= (1 << 8)):(GPIO_PORT->DIR[1] &= ~(1 << 8)))
#define DIR14(I14)	((I14)==(1)?(GPIO_PORT->DIR[3] |= (1 << 12)):(GPIO_PORT->DIR[3] &= ~(1 << 12)))
#define DIR15(I15)	((I15)==(1)?(GPIO_PORT->DIR[3] |= (1 << 13)):(GPIO_PORT->DIR[3] &= ~(1 << 13)))


#define DIR_RS(I)		((I)==(1)?(GPIO_PORT->DIR[2] |= (1 << 5)):(GPIO_PORT->DIR[2] &= ~(1 << 5)))
#define DIR_WR(I)		((I)==(1)?(GPIO_PORT->DIR[2] |= (1 << 6)):(GPIO_PORT->DIR[2] &= ~(1 << 6)))
#define DIR_RD(I)		((I)==(1)?(GPIO_PORT->DIR[5] |= (1 << 14)):(GPIO_PORT->DIR[5] &= ~(1 << 14)))
#define DIR_RESET(I)	((I)==(1)?(GPIO_PORT->DIR[5] |= (1 << 12)):(GPIO_PORT->DIR[5] &= ~(1 << 12)))
#define DIR_CS(I)		((I)==(1)?(GPIO_PORT->DIR[5] |= (1 << 13)):(GPIO_PORT->DIR[5] &= ~(1 << 13)))

		

#define PULSO_RS(C)		if(C == 1){RS(1);RS(0);}else{RS(0);RS(1);}
#define PULSO_WR(C)		if(C == 1){WR(0);WR(1);WR(0);}else{WR(1);WR(0);WR(1);}
#define PULSO_RD(C)		if(C == 1){RD(1);RD(0);}else{RD(0);RD(1);}
#define PULSO_RESET(C)	if(C == 1){RESET(1);RESET(0);}else{RESET(0);RESET(1);}
#define PULSO_CS(C)		if(C == 1){CS(1);CS(0);}else{CS(0);CS(1);}




//#include "math.h"
#include "board.h"
#include <string.h>
#include "cmsis_43xx_m0app.h"
#include "Headers/main_m0.h"

__attribute__((section(".misvariables.MemoriaIPC"))) ipc_t MemoriaIPC;    //Estructura para comunicación entre procesadores
__attribute__((section(".misvariables.MemoriaIPC2"))) ipc2_t MemDisplay;    //Estructura para comunicación entre procesadores

bool signal_M4= false;
char aux[LONGITUD_MENSAJE];								 // Para debugging a través de USART 2 (115200 Baudios-8N1)
bool gpio3= false;
bool tec1= false;
unsigned char tipo_dato_UART2=3;

union LCD16bits{
int numero;
struct cadaBit {
unsigned char d0:1;
unsigned char d1:1;
unsigned char d2:1;
unsigned char d3:1;
unsigned char d4:1;
unsigned char d5:1;
unsigned char d6:1;
unsigned char d7:1;
unsigned char d8:1;
unsigned char d9:1;
unsigned char d10:1;
unsigned char d11:1;
unsigned char d12:1;
unsigned char d13:1;
unsigned char d14:1;
unsigned char d15:1;
} bus16;
};


union LCD8bits{
int numero;
struct cada8Bit {
unsigned char d0:1;
unsigned char d1:1;
unsigned char d2:1;
unsigned char d3:1;
unsigned char d4:1;
unsigned char d5:1;
unsigned char d6:1;
unsigned char d7:1;
} bus8;
};



#define BUFFER_INSTRUCCIONES_SERIE  50
#define BUFFER_COMANDOS_SERIE  50


unsigned char dataRx_UART2[BUFFER_RX_UART2];  // Buffer para armar el comando o valor recibido.
char * pRx_UART2= dataRx_UART2;      // Puntero para recorrer la cola.
int contadorRx_UART2=0;				    // Variable para recorrer con el puntero.
unsigned char instruccionPendiente= 0;    // Acumulador de instrucciones a ejecutar. Se creó una cola circular de instrucciones
unsigned long int indiceFunciones=0;     // Selección de función a ejecutar.
unsigned long int valorFuncion=0;        // Argumento de la función a ejecutar.
unsigned char contCmd=0;				// indice del buffer de comandos
unsigned char proximoCmd=0;			// indice para indicar el proximo elemento del array que se ejecutará

unsigned long int cmdProcesar[BUFFER_COMANDOS_SERIE][2];

int sensor=0;

uint32_t cant0_3=0;
uint32_t cant1_3=0;
uint32_t cant2_3=0;
uint32_t cant3_3=0;
uint32_t valor32bits=0;

unsigned long int addressSD=0;
unsigned long int elementoSD=0;
unsigned char valorElementoSD=0;

#define BUFFER_SERIE		512   

bool cargarBuffer= false;
unsigned char bufferSerie[BUFFER_SERIE];
unsigned int indiceSerie=0;
bool estadoBufferSerie= false;


// ************ Variables para Display Gráfico ***************
#define BUFFERLCD		20

int posIniX=0;
int posIniY=0;
int posFinX=0;
int posFinY=0;
int longLinea=0;
unsigned char lcdRcolor=0;
unsigned char lcdGcolor=0;
unsigned char lcdBcolor=0;
unsigned char lcdRBcolor=0;
unsigned char lcdGBcolor=0;
unsigned char lcdBBcolor=0;
unsigned char posAngular=0;
char LCDbuffer[BUFFERLCD];
unsigned char indiceLCD=0;
int numeroLCD;

unsigned char cifra0=0;
unsigned char cifra1=0;
unsigned char cifra2=0;




// ******************* Memoria SD **************************+

bool	iniSD=false;
unsigned long int addressImg= 153600;


// ***********************************************************
//				Touch Screen
unsigned int coordX=0;
unsigned int coordY=0;

// **********************************************************



int dato16=0;
unsigned char dato8=0;


void demoraPulso(){
	int a=0;
/*	for(int i=0; i< 10/2; i++)
		a= i+2;
*/
}


// Setea la direcciones de los datos en el Bus. Ejemplo: dato=0xff -> Pines como salida
void busDirDisplay(int dato){
	union LCD16bits display;
	display.numero = dato;
	DIR15(display.bus16.d15);
	DIR14(display.bus16.d14);
	DIR13(display.bus16.d13);
	DIR12(display.bus16.d12);
	DIR11(display.bus16.d11);
	DIR10(display.bus16.d10);
	DIR9(display.bus16.d9);
	DIR8(display.bus16.d8);
	DIR7(display.bus16.d7);
	DIR6(display.bus16.d6);
	DIR5(display.bus16.d5);
	DIR4(display.bus16.d4);
	DIR3(display.bus16.d3);
	DIR2(display.bus16.d2);
	DIR1(display.bus16.d1);
	DIR0(display.bus16.d0);
}


void busDataDisplay(int dato){
	union LCD16bits display;
	display.numero = dato;
	D15(display.bus16.d15);
	D14(display.bus16.d14);
	D13(display.bus16.d13);
	D12(display.bus16.d12);
	D11(display.bus16.d11);
	D10(display.bus16.d10);
	D9(display.bus16.d9);
	D8(display.bus16.d8);
	D7(display.bus16.d7);
	D6(display.bus16.d6);
	D5(display.bus16.d5);
	D4(display.bus16.d4);
	D3(display.bus16.d3);
	D2(display.bus16.d2);
	D1(display.bus16.d1);
	D0(display.bus16.d0);
}


void busDataDisplay8(unsigned short dato){
	union LCD8bits display;
	display.numero = dato;
	D7(display.bus8.d7);
	D6(display.bus8.d6);
	D5(display.bus8.d5);
	D4(display.bus8.d4);
	D3(display.bus8.d3);
	D2(display.bus8.d2);
	D1(display.bus8.d1);
	D0(display.bus8.d0);
}


int noOpera(unsigned long int valor){
	return 0;
}


#define PANTALLAS_MENU	3
#define BOTONES_MENU		20  

unsigned char pantallaSel=0;	  //supongo no superar las 255 pantallas
unsigned char pantallaMenu=0;
unsigned long int valorMenu=0;  //valor utilizado para el argumento de la función de Menú

char listadoImagenesMenu[BOTONES_MENU] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };  // 20 posibles botones de 60x60 en 3 Pantallas de 240x320 pixeles

int(*pFuncionesMenu[BOTONES_MENU])(unsigned long int) = { \
noOpera,				// Funcion[indiceFuncion=0]
noOpera, 			// Funcion[indiceFuncion=1]
noOpera,				// Funcion[indiceFuncion=2]
noOpera, 	 		// Funcion[indiceFuncion=3]
noOpera,				// Funcion[indiceFuncion=4]
noOpera, 			// Funcion[indiceFuncion=5]
noOpera,				// Funcion[indiceFuncion=6]
noOpera, 	 		// Funcion[indiceFuncion=7]
noOpera,				// Funcion[indiceFuncion=8]
noOpera, 			// Funcion[indiceFuncion=9]
noOpera,				// Funcion[indiceFuncion=10]
noOpera, 	 		// Funcion[indiceFuncion=11]
noOpera,				// Funcion[indiceFuncion=12]
noOpera, 			// Funcion[indiceFuncion=13]
noOpera,				// Funcion[indiceFuncion=14]
noOpera, 	 		// Funcion[indiceFuncion=15]
noOpera,				// Funcion[indiceFuncion=16]
noOpera, 			// Funcion[indiceFuncion=17]
noOpera,				// Funcion[indiceFuncion=18]
noOpera };			// Funcion[indiceFuncion=19]







#include "funciones.c"
#include "config.c"
#include "interrupt.c"


#include "touchScreen.c"
#include "transceiver.c"


#ifdef DISPLAY_GRAFICO_CONECTADO
#include "lcd320x240.c"
#endif



#include "convAritmeticas.c"

#include "arrayFunciones.c"


#ifdef DISPLAY_GRAFICO_CONECTADO
#include "lcdGrafico_AltoNivel.c"
#endif




bool interpretarMenu(){
	unsigned char i=0;	
	for(i=0; i < PANTALLAS_MENU; i++){
		if(listadoImagenesMenu[i] > -1){
			//cambiarEstadoBoton(i);
			delay_rit_ms(200);
			//cambiarEstadoBoton(i);
			pFuncionesMenu[i](valorMenu);  // ejecutar función asociada al botón
		}
	}
	return true;
}



bool interpretarComando(){
	unsigned long int indice=0;
	unsigned long int valor=0;
	int resultado=0;

	if(instruccionPendiente > 0){
		indice= cmdProcesar[proximoCmd][0];
		valor= cmdProcesar[proximoCmd][1];

		if(indiceFunciones < FUNCIONES_VECTORIZADAS){
			resultado= pInstrucciones[indice](valor);
			}
		proximoCmd++;
		instruccionPendiente--;
		if(proximoCmd == BUFFER_INSTRUCCIONES_SERIE)
			proximoCmd=0;

//		sprintf_mio(aux,"\n indiceFunciones: %d   valorFuncion: %d ", indice, valor);
//		DEBUGSTR(aux);	
//		limpiaBufferAUX();
		return true;
	}
	return false;
}


bool inicializaBufferSerie(){
	for(int i=0; i < BUFFER_RX_UART2; i++ )
		dataRx_UART2[i]=0;
	return true;
}


int main(void)
{

	int tiempo=4;
	unsigned char contador=0;
	unsigned int i=0,j=0,k=0;
	unsigned long int addressInicial=0;
	int valor=0;
	float a=0; 
	bool flag= true;
	float respuesta_M4=0;
	char rta[2];
	uint8_t data[32];  // aaray utilizado para la lectura de registros de la microSD
	unsigned char contadorIntentos=0; // variable para contar los intentos de conexión con la memoria microSD

	unsigned char tmp=0;


	Cfg_Led();
	Cfg_PinesDisplayGrafico();
	cfgRITTimer();
	Cfg_UART2();
	InicializarIPC();
	inicializaBufferSerie();
	limpiaBufferUART2();

	WR(1);


	sprintf_mio(aux,"\n *** INICIANDO CORTEX M0 *** ");
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	i=0; 
	sprintf_mio(aux,"\n *** LCD GRAFICO INICIALIZADO *** ");
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	i=0; 
	sprintf_mio(aux,"\n Pulso... ");
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	spiInit(0);

	initSDcard(0);

//	spiInit(2);

	do{
		read_CID(data); 
		show_CID_data(data);
		read_CSD(data); 
		delay_rit_ms(1000);
		sprintf_mio(aux,"\n Intento Nro: %d de conexion SPI con memoria microSD. Espacio: %d ", contadorIntentos, show_CSD_data(data));
		DEBUGSTR(aux);	
		limpiaBufferAUX();
		contadorIntentos++;
	}while( (show_CSD_data(data) != 971 ) && (show_CSD_data(data) != 1023 ) && ( contadorIntentos < 10) );

	if(contadorIntentos < 5){
		sprintf_mio(aux,"\n *** SPI Inicializado y Reconfigurado a 400kHz*** ");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	else{
		sprintf_mio(aux,"\n --- ERROR: Fallo la configuracion SPI ");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}

#ifdef DISPLAY_GRAFICO_CONECTADO

	ili9325_Init();
	propfuente(1);

	clearScreenFull(0);
	addressInicial= ADDR_PORTADA;
	setAddressSD(addressInicial);  // Dirección de memoria de portada
	cargarImagenFull(addressSD);
	delay_rit_ms(2000);
	printBoton(8,60,120);




/* Ayuda Memoria de Menues

0- Portada	 				1075200
1- Paella	 				1228800
2- Bife		 				1382400
3- Canelones 				1536000
4- Pollo		 				1689600
5- Filet		 				1843200
6- Ñoquis	 				1996800
7- Postre 	 				2150400
8- ConfirmaPedirCuenta 	2304000


*/

//	cargarArraySD(0);
//	setFont(MemoriaIPC.mensaje);

/*
	printValor(36, 40, 40);
	delay_rit_ms(500);
	printValor(37, 40, 40);
	delay_rit_ms(500);
	printValor(38, 40, 40);
*/
//	printBitmap(0,0,24,32, &bitmap[0]);
//	nuevoTanque(cifra0 , 0, 0, 239, 159,0,0,0,1,255,1);
//	nuevoTanque(cifra1 , 0, 160, 239, 159,0,0,0,255,1,1);
#endif

	while(1){

	//obtenerValorTouch(1);
	leerTouch( &coordX , &coordY );

	validarSectorTouch();

	adminBotones();

	delay_rit_ms(5);

		interpretarComando();
		
		if(MemoriaIPC.comando2_M0 == PENDIENTE && MemoriaIPC.estado_M4 == NO_BUSY){
			MemoriaIPC.comando2_M0= STOP;
			respuesta_M4= (int)MemoriaIPC.valor_M0;
			if(respuesta_M4 == 1)
				sprintf_mio(aux,"\n Respuesta de la Red Neuronal en Cortex M0: SI \n\n");
			else
				sprintf_mio(aux,"\n Respuesta de la Red Neuronal en Cortex M0: NO \n\n");
			
			DEBUGSTR(aux);	
			limpiaBufferAUX();
		}


		if(sensor == 1){
			sprintf_mio(aux,"\n R: %d \n",MemoriaIPC.mensaje[0]);
			sensor=0;

			DEBUGSTR(aux);	
			limpiaBufferAUX();
		}

	}

}

/* ************************* EOF M0 *************************** */
