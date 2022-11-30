// Funciones de Alto Nivel


void limpiarPantallaLCD(){
	setPosIniX_L(0);
	setPosIniX_U(0);
	setPosIniY_L(0);
	setPosIniY_U(0);
	setPosFinX_L(239);
	setPosFinX_U(0);
	setPosFinY_L(63);
	setPosFinY_U(1);
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

  

