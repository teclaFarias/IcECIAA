// Funciones de Alto Nivel


void limpiarPantallaLCD(){
	setCoordIniXY(0);
	setCoordFinXY( ((200 << 16)&(0xffff0000)) | (163 & 0xff));
	colorRGBPixel( ((250 << 16)&(0x00ff0000)) | ((250 << 8)&(0x0000ff00)) | (250 & 0xff) );
	setColorDisplay( ((250 << 16)&(0x00ff0000)) | ((250 << 8)&(0x0000ff00)) | (250 & 0xff) );
	imprimirLCD(RTG_RELLENO);
}



void insertarMenuIconos(unsigned char verTodos){

	if((sectorHabilitado[9] == 1) && (sectorHabilitado[10] == 1) && (pantallaSel == 0) && ((sectorPresionado[9] == 1) ||(sectorPresionado[10] == 1) ||(verTodos == 1)) ){
		printBoton(8,120,60);
		sectorPresionado[9] = 0;
		sectorPresionado[10] = 0;
	}

	if((sectorHabilitado[16] == 1) && (pantallaSel == 1) && ((sectorPresionado[16] == 1) ||(verTodos == 1)) ){
		printIcono(0,1,260);
		sectorPresionado[16]= 0;
	}

	if((sectorHabilitado[17] == 1) && (pantallaSel == 1) && ((sectorPresionado[17] == 1) ||(verTodos == 1)) ){
		printIcono(4,59,260);
		sectorPresionado[17]= 0;
	}

	if((sectorHabilitado[18] == 1) && (pantallaSel == 1) && ((sectorPresionado[18] == 1) ||(verTodos == 1)) ){
		printIcono(6,120,260);
		sectorPresionado[18] = 0;
	}

	if((sectorHabilitado[19] == 1) && (pantallaSel == 1) && ((sectorPresionado[19] == 1) ||(verTodos == 1)) ){
		printIcono(1,180,260);
		sectorPresionado[19] = 0;
	}




}





void adminBotones(){
	unsigned char sector=0;



	if(pantallaSel == 2){

// Acepta Pedir la cuenta
		sector= 16;
		if(sectorHabilitado[sector] && sectorHabilitado[sector + 1]){
			if(sectorPresionado[sector] || sectorPresionado[sector + 1]){
				printBoton(14,1,260);

				for(unsigned char c=0; c<10; c++){
					Led_ON(3);
					Led_OFF(5);
					delay_rit_ms(250);	
					Led_OFF(3);
					Led_ON(5);
					delay_rit_ms(250);
					Led_OFF(5);
				}

				pantallaSel= 0; // Seteo la pantalla para mostrar el primer slide
				addressSD= ADDR_PORTADA;
				delay_rit_ms(50);	
				cargarImagenFull(addressSD);
				delay_rit_ms(2000);	
				printBoton(8,60,120);

			}
		}

// Cancela Pedir la cuenta
		sector= 18;
		if(sectorHabilitado[sector] && sectorHabilitado[sector + 1]){
			if(sectorPresionado[sector] || sectorPresionado[sector + 1]){
				printBoton(18,120,260);
				sectorHabilitado[16]= 1;
				sectorHabilitado[17]= 1;
				sectorHabilitado[18]= 1;
				sectorHabilitado[19]= 1; 
				pantallaSel= 1; // Seteo la pantalla para mostrar el primer slide
				addressSD= ADDR_SLIDER_0;
				cargarImagenFull(addressSD);
				delay_rit_ms(50);	
				insertarMenuIconos(1);
			}
		}

	}



	if(pantallaSel == 1){

// Plato Anterior
		sector= 16;
		if(sectorHabilitado[sector]){
			if(sectorPresionado[sector]){
				printIcono(2,1,260);

				siguienteImagen(1);	// Slide Anterior
				printBitmapCustom(addressSD, 0, 0, 0, 0, 240,260 );
				delay_rit_ms(50);	
				insertarMenuIconos(0);

			}
		}

// Solicita al Mozo
		sector= 17;
		if(sectorHabilitado[sector]){
			if(sectorPresionado[sector]){
				printIcono(5,59,260);

				for(unsigned char b=0; b<10; b++){
					Led_ON(2);
					delay_rit_ms(350);	
					Led_OFF(2);
					delay_rit_ms(350);	
				}
	
				delay_rit_ms(50);	
				insertarMenuIconos(0);

			}
		}

// Pedir la cuenta
		sector= 18;
		if(sectorHabilitado[sector]){
			if(sectorPresionado[sector]){
				printIcono(7,120,260);
				
				pantallaSel= 2; // Pantalla de confirmación de "Pedir La Cuenta"
				addressSD= ADDR_CONFIRMA_CUENTA;
				printBitmapFull(addressSD);
				printBoton(12,1,260);
				printBoton(16,120,260);

				delay_rit_ms(50);	
				//insertarMenuIconos(0);

			}
		}

// Siguiente Plato
		sector= 19;
		if(sectorHabilitado[sector]){
			if(sectorPresionado[sector]){
				printIcono(3,180,260);

				siguienteImagen(0);   // Slide Siguiente
				printBitmapCustom(addressSD, 0, 0, 0, 0, 240,260 );
				delay_rit_ms(50);
				insertarMenuIconos(0);
			}
		}
	}



	if(pantallaSel == 0){
		sector= 9;
		if(sectorHabilitado[sector] && sectorHabilitado[sector + 1]){
			if(sectorPresionado[sector] || sectorPresionado[sector + 1]){
				printBoton(10,60,120);

				pantallaSel=1;
				sectorHabilitado[sector]= 0;
				sectorHabilitado[sector + 1]= 0;
				sectorHabilitado[16]= 1;
				sectorHabilitado[17]= 1;
				sectorHabilitado[18]= 1;
				sectorHabilitado[19]= 1; 

				addressSD= ADDR_SLIDER_0;
				printBitmapCustom(addressSD, 0, 0, 0, 0, 240,320 );
				insertarMenuIconos(1);

				delay_rit_ms(50);	


			}
		}
	}


}







/*
void setCifra(int cifra , int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char rb,unsigned char gb,unsigned char bb){

	unsigned char vtmpL= (unsigned char)cifra & 0x00ff;
	unsigned char vtmpU= (unsigned char)((cifra >> 8) & 0x00ff);

	setCoordIniXY((unsigned char)x & 0x00ff);
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
*/

/*

void nuevoTanque(int nivel, int x, int y, int ancho, int alto, unsigned char r, unsigned char g, unsigned char b, unsigned char rb,unsigned char gb,unsigned char bb){
	int proporcion=0;

	nivel= (int)nivel*100/255;
	
	setCoordIniXY((unsigned char)x & 0x00ff);
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
	setCoordIniXY((unsigned char)(x+1) & 0x00ff);
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

	setCoordIniXY((unsigned char)(x+1) & 0x00ff);
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

*/

