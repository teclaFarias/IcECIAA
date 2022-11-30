// Array de Funciones

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
#ifdef DISPLAY_GRAFICO_CONECTADO
	setColor(lcdRcolor, lcdGcolor, lcdBcolor);
#endif
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
#ifdef DISPLAY_GRAFICO_CONECTADO
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
#endif
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
#ifdef DISPLAY_GRAFICO_CONECTADO
	setBackColor(lcdRBcolor, lcdGBcolor, lcdBBcolor);
#endif
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


int initSDcard(unsigned char dato){
	sprintf_mio(aux,"\n\r Iniciando MicroSD...\n\r");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	if(initSD() == true){
		sprintf_mio(aux,"\n\r Inicializacion Finalizada OK \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	return 36;
}


int infoSDcard(unsigned char dato){
	sprintf_mio(aux,"\n\r Obteniendo Informacion de Registros de MicroSD...\n\r");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	if(lecturaRegSD() == true){
		sprintf_mio(aux,"\n\r Lectura de Registros OK \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	} 
	return 37;
}


int imgToSDcard(unsigned char dato){

	sprintf_mio(aux,"Subiendo Imagen a SD Card \n\r");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

		for(int i=0; i < 512; i++){
			if(i< 255)
				str[i]= 0x50; //i;
			else
				str[i]= 0x99;  //i - 256;
		}				

	if(subirImagenSD() == true){
		sprintf_mio(aux,"\r\n Transferencia de Imagen a SD Card Finalizada OK. \r\n");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	return 38;
}



int readImgSDcard(unsigned char dato){

	sprintf_mio(aux,"Leyendo Imagen SD Card \n\r");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	if(leerImagenSD() == true){
		sprintf_mio(aux,"\n\r Lectura de Imagen Tarjeta SD OK \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	return 39;
}

// Byte menos significativo para formar una cantidad de 32 bits
int valor0_3(unsigned char dato){
	cant0_3= dato;
	return 40;
}


int valor1_3(unsigned char dato){
	cant1_3= dato;
	cant1_3 <<= 8;
	return 41;
}


int valor2_3(unsigned char dato){
	cant2_3= dato;
	cant2_3 <<= 16;
	return 42;
}


int valor3_3(unsigned char dato){
	cant3_3= dato;
	cant3_3 <<= 24;
	return 43;
}


int defineValor32bits(unsigned char dato){
	valor32bits= cant3_3 + cant2_3 + cant1_3 + cant0_3;
	return 44;
}


int cantBloquesSD(unsigned char dato){
	cantBloques= valor32bits;
	sprintf_mio(aux,"Cantidad de Bloques en SD Card: %d \n\r",cantBloques);
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	return 45;
}


int agregarByteSDCard(unsigned char dato){
	str[indiceBloqueSD]= dato;
	indiceBloqueSD++;
	if(indiceBloqueSD == BUFFER_SERIE){
		indiceBloqueSD=0;
		sprintf_mio(aux,"Enviando Bloque Nro: %d --->>> SD Card \n\r",indiceSD);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
		importarImagenSD();		
	}
	return 46;

}


int cargarBufferSerie(unsigned char dato){
	cargarBuffer= true;
	estadoBufferSerie= false;
	indiceSerie= 0;
	return 47;
}


int demoraMiliSegundos(unsigned char dato){
	cantBloques= valor32bits;
	delay_rit_ms(cantBloques);
	return 45;
}

int leerBuffer(unsigned char dato){
	dump_bytes(bufferSerie, BUFFER_SERIE);
	return 49;
}





// FIn de Funciones Vectorizadas

#define  FUNCIONES_VECTORIZADAS  50

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
setNivelTanque2,		// Funcion[indiceFuncion=35]
initSDcard,				// Funcion[indiceFuncion=36]
infoSDcard,				// Funcion[indiceFuncion=37]
imgToSDcard,			// Funcion[indiceFuncion=38]
readImgSDcard,			// Funcion[indiceFuncion=39]
valor0_3,				// Funcion[indiceFuncion=40]
valor1_3,				// Funcion[indiceFuncion=41]
valor2_3,				// Funcion[indiceFuncion=42]
valor3_3,				// Funcion[indiceFuncion=43]
defineValor32bits,	// Funcion[indiceFuncion=44]
cantBloquesSD,			// Funcion[indiceFuncion=45]
agregarByteSDCard,	// Funcion[indiceFuncion=46]
cargarBufferSerie,	// Funcion[indiceFuncion=47]
demoraMiliSegundos,	// Funcion[indiceFuncion=48]
leerBuffer           // Funcion[indiceFuncion=49]
};

