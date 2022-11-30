/**********************************************************************************************
 * Trabajo Práctico - Electrónica Digital III
 * Profesores: Ing. Marcelo Romeo
 * Autor: Farías Darío Emmanuel
 * Email: defarias@gmail.com
 *
 * Proyecto: SensorRGB
 * Procesador utilizado: Cortex-M0 en modo APP
 * Descripción:

 *** En M0: El cortexM0 tiene implementado un protocolo de comunicación para ejecutar funciones. 
Dichas funciones poseen el prototipo: int nombre (unsigned char)
También posee una comunicación con el Cortex M4 a través de la estructura reservada en memoria 
y accedida por ambos procesadores.
Implementa una función para decodificar el nivel de intensidad lumínica RGB a través del sensor TCS3200.
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




char dataRx_UART2[BUFFER_RX_UART2];  // Buffer para armar el comando o valor recibido.
char * pRx_UART2= dataRx_UART2;      // Puntero para recorrer la cola.
int contadorRx_UART2=0;				    // Variable para recorrer con el puntero.
bool instruccionPendiente= false;    // Flag para ejecutar instrucción. True: Pendiente de atención.
unsigned char indiceFunciones=0;     // Selección de función a ejecutar.
unsigned char valorFuncion=0;        // Argumento de la función a ejecutar.

int sensor=0;


#include "funciones.c"
#include "config.c"
#include "interrupt.c"
#include "lcd320x240.c"



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

// ***********************************************************

int dato16=0;
unsigned char dato8=0;


void demoraPulso(){
	int a=0;
	for(int i=0; i< 10; i++)
		a= i+2;
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
	return 0;
}

int encenderLed(unsigned char led){
	Led_ON(led);
	return 1;
}

int apagarLed(unsigned char led){
	Led_OFF(led);
	return 2;
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


int leerSensorRGB(unsigned char valor){
	MemoriaIPC.comando2_M4= OBTENER_COLOR;	
			sprintf_mio(aux,"Obtener lectura del sensorRGB... \n\r");
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 7;
}


int dato8BitsDisplay(unsigned char dato){
	dato8= dato;
			sprintf_mio(aux,"Se carga dato bajo: %d \n\r", dato);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 8;
}


int dato16BitsDisplay(unsigned char dato){
	dato16= dato8 + (dato << 8);
	busDataDisplay(dato16);
			sprintf_mio(aux,"Envio dato de 16 bits: %d \n\r", dato);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 9;
}



int setPosIniX_U(unsigned char dato){
	posIniX= (int)((dato << 8) | posIniX);
	return 10;
}


int setPosIniX_L(unsigned char dato){
	posIniX= dato;
	return 11;
}


int setPosIniY_U(unsigned char dato){
	posIniY= (int)((dato << 8) | posIniY);
	return 12;
}


int setPosIniY_L(unsigned char dato){
	posIniY= dato;
	return 13;
}


int setPosFinX_U(unsigned char dato){
	posFinX= (int)((dato << 8) | posFinX);
	return 14;
}


int setPosFinX_L(unsigned char dato){
	posFinX= dato;
	return 15;
}


int setPosFinY_U(unsigned char dato){
	posFinY= (int)((dato << 8) | posFinY);
	return 16;
}


int setPosFinY_L(unsigned char dato){
	posFinY= dato;
	return 17;
}


int nuevoChar(unsigned char dato){
	LCDbuffer[indiceLCD]= dato;
	indiceLCD++;
	if(indiceLCD == BUFFERLCD)
		indiceLCD=0;
	return 18;
}


int borrarChar(unsigned char dato){
	return 19;
}


int setColorDisplay(unsigned char dato){
	setColor(lcdRcolor, lcdGcolor, lcdBcolor);
	return 20;
}


int setLongitud_U(unsigned char dato){
	longLinea= (int)(dato << 8);
	return 21;
}


int setLongitud_L(unsigned char dato){
	longLinea= dato;
	return 22;
}

#define	PIXEL				0	
#define	HLINEA			1
#define	VLINEA			2
#define	LINEA				3
#define	RTG_VACIO		4
#define	RTG_RELLENO		5
#define	CARACTER			6
#define	TEXTO				7
#define	VALOR				8

int imprimirLCD(unsigned char dato){
	unsigned char valorTmp=0;
	switch(dato){
		case 0:
			setColorDisplay(0);
			setPixel((int)(fch|fcl));
			break;
		case 1:
			drawHLine(posIniX, posIniY, longLinea);		//Imprime en Display LCD: Línea Horizontal
			break;
		case 2:
			drawVLine(posIniX, posIniY, longLinea);		//Imprime en Display LCD: Línea Vertical
			break;
		case 3:
			drawLine(posIniX, posIniY, posFinX, posFinY);		//Imprime en Display LCD: Línea
			break;
		case 4:
			drawRect(posIniX, posIniY, posFinX, posFinY);		//Imprime en Display LCD: Rectángulo con bordes rectos
			break;
		case 5:
			fillRect(posIniX, posIniY, posFinX, posFinY);		//Imprime en Display LCD: Rectángulo relleno con bordes rectos
			break;
		case 6:
			if(indiceLCD == 0)
				valorTmp= BUFFERLCD-1;
			else
				valorTmp= indiceLCD - 1;
			printChar(LCDbuffer[valorTmp], posIniX, posIniY);		//Imprime el último caracter agregado al buffer
			break;
		case 7:
			printLCD(*LCDbuffer, posIniX, posIniY, posAngular);		//Imprime el buffer
			break;
		case 8:
			printValor(numeroLCD, posIniX, posIniY);
		default:
			break;
	}
	return 23;
}


int colorRPixel(unsigned char dato){
	lcdRcolor= dato;
	return 24;
}


int colorGPixel(unsigned char dato){
	lcdGcolor= dato;
	return 25;
}


int colorBPixel(unsigned char dato){
	lcdBcolor= dato;
	return 26;
}


int colorRBPixel(unsigned char dato){
	lcdRBcolor= dato;
	return 27;
}


int colorGBPixel(unsigned char dato){
	lcdGBcolor= dato;
	return 28;
}


int colorBBPixel(unsigned char dato){
	lcdBBcolor= dato;
	return 29;
}


int setBackColorDisplay(unsigned char dato){
	setBackColor(lcdRBcolor, lcdGBcolor, lcdBBcolor);
	return 30;
}


int setNumLcdU(unsigned char dato){
	numeroLCD= (int)((dato << 8) | numeroLCD);
	return 31;
}


int setNumLcdL(unsigned char dato){
	numeroLCD= dato;
	return 32;
}


int setNivelTanque0(unsigned char dato){
	cifra0= dato;
	return 33;
}


int setNivelTanque1(unsigned char dato){
	cifra1= dato;
	return 34;
}


int setNivelTanque2(unsigned char dato){
	cifra2= dato;
	return 35;
}


// ************************************** Conversión de Números a String **************************


void reverse(char *str, int len) 
{ 
    int i=0, j=len-1, temp; 
    while (i<j){ 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; j--; 
    } 
} 

int intToStr(int x, char str[], int d){ 
    int i = 0; 
    while(x){ 
	    str[i++] = (x%10) + '0'; 
	    x = x/10; 
	 } 
    while(i < d) 
    	str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
} 

void ftoa(float n, char *res, int afterpoint) 
{ 
    int ipart = (int)n; 
    float fpart = n - (float)ipart; 
    int i = intToStr(ipart, res, 0); 
    if (afterpoint != 0){ 
    	res[i] = '.';  
      fpart = fpart * pow(10, afterpoint); 
      intToStr((int)fpart, res + i + 1, afterpoint); 
    } 
} 
// *********************************************************************************************


void convert_float(char *buf, double num, int width, unsigned char prec){
   ftoa(num, buf, 4); 
}




// FIn de Funciones Vectorizadas

#define  FUNCIONES_VECTORIZADAS  36

int(*pInstrucciones[FUNCIONES_VECTORIZADAS])(unsigned char) = { \
SinOperar,				// Funcion[indiceFuncion=0]
apagarLed, 				// Funcion[indiceFuncion=1]
encenderLed,			// Funcion[indiceFuncion=2]
nuevoElementoR,  		// Funcion[indiceFuncion=3]
nuevoElementoG,  		// Funcion[indiceFuncion=4]
nuevoElementoB,  		// Funcion[indiceFuncion=5]
procesarRGB,			// Funcion[indiceFuncion=6]
leerSensorRGB,			// Funcion[indiceFuncion=7]
dato8BitsDisplay,		// Funcion[indiceFuncion=8]
dato16BitsDisplay,	// Funcion[indiceFuncion=9]
setPosIniX_U,			// Funcion[indiceFuncion=10]
setPosIniX_L,			// Funcion[indiceFuncion=11]
setPosIniY_U,			// Funcion[indiceFuncion=12]
setPosIniY_L,			// Funcion[indiceFuncion=13]
setPosFinX_U,			// Funcion[indiceFuncion=14]
setPosFinX_L,			// Funcion[indiceFuncion=15]
setPosFinY_U,			// Funcion[indiceFuncion=16]
setPosFinY_L,			// Funcion[indiceFuncion=17]
nuevoChar,				// Funcion[indiceFuncion=18]
borrarChar,				// Funcion[indiceFuncion=19]
setColorDisplay,		// Funcion[indiceFuncion=20]
setLongitud_U,			// Funcion[indiceFuncion=21]
setLongitud_L,			// Funcion[indiceFuncion=22]
imprimirLCD,			// Funcion[indiceFuncion=23]
colorRPixel,			// Funcion[indiceFuncion=24]
colorGPixel,			// Funcion[indiceFuncion=25]
colorBPixel,			// Funcion[indiceFuncion=26]
colorRBPixel,			// Funcion[indiceFuncion=27]
colorGBPixel,			// Funcion[indiceFuncion=28]
colorBBPixel,			// Funcion[indiceFuncion=29]
setBackColorDisplay,	// Funcion[indiceFuncion=30]
setNumLcdU,				// Funcion[indiceFuncion=31]
setNumLcdL,				// Funcion[indiceFuncion=32]
setNivelTanque0,		// Funcion[indiceFuncion=33]
setNivelTanque1,		// Funcion[indiceFuncion=34]
setNivelTanque2		// Funcion[indiceFuncion=35]
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


// Funciones de Alto Nivel

void limpiarPantallaLCD(){
	setCoordIniXY();		//separa el argumento en parte alta (x) y parte baja (y)
	setCoordFinXY();     //lo mismo :)
	setPosIniX_L(0);
	setPosIniX_U(0);
	setPosIniY_L(0);
	setPosIniY_U(0);
	setPosFinX_L(239);
	setPosFinX_U(0);
	setPosFinY_L(63);
	setPosFinY_U(1);
	colorRGBPixel();  // En los 3 bytes más bajos encuentro RGB
	colorRPixel(250);
	colorGPixel(250);
	colorBPixel(250);
	setColorDisplay(0);
	imprimirLCD(RTG_RELLENO);
}


void setCifra(int cifra , int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char rb,unsigned char gb,unsigned char bb){

	unsigned char vtmpL= (unsigned char)cifra & 0x00ff;
	unsigned char vtmpU= (unsigned char)((cifra >> 8) & 0x00ff);

	setPosIniX_L((unsigned char)x & 0x00ff);
	setPosIniX_U((unsigned char)((x >> 8) & 0x00ff));
	setPosIniY_L((unsigned char)y & 0x00ff);
	setPosIniY_U((unsigned char)((y >> 8) & 0x00ff));
	colorRPixel(r);
	colorGPixel(g);
	colorBPixel(b);
	colorRBPixel(rb);
	colorGBPixel(gb);
	colorBBPixel(bb);
	setColorDisplay(0);
	setBackColorDisplay(0);
	setNumLcdL(vtmpL);
	setNumLcdU(vtmpU);
	imprimirLCD(VALOR);
}


void nuevoTanque(int nivel, int x, int y, int ancho, int alto, unsigned char r, unsigned char g, unsigned char b, unsigned char rb,unsigned char gb,unsigned char bb){
	int proporcion=0;

	nivel= (int)nivel*100/255;
	
	setPosIniX_L((unsigned char)x & 0x00ff);
	setPosIniX_U((unsigned char)((x >> 8) & 0x00ff));
	setPosIniY_L((unsigned char)y & 0x00ff);
	setPosIniY_U((unsigned char)((y >> 8) & 0x00ff));

	setPosFinX_L((unsigned char)(x + ancho) & 0x00ff);
	setPosFinX_U((unsigned char)(((x + ancho) >> 8) & 0x00ff));
	setPosFinY_L((unsigned char)(y + alto) & 0x00ff);
	setPosFinY_U((unsigned char)(((y + alto) >> 8) & 0x00ff));

	colorRPixel(0);
	colorGPixel(0);
	colorBPixel(0);
	colorRBPixel(rb);
	colorGBPixel(gb);
	colorBBPixel(bb);
	setColorDisplay(0);
	setBackColorDisplay(0);
	imprimirLCD(RTG_VACIO);

	colorRPixel(r);
	colorGPixel(g);
	colorBPixel(b);

	proporcion= nivel * alto / 100;

	// Pinto el contenido del tanque
	setPosIniX_L((unsigned char)(x+1) & 0x00ff);
	setPosIniX_U((unsigned char)(((x+1) >> 8) & 0x00ff));
	setPosIniY_L((unsigned char)(y+ 1 + alto -proporcion) & 0x00ff);
	setPosIniY_U((unsigned char)(((y+ 1 + alto -proporcion) >> 8) & 0x00ff));

	setPosFinX_L((unsigned char)(x + ancho -1) & 0x00ff);
	setPosFinX_U((unsigned char)(((x + ancho -1) >> 8) & 0x00ff));
	setPosFinY_L((unsigned char)(y + alto - 1) & 0x00ff);
	setPosFinY_U((unsigned char)(((y + alto - 1) >> 8) & 0x00ff));

	setColorDisplay(0);
	setBackColorDisplay(0);
	imprimirLCD(RTG_RELLENO);

	// Pinto el vacío del tanque
	colorRPixel(rb);
	colorGPixel(gb);
	colorBPixel(bb);

	setPosIniX_L((unsigned char)(x+1) & 0x00ff);
	setPosIniX_U((unsigned char)(((x+1) >> 8) & 0x00ff));
	setPosIniY_L((unsigned char)(y+ 1) & 0x00ff);
	setPosIniY_U((unsigned char)(((y+ 1) >> 8) & 0x00ff));

	setPosFinX_L((unsigned char)(x + ancho -1) & 0x00ff);
	setPosFinX_U((unsigned char)(((x + ancho -1) >> 8) & 0x00ff));
	setPosFinY_L((unsigned char)(y + alto - proporcion - 1) & 0x00ff);
	setPosFinY_U((unsigned char)(((y + alto - proporcion - 1) >> 8) & 0x00ff));

	setColorDisplay(0);
	setBackColorDisplay(0);
	imprimirLCD(RTG_RELLENO);


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
	Cfg_PinesDisplayGrafico();
	cfgRITTimer();
	Cfg_UART2();
	InicializarIPC();

	WR(1);


	sprintf_mio(aux,"\n\r *** INICIANDO CORTEX M0 *** ");
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	i=0; 
	sprintf_mio(aux,"\n\r *** LCD GRAFICO INICIALIZADO *** ");
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	i=0; 
	sprintf_mio(aux,"\n\r Pulso... ");
	DEBUGSTR(aux);	
	limpiaBufferAUX();


	ili9325_Init();
	propfuente(1);
	setFont(BigFont);
	limpiarPantallaLCD();

	printBitmap(0,0,24,32, &bitmap[0]);

	delay_rit_ms(5000);


	while(1){

/*

	setColor(255, 200, 170);
	printLCD("ADIOS...", 48, 190, 0);
*/


		setCifra(cifra0 , 0, 190, 2, 1, 1, 255,255,255);
		setCifra(cifra1 , 85, 190, 2, 1, 1, 255,255,255);
		setCifra(cifra2 , 170, 190, 2, 1, 1,255,255,255);


		nuevoTanque(cifra0 , 0, 60, 60, 128,255,0,0,255,255,255);
		nuevoTanque(cifra1 , 85, 60, 60, 128,0,255,0,255,255,255);
		nuevoTanque(cifra2 , 170, 60, 60,128,0,0,255,255,255,255);

	

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


		if(sensor == 1){
			sprintf_mio(aux,"\n\r R: %d \n\r",MemoriaIPC.mensaje[0]);
			sensor=0;

			DEBUGSTR(aux);	
			limpiaBufferAUX();
		}

	}

}

/* ************************* EOF M0 *************************** */
