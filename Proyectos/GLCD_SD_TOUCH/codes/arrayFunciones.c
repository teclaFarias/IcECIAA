// Array de Funciones

int SinOperar(unsigned long int i){
	return 0;
}

int apagarLed(unsigned long int led){
	Led_OFF((unsigned char)led);
	return 1;
}

int encenderLed(unsigned long int led){
	Led_ON((unsigned char)led);
	return 2;
}


int nuevoElementoR(unsigned long int valor){
	MemoriaIPC.rgb[0]= (unsigned char) valor;	
			sprintf_mio(aux,"Color Rojo: %d \n", valor);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 3;
}

int nuevoElementoG(unsigned long int valor){
	MemoriaIPC.rgb[1]= (unsigned char) valor;	
			sprintf_mio(aux,"Color Verde: %d \n", valor);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 4;
}

int nuevoElementoB(unsigned long int valor){
	MemoriaIPC.rgb[2]= (unsigned char) valor;	
			sprintf_mio(aux,"Color Azul: %d \n", valor);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 5;
}

int procesarRGB(unsigned long int valor){
	MemoriaIPC.comando2_M4= ELEMENTO_RGB;	
			sprintf_mio(aux,"Procesando datos... \n");
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 6;
}


int leerSensorRGB(unsigned long int valor){
	MemoriaIPC.comando2_M4= OBTENER_COLOR;	
			sprintf_mio(aux,"Obtener lectura del sensorRGB... \n");
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 7;
}


int dato8BitsDisplay(unsigned long int dato){
	dato8= dato;
			sprintf_mio(aux,"Se carga dato bajo: %d \n", dato);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 8;
}


int dato16BitsDisplay(unsigned long int dato){
	dato16= dato8 + (dato << 8);
	busDataDisplay(dato16);
			sprintf_mio(aux,"Envio dato de 16 bits: %d \n", dato);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 9;
}



int setCoordIniXY(unsigned long int dato){
	posIniX= (int) ((dato & 0xffff0000) >> 16);
	posIniY= (int) (dato & 0x0000ffff);
	return 10;
}


int setCoordFinXY(unsigned long int dato){
	posFinX= (int) ( (dato >> 16) & 0x0000ffff);
	posFinY= (int) (dato & 0x0000ffff);
	sprintf_mio(aux,"\n Coord X: %d    Coord Y: %d \n", posFinX, posFinY);
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	return 11;
}


int cargarIcono(unsigned long int dato){
	cargarIconoSD(dato);
	return 12;
}


int obtenerValorTouch(unsigned long int dato){

	leerTouch( &coordX , &coordY );
	analizarSectoresTouch( &coordX , &coordY );
	return 13;
}


int cargarImagenFull(unsigned long int address){
	printBitmapFull( address);
	return 14;
}


int cagarImagenEnSD(unsigned long int dato){
	writeMultiBlockSD(dato);

	return 15;
}


int insertarIconos(unsigned long int address){
// Esta función carga los iconos correspondientes segun la posición de memoria
// de la imagen que está leyendo.
// Si la dirección es la primera de las imágenes, muestra un solo ícono.
/*
	if((sectorHabilitado[0] == 1) && (pantalaSel == 0)){
		printBoton(8,120,60);
	}

	if((sectorHabilitado[1] == 1) && (pantalaSel == 1)){
		printIcono(0,2,260);
	}

	if((sectorHabilitado[2] == 1) && (pantalaSel == 1)){
		printIcono(2,179,260);
	}  */
	return 16;
}


int siguienteImagen(unsigned long int sentido){
	if(sentido == 0){
		addressSD += 153600;
	}
	if(sentido == 1){
		if(addressSD > 307200){
			addressSD -= 153600;
		}
	}
	return 17;
}


int nuevoChar(unsigned long int dato){
	LCDbuffer[indiceLCD]= (unsigned char) dato;
	indiceLCD++;
	if(indiceLCD == BUFFERLCD)
		indiceLCD=0;
	return 18;
}


int borrarChar(unsigned long int dato){
	return 19;
}


int setColorDisplay(unsigned long int dato){
#ifdef DISPLAY_GRAFICO_CONECTADO
	setColor((unsigned char) ((dato & 0x00ff0000) >> 16), (unsigned char) ((dato & 0x0000ff00) >> 8), (unsigned char) (dato & 0x000000ff));
#endif
	return 20;
}


int setLongitud(unsigned long int dato){
	longLinea= (int) dato;
	return 21;
}


int vacante7(unsigned long int dato){
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

int imprimirLCD(unsigned long int dato){
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


int colorRGBPixel(unsigned long int dato){
	lcdRcolor=(unsigned char) ((dato & 0x00ff0000) >> 16);
	lcdGcolor=(unsigned char) ((dato & 0x0000ff00) >> 8);
	lcdBcolor=(unsigned char) (dato & 0x000000ff);
	return 24;
}


int vacante8(unsigned long int dato){
	lcdGcolor= dato;
	return 25;
}


int vacante9(unsigned long int dato){
	lcdBcolor= dato;
	return 26;
}


int vacante10(unsigned long int dato){
	lcdRBcolor= dato;
	return 27;
}


int colorRGBPixelBG(unsigned long int dato){
	lcdRBcolor=(unsigned char) ((dato & 0x00ff0000) >> 16);
	lcdGBcolor=(unsigned char) ((dato & 0x0000ff00) >> 8);
	lcdBBcolor=(unsigned char) (dato & 0x000000ff);
	return 28;
}


int vacante11(unsigned long int dato){
	lcdBBcolor= dato;
	return 29;
}


int setBackColorDisplay(unsigned long int dato){
#ifdef DISPLAY_GRAFICO_CONECTADO
	setBackColor(lcdRBcolor, lcdGBcolor, lcdBBcolor);
#endif
	return 30;
}


int setNumLcd(unsigned long int dato){
	numeroLCD= (int)(dato & 0x0000ffff);
	return 31;
}


int vacante12(unsigned long int dato){
	numeroLCD= dato;
	return 32;
}


int setNivelTanque0(unsigned long int dato){
	cifra0= (unsigned char) dato;
	return 33;
}


int setNivelTanque1(unsigned long int dato){
	cifra1= (unsigned char) dato;
	return 34;
}


int setNivelTanque2(unsigned long int dato){
	cifra2= (unsigned char) dato;
	return 35;
}


int initSDcard(unsigned long int dato){
	unsigned char contador= 0;
	unsigned char cant_intentos=10;
	unsigned char frec=0;
	bool salida= false;
	sprintf_mio(aux,"\n Comenzando Inicializacion MicroSD...\n");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	do{
		salida= initSD(frec);
		delay_rit_ms(500);
		contador++;
		if((contador == cant_intentos) && (frec < 3)){
			frec++;
			contador=0;

			sprintf_mio(aux,"\n Intentando Frecuencia: %d \n", frec);
			DEBUGSTR(aux);	
			limpiaBufferAUX();

		}
	}while((contador < cant_intentos) && (salida== false));

	if(contador == cant_intentos){
		sprintf_mio(aux,"\n Inicializacion MicroSD Fallida: %d intentos \n", contador);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	else{
		sprintf_mio(aux,"\n MicroSD Inicializada: %d intentos - Tipo de Frecuencia: %d \n", contador, frec);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}


	return 36;
}


int infoSDcard(unsigned long int dato){
	sprintf_mio(aux,"\n Obteniendo Informacion de Registros de MicroSD...\n");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	if(lecturaRegSD() == true){
		sprintf_mio(aux,"\n Lectura de Registros OK \n");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	} 
	return 37;
}


int imgToSDcard(unsigned long int dato){

	sprintf_mio(aux,"Subiendo Imagen a SD Card \n");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

		for(int i=0; i < 512; i++){
				str[i]= dato; //i;
		}				

	if(subirImagenSD() == true){
		sprintf_mio(aux,"\r\n Transferencia de Imagen a SD Card Finalizada OK. \r\n");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	return 38;
}



int readImgSDcard(unsigned long int address){

	sprintf_mio(aux,"Leyendo Imagen SD Card \n");
	DEBUGSTR(aux);	
	limpiaBufferAUX();
/*
	if(!readBlockSD(address)){
		sprintf_mio(aux,"\r\n Ocurrio un Error al leer la direccion: %d \r\n", address);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
		return 39;
	}*/

	dump_bytes(&MemDisplay.buffer, 10240);   //(str, 512);
	return 39;
}

// Byte menos significativo para formar una cantidad de 32 bits
int valor0_3(unsigned long int dato){
	cant0_3= dato;
	return 40;
}


int valor1_3(unsigned long int dato){
	cant1_3= dato;
	cant1_3 <<= 8;
	return 41;
}


int valor2_3(unsigned long int dato){
	cant2_3= dato;
	cant2_3 <<= 16;
	return 42;
}


int valor3_3(unsigned long int dato){
	cant3_3= dato;
	cant3_3 <<= 24;
	return 43;
}


int defineValor32bits(unsigned long int dato){
	valor32bits= dato;
	sprintf_mio(aux,"Valor de 32bits: %d \n,", valor32bits);
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	return 44;
}


int cantBloquesSD(unsigned long int dato){
	cantBloques= valor32bits;
	sprintf_mio(aux,"Cantidad de Bloques en SD Card: %d \n",cantBloques);
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	return 45;
}


int agregarByteSDCard(unsigned long int dato){

	unsigned long int bloques= (addressSD / BUFFER_SERIE);
	unsigned long int addrBloque= bloques * BUFFER_SERIE;

	if(!writeBlockSD(addrBloque, &bufferSerie[0])){
		sprintf_mio(aux,"Se produjo un error copiando bloque: %d \n,", addrBloque);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	else{
		sprintf_mio(aux,"Copiado bloque: %d \n,", addrBloque);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}

	return 46;

}


int cargarBufferSerie(unsigned long int dato){
	indiceSerie= 0;
	cargarBuffer= true;
	estadoBufferSerie= false;
	while(!estadoBufferSerie); // espero a que se llene el Buffer
	sprintf_mio(aux,"OK| Buffer Completo\n");
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	return 47;
}


int demoraMiliSegundos(unsigned long int dato){
	delay_rit_ms((int)dato);
	return 48;
}

int leerBuffer(unsigned long int dato){
	dump_bytes(bufferSerie, BUFFER_SERIE);
	return 49;
}


int setAddressSD(unsigned long int dato){
	addressSD= dato;
	return 50;
}


int setElementoSD(unsigned long int dato){
	elementoSD= valor32bits;
	return 51;
}

int leerVectorSD(unsigned long int dato){
	unsigned long int bloqueElemento=0;
	unsigned long int desplazamiento=0;
	valorElementoSD= extraerByteSD(dato);
	sprintf_mio(aux," Direccion [%d] : %d   \n", addressSD, valorElementoSD);
	DEBUGSTR(aux);	
	limpiaBufferAUX();
	return 52;
}


int escribirVectorSD(unsigned long int dato){
	escribirByteSD((unsigned char)dato);
	return 53;
}


// FIn de Funciones Vectorizadas

#define  FUNCIONES_VECTORIZADAS  54

int(*pInstrucciones[FUNCIONES_VECTORIZADAS])(unsigned long int) = { \
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
setCoordIniXY,			// Funcion[indiceFuncion=10]
setCoordFinXY,			// Funcion[indiceFuncion=11]
cargarIcono,			// Funcion[indiceFuncion=12]
obtenerValorTouch,	// Funcion[indiceFuncion=13]
cargarImagenFull,		// Funcion[indiceFuncion=14]
cagarImagenEnSD,		// Funcion[indiceFuncion=15]
insertarIconos,			// Funcion[indiceFuncion=16]
siguienteImagen,		// Funcion[indiceFuncion=17]
nuevoChar,				// Funcion[indiceFuncion=18]
borrarChar,				// Funcion[indiceFuncion=19]
setColorDisplay,		// Funcion[indiceFuncion=20]
setLongitud,			// Funcion[indiceFuncion=21]
vacante7,					// Funcion[indiceFuncion=22]
imprimirLCD,			// Funcion[indiceFuncion=23]
colorRGBPixel,			// Funcion[indiceFuncion=24]
vacante8,					// Funcion[indiceFuncion=25]
vacante9,					// Funcion[indiceFuncion=26]
vacante10,					// Funcion[indiceFuncion=27]
colorRGBPixelBG,		// Funcion[indiceFuncion=28]
vacante11,			// Funcion[indiceFuncion=29]
setBackColorDisplay,	// Funcion[indiceFuncion=30]
setNumLcd,				// Funcion[indiceFuncion=31]
vacante12,				// Funcion[indiceFuncion=32]
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
leerBuffer,          // Funcion[indiceFuncion=49]
setAddressSD,			// Funcion[indiceFuncion=50]
setElementoSD,			// Funcion[indiceFuncion=51]
leerVectorSD,			// Funcion[indiceFuncion=52]
escribirVectorSD		// Funcion[indiceFuncion=53]
};

