/*
  UTFT.cpp - Multi-Platform library support for Color TFT LCD Boards
  Copyright (C)2015-2018 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  This library is the continuation of my ITDB02_Graph, ITDB02_Graph16
  and RGB_GLCD libraries for Arduino and chipKit. As the number of 
  supported display modules and controllers started to increase I felt 
  it was time to make a single, universal library as it will be much 
  easier to maintain in the future.

  Basic functionality of this library was origianlly based on the 
  demo-code provided by ITead studio (for the ITDB02 modules) and 
  NKC Electronics (for the RGB GLCD module/shield).

  This library supports a number of 8bit, 16bit and serial graphic 
  displays, and will work with both Arduino, chipKit boards and select 
  TI LaunchPads. For a full list of tested display modules and controllers,
  see the document UTFT_Supported_display_modules_&_controllers.pdf.

  When using 8bit and 16bit display modules there are some 
  requirements you must adhere to. These requirements can be found 
  in the document UTFT_Requirements.pdf.
  There are no special requirements when using serial displays.

  You can find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.

	Modify by Darío E. Farías for Edu-CIAA
	Fecha: 26/10/2018
*/


// *** Hardwarespecific defines ***

#define swap(type, i, j) {type t = i; i = j; j = t;}
#define pgm_read_byte(data) *data

#define fontbyte(x) pgm_read_byte(&cfont.font[x])  

#define regtype volatile uint8_t
#define regsize uint8_t
#define bitmapdatatype unsigned int*


// ***************************** Defines ************************************


#define UTFT_VERSION	283

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT 0
#define LANDSCAPE 1


#define ILI9325D_8		4
#define ILI9325D_16		5
#define HX8340B_8		6
#define HX8340B_S		7
#define HX8352A			8
#define ST7735			9
#define PCF8833			10
#define S1D19122		11
#define SSD1963_480		12
#define SSD1963_800		13
#define S6D1121_8		14
#define S6D1121_16		15
#define	SSD1289LATCHED	16
#define ILI9341_16		17
#define ILI9341_8       18
#define SSD1289_8		19
#define	SSD1963_800ALT	20
#define ILI9481			21
#define ILI9325D_16ALT	22
#define S6D0164			23
#define NOT_IN_USE		24
#define ILI9341_S5P		25
#define ILI9341_S4P		26
#define R61581			27
#define ILI9486			28
#define CPLD			29
#define HX8353C			30

#define ITDB32			0	// HX8347-A (16bit)
#define ITDB32WC		1	// ILI9327  (16bit)
#define ITDB32S			2	// SSD1289  (16bit)
#define CTE32			2	// SSD1289  (16bit)
#define ITDB24			3	// ILI9325C (8bit)
#define ITDB24D			4	// ILI9325D (8bit)
#define ITDB24DWOT		4	// ILI9325D (8bit)
#define ITDB28			4	// ILI9325D (8bit)
#define DMTFT24104      4   // ILI9325D (8bit)
#define DMTFT28103      4   // ILI9325D (8bit)
#define ITDB22			6	// HX8340-B (8bit)
#define ITDB22SP		7	// HX8340-B (Serial 4Pin)
#define ITDB32WD		8	// HX8352-A (16bit)
#define CTE32W			8	// HX8352-A	(16bit)
#define ITDB18SP		9	// ST7735   (Serial 5Pin)
#define LPH9135			10	// PCF8833	(Serial 5Pin)
#define ITDB25H			11	// S1D19122	(16bit)
#define ITDB43			12	// SSD1963	(16bit) 480x272
#define ITDB50			13	// SSD1963	(16bit) 800x480
#define CTE50			13	// SSD1963	(16bit) 800x480
#define EHOUSE50		13	// SSD1963	(16bit) 800x480
#define ITDB24E_8		14	// S6D1121	(8bit)
#define ITDB24E_16		15	// S6D1121	(16bit)
#define INFINIT32		16	// SSD1289	(Latched 16bit) -- Legacy, will be removed later
#define ELEE32_REVA		16	// SSD1289	(Latched 16bit)
#define CTE32_R2		17	// ILI9341	(16bit)
#define ITDB24E_V2_16   17  // ILI9341	(16bit)
#define ITDB28_V2_ALT   17  // ILI9341	(16bit)
#define ITDB32S_V2      17  // ILI9341	(16bit)
#define ITDB28_V2       18  // ILI9341	(8bit)
#define ITDB24E_V2_8    18  // ILI9341	(8bit) 
#define ELEE32_REVB		19	// SSD1289	(8bit)
#define CTE70			20	// SSD1963	(16bit) 800x480 Alternative Init
#define EHOUSE70		20	// SSD1963	(16bit) 800x480 Alternative Init
#define CTE32HR			21	// ILI9481	(16bit)
#define CTE28			22	// ILI9325D (16bit) Alternative Init
#define CTE22			23	// S6D0164	(8bit)
#define DMTFT22102      23  // S6D0164  (8bit)
#define DMTFT28105      25  // ILI9341  (Serial 5Pin)
#define MI0283QT9		26  // ILI9341	(Serial 4Pin)
#define CTE35IPS		27	// R61581	(16bit)
#define CTE40			28	// ILI9486	(16bit)
#define EHOUSE50CPLD	29	// CPLD		(16bit)
#define CTE50CPLD		29	// CPLD		(16bit)
#define CTE70CPLD		29	// CPLD		(16bit)
#define DMTFT18101      30  // HX8353C  (Serial 5Pin)

#define SERIAL_4PIN		4
#define SERIAL_5PIN		5
#define LATCHED_16		17

#define NOTINUSE		255

//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

// ********************* Fuentes ****************

#define fontdatatype    unsigned char

#include "DefaultFonts.c"

// **********************************************

struct _current_font{
	unsigned char * font;
	unsigned char x_size;
	unsigned char y_size;
	unsigned char offset;
	unsigned char numchars;
}cfont;


unsigned char			fch, fcl, bch, bcl;
unsigned char			orient;
long			disp_x_size, disp_y_size;
unsigned char			display_model, display_transfer_mode, display_serial_mode;
regtype			*P_RS, *P_WR, *P_CS, *P_RST, *P_SDA, *P_SCL, *P_ALE;
regsize			B_RS, B_WR, B_CS, B_RST, B_SDA, B_SCL, B_ALE;
unsigned char			__p1, __p2, __p3, __p4, __p5;

bool			_transparent;
unsigned char dato8bits=0;




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


void LCD_Writ_Bus(unsigned char VH,unsigned char VL, unsigned char mode){
		dato8= VL;
		dato16= dato8bits + (VH << 8);
		busDataDisplay(dato16);
		PULSO_WR(0); // pulso bajo en WR
}



void _set_direction_registers(unsigned char mode){
	busDirDisplay(255);
}



void _fast_fill_16(unsigned char VH,unsigned char VL, long pix){
	long blocks;

	dato8= VL;
	dato16= dato8bits + (VH << 8);
	busDataDisplay(dato16);

	blocks = pix/16;
	for (int i=0; i<blocks; i++){
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
		PULSO_WR(0);
	}
	if ((pix % 16) != 0)
		for (int i=0; i<(pix % 16)+1; i++){
			PULSO_WR(0);
		}
}




void LCD_Write_COM(unsigned char VL){   
	RS(0);
	LCD_Writ_Bus(0x00,VL,display_transfer_mode);
}



void LCD_Write_DATA(unsigned char VH,unsigned char VL){
	RS(1);
	LCD_Writ_Bus(VH,VL,display_transfer_mode);
}

// borré otra función con mismo nombre y solo VL como argumento

void LCD_Write_COM_DATA(unsigned char com1,int dat1){
	unsigned char VL = (unsigned char) 0x00FF & dat1;
	unsigned char VH = (unsigned char) 0x00FF & (dat1 >> 8);
   LCD_Write_COM(com1);
	demoraPulso();
   LCD_Write_DATA(VH,VL);
	demoraPulso();
}


int LCD_Write_COM_Prueba(unsigned char VL){   
	RS(0);
	LCD_Writ_Bus(0x00,VL,display_transfer_mode);
			sprintf_mio(aux,"Envio dato de prueba: %d \n\r", VL);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
	return 1;
}


void InitLCD(unsigned char orientation){
	orient=orientation;

	DIR_RS(1); // RS como salida
	DIR_WR(1);
	DIR_CS(1);
	DIR_RESET(1);
	_set_direction_registers(display_transfer_mode);

	RESET(1);
	delay_rit_ms(5); 
	RESET(0);
	delay_rit_ms(15);
	RESET(1);
	delay_rit_ms(15);
	CS(0);

	LCD_Write_COM_DATA(0xE5, 0x78F0); // set SRAM internal timing
	LCD_Write_COM_DATA(0x01, 0x0100); // set Driver Output Control  
	LCD_Write_COM_DATA(0x02, 0x0200); // set 1 line inversion  
	LCD_Write_COM_DATA(0x03, 0x1030); // set GRAM write direction and BGR=1.  
	LCD_Write_COM_DATA(0x04, 0x0000); // Resize register  
	LCD_Write_COM_DATA(0x08, 0x0207); // set the back porch and front porch  
	LCD_Write_COM_DATA(0x09, 0x0000); // set non-display area refresh cycle ISC[3:0]  
	LCD_Write_COM_DATA(0x0A, 0x0000); // FMARK function  
	LCD_Write_COM_DATA(0x0C, 0x0000); // RGB interface setting  
	LCD_Write_COM_DATA(0x0D, 0x0000); // Frame marker Position  
	LCD_Write_COM_DATA(0x0F, 0x0000); // RGB interface polarity  
	//*************Power On sequence ****************//  
	LCD_Write_COM_DATA(0x10, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB  
	LCD_Write_COM_DATA(0x11, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]  
	LCD_Write_COM_DATA(0x12, 0x0000); // VREG1OUT voltage  
	LCD_Write_COM_DATA(0x13, 0x0000); // VDV[4:0] for VCOM amplitude  
	LCD_Write_COM_DATA(0x07, 0x0001);  
	delay_rit_ms(200); // Dis-charge capacitor power voltage  
	LCD_Write_COM_DATA(0x10, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB  
	LCD_Write_COM_DATA(0x11, 0x0227); // Set DC1[2:0], DC0[2:0], VC[2:0]  
	delay_rit_ms(50); // delay_rit_ms 50ms  
	LCD_Write_COM_DATA(0x12, 0x000D); // 0012  
	delay_rit_ms(50); // delay_rit_ms 50ms  
	LCD_Write_COM_DATA(0x13, 0x1200); // VDV[4:0] for VCOM amplitude  
	LCD_Write_COM_DATA(0x29, 0x000A); // 04  VCM[5:0] for VCOMH  
	LCD_Write_COM_DATA(0x2B, 0x000D); // Set Frame Rate  
	delay_rit_ms(50); // delay_rit_ms 50ms  
	LCD_Write_COM_DATA(0x20, 0x0000); // GRAM horizontal Address  
	LCD_Write_COM_DATA(0x21, 0x0000); // GRAM Vertical Address  
	// ----------- Adjust the Gamma Curve ----------//  
	LCD_Write_COM_DATA(0x30, 0x0000);  
	LCD_Write_COM_DATA(0x31, 0x0404);  
	LCD_Write_COM_DATA(0x32, 0x0003);  
	LCD_Write_COM_DATA(0x35, 0x0405);  
	LCD_Write_COM_DATA(0x36, 0x0808);  
	LCD_Write_COM_DATA(0x37, 0x0407);  
	LCD_Write_COM_DATA(0x38, 0x0303);  
	LCD_Write_COM_DATA(0x39, 0x0707);  
	LCD_Write_COM_DATA(0x3C, 0x0504);  
	LCD_Write_COM_DATA(0x3D, 0x0808);  
	//------------------ Set GRAM area ---------------//  
	LCD_Write_COM_DATA(0x50, 0x0000); // Horizontal GRAM Start Address  
	LCD_Write_COM_DATA(0x51, 0x00EF); // Horizontal GRAM End Address  
	LCD_Write_COM_DATA(0x52, 0x0000); // Vertical GRAM Start Address  
	LCD_Write_COM_DATA(0x53, 0x013F); // Vertical GRAM Start Address  
	LCD_Write_COM_DATA(0x60, 0xA700); // Gate Scan Line  
	LCD_Write_COM_DATA(0x61, 0x0001); // NDL,VLE, REV   
	LCD_Write_COM_DATA(0x6A, 0x0000); // set scrolling line  
	//-------------- Partial Display Control ---------//  
	LCD_Write_COM_DATA(0x80, 0x0000);  
	LCD_Write_COM_DATA(0x81, 0x0000);  
	LCD_Write_COM_DATA(0x82, 0x0000);  
	LCD_Write_COM_DATA(0x83, 0x0000);  
	LCD_Write_COM_DATA(0x84, 0x0000);  
	LCD_Write_COM_DATA(0x85, 0x0000);  
	//-------------- Panel Control -------------------//  
	LCD_Write_COM_DATA(0x90, 0x0010);  
	LCD_Write_COM_DATA(0x92, 0x0000);  
	LCD_Write_COM_DATA(0x07, 0x0133); // 262K color and display ON    

	CS(1); 

	setColor(255, 255, 255);
	setBackColor(0, 0, 0);
	cfont.font=0;
	_transparent = false;
}


void setXY(int x1, int y1, int x2, int y2){
	if (orient==LANDSCAPE){
		swap(int, x1, y1);
		swap(int, x2, y2)
		y1=disp_y_size-y1;
		y2=disp_y_size-y2;
		swap(int, y1, y2)
	}

	LCD_Write_COM_DATA(0x20,x1);
	LCD_Write_COM_DATA(0x21,y1);
	LCD_Write_COM_DATA(0x50,x1);
	LCD_Write_COM_DATA(0x52,y1);
	LCD_Write_COM_DATA(0x51,x2);
	LCD_Write_COM_DATA(0x53,y2);
	LCD_Write_COM(0x22); 
}


void clrXY(){
	if (orient==PORTRAIT)
		setXY(0,0,disp_x_size,disp_y_size);
	else
		setXY(0,0,disp_y_size,disp_x_size);
}


void drawRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}

	drawHLine(x1, y1, x2-x1);
	drawHLine(x1, y2, x2-x1);
	drawVLine(x1, y1, y2-y1);
	drawVLine(x2, y1, y2-y1);
}


void drawRoundRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4){
		drawPixel(x1+1,y1+1);
		drawPixel(x2-1,y1+1);
		drawPixel(x1+1,y2-1);
		drawPixel(x2-1,y2-1);
		drawHLine(x1+2, y1, x2-x1-4);
		drawHLine(x1+2, y2, x2-x1-4);
		drawVLine(x1, y1+2, y2-y1-4);
		drawVLine(x2, y1+2, y2-y1-4);
	}
}


void fillRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}
	if (display_transfer_mode==16){
		CS(0);
		setXY(x1, y1, x2, y2);
		RS(1);
		_fast_fill_16(fch,fcl,(((long)(x2-x1)+1)*((long)(y2-y1)+1)));
		CS(1);
	}
}


void fillRoundRect(int x1, int y1, int x2, int y2){
	if (x1>x2){
		swap(int, x1, x2);
	}
	if (y1>y2){
		swap(int, y1, y2);
	}

	if ((x2-x1)>4 && (y2-y1)>4){
		for (int i=0; i<((y2-y1)/2)+1; i++){
			switch(i){
			case 0:
				drawHLine(x1+2, y1+i, x2-x1-4);
				drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				drawHLine(x1+1, y1+i, x2-x1-2);
				drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				drawHLine(x1, y1+i, x2-x1);
				drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}



void drawCircle(int x, int y, int radius){
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
 
	CS(0);
	setXY(x, y + radius, x, y + radius);
	LCD_Write_DATA(fch,fcl);
	setXY(x, y - radius, x, y - radius);
	LCD_Write_DATA(fch,fcl);
	setXY(x + radius, y, x + radius, y);
	LCD_Write_DATA(fch,fcl);
	setXY(x - radius, y, x - radius, y);
	LCD_Write_DATA(fch,fcl);
 
	while(x1 < y1){
		if(f >= 0){
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		setXY(x + x1, y + y1, x + x1, y + y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - x1, y + y1, x - x1, y + y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + x1, y - y1, x + x1, y - y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - x1, y - y1, x - x1, y - y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + y1, y + x1, x + y1, y + x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - y1, y + x1, x - y1, y + x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + y1, y - x1, x + y1, y - x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - y1, y - x1, x - y1, y - x1);
		LCD_Write_DATA(fch,fcl);
	}
	CS(1);
	clrXY();
}



void fillCircle(int x, int y, int radius){
	for(int y1=-radius; y1<=0; y1++){
		for(int x1=-radius; x1<=0; x1++){
			if(x1*x1+y1*y1 <= radius*radius){
				drawHLine(x+x1, y+y1, 2*(-x1));
				drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
		}
	}
}



void clrScr(){
	long i;
	
	CS(0);
	clrXY();
	RS(1);
	_fast_fill_16(0,0,((disp_x_size+1)*(disp_y_size+1)));
	CS(1);
}



void fillScr(unsigned char r, unsigned char g, unsigned char b){
	int color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
	fillScr2(color);
}



void fillScr2(int color){
	long i;
	unsigned char ch, cl;
	
	ch=byte(color>>8);
	cl=byte(color & 0xFF);

	CS(0);
	clrXY();
	RS(1);
	_fast_fill_16(ch,cl,((disp_x_size+1)*(disp_y_size+1)));
	CS(1);
}



void setColor(unsigned char r, unsigned char g, unsigned char b){
	fch=((r&248)| g >> 5);
	fcl=((g&28) << 3| b >> 3);
}


/*
void setColor(int color){
	fch=byte(color >> 8);
	fcl=byte(color & 0xFF);
}
*/


int getColor(){
	return ((fch<<8) | fcl);
}



void setBackColor(unsigned char r, unsigned char g, unsigned char b){
	bch=((r&248)|g>>5);
	bcl=((g&28)<<3|b>>3);
	_transparent=false;
}


/*
void setBackColor(uint32_t color){
	if (color==VGA_TRANSPARENT)
		_transparent=true;
	else{
		bch=byte(color>>8);
		bcl=byte(color & 0xFF);
		_transparent=false;
	}
}

*/

int getBackColor(){
	return (bch<<8) | bcl;
}



void setPixel(int color){
	LCD_Write_DATA((color>>8),(color&0xFF));	// rrrrrggggggbbbbb
}



void drawPixel(int x, int y){
	CS(0);
	setXY(x, y, x, y);
	setPixel((fch<<8)|fcl);
	CS(1);
	clrXY();
}



void drawLine(int x1, int y1, int x2, int y2){
	if (y1==y2)
		drawHLine(x1, y1, x2-x1);
	else if (x1==x2)
		drawVLine(x1, y1, y2-y1);
	else{
		unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
		short			xstep =  x2 > x1 ? 1 : -1;
		unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
		short			ystep =  y2 > y1 ? 1 : -1;
		int				col = x1, row = y1;

		CS(0);
		if (dx < dy)
		{
			int t = - (dy >> 1);
			while (true){
				setXY (col, row, col, row);
				LCD_Write_DATA (fch, fcl);
				if (row == y2)
					return;
				row += ystep;
				t += dx;
				if (t >= 0){
					col += xstep;
					t   -= dy;
				}
			} 
		}
		else{
			int t = - (dx >> 1);
			while (true){
				setXY (col, row, col, row);
				LCD_Write_DATA (fch, fcl);
				if (col == x2)
					return;
				col += xstep;
				t += dy;
				if (t >= 0){
					row += ystep;
					t   -= dx;
				}
			} 
		}
		CS(1);
	}
	clrXY();
}



void drawHLine(int x, int y, int l){
	if (l<0){
		l = -l;
		x -= l;
	}
	CS(0);
	setXY(x, y, x+l, y);
	RS(1);
	_fast_fill_16(fch,fcl,l);
	CS(1);
	clrXY();
}



void drawVLine(int x, int y, int l){
	if (l<0){
		l = -l;
		y -= l;
	}
	CS(0);
	setXY(x, y, x, y+l);
	RS(1);
	_fast_fill_16(fch,fcl,l);
	CS(1);
	clrXY();
}



void printChar(unsigned char c, int x, int y){
	unsigned char i,ch;
	int j;
	int temp; 

	CS(0);
  
	if (!_transparent){
		if (orient==PORTRAIT){
			setXY(x,y,x+cfont.x_size-1,y+cfont.y_size-1);
	  
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j++){
				ch=pgm_read_byte(&cfont.font[temp]);
				for(i=0;i<8;i++){   
					if((ch&(1<<(7-i)))!=0){
						setPixel((fch<<8)|fcl);
					} 
					else{
						setPixel((bch<<8)|bcl);
					}   
				}
				temp++;
			}
		}
		else{
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;

			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j+=(cfont.x_size/8)){
				setXY(x,y+(j/(cfont.x_size/8)),x+cfont.x_size-1,y+(j/(cfont.x_size/8)));
				for (int zz=(cfont.x_size/8)-1; zz>=0; zz--){
					ch=pgm_read_byte(&cfont.font[temp+zz]);
					for(i=0;i<8;i++){   
						if((ch&(1<<i))!=0){
							setPixel((fch<<8)|fcl);
						} 
						else{
							setPixel((bch<<8)|bcl);
						}   
					}
				}
				temp+=(cfont.x_size/8);
			}
		}
	}
	else{
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
		for(j=0;j<cfont.y_size;j++){
			for (int zz=0; zz<(cfont.x_size/8); zz++){
				ch=pgm_read_byte(&cfont.font[temp+zz]); 
				for(i=0;i<8;i++){   
					if((ch&(1<<(7-i)))!=0){
						setXY(x+i+(zz*8),y+j,x+i+(zz*8)+1,y+j+1);
						setPixel((fch<<8)|fcl);
					} 
				}
			}
			temp+=(cfont.x_size/8);
		}
	}

	CS(1);
	clrXY();
}



void rotateChar(unsigned char c, int x, int y, int pos, int deg){
	unsigned char i,j,ch;
	int temp; 
	int newx,newy;
	double radian;
	radian=deg*0.0175;  

	CS(0);

	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++){
		for (int zz=0; zz<(cfont.x_size/8); zz++){
			ch=pgm_read_byte(&cfont.font[temp+zz]); 
			for(i=0;i<8;i++){   
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				setXY(newx,newy,newx+1,newy+1);
				
				if((ch&(1<<(7-i)))!=0){
					setPixel((fch<<8)|fcl);
				} 
				else{
					if (!_transparent)
						setPixel((bch<<8)|bcl);
				}   
			}
		}
		temp+=(cfont.x_size/8);
	}
	CS(1);
	clrXY();
}



void printLCD(char *st, int x, int y, int deg){
	int stl, i;
	stl = strlen(st);

	if (orient==PORTRAIT){
		if (x==RIGHT)
			x=(disp_x_size+1)-(stl*cfont.x_size);
		if (x==CENTER)
			x=((disp_x_size+1)-(stl*cfont.x_size))/2;
	}
	else{
		if (x==RIGHT)
			x=(disp_y_size+1)-(stl*cfont.x_size);
		if (x==CENTER)
			x=((disp_y_size+1)-(stl*cfont.x_size))/2;
	}

	for (i=0; i<stl; i++)
		if (deg==0)
			printChar(*st++, x + (i*(cfont.x_size)), y);
		else
			rotateChar(*st++, x, y, i, deg);
}

/*

void printLCD(String st, int x, int y, int deg){
	char buf[st.length()+1];

	st.toCharArray(buf, st.length()+1);
	printLCD(buf, x, y, deg);
}

*/

void printNumI(long num, int x, int y, int length, unsigned char filler){
	char buf[25];
	char st[27];
	bool neg=false;
	int c=0, f=0;
  
	if (num==0){
		if (length!=0){
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else
		{
			st[0]=48;
			st[1]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=true;
			num=-num;
		}
	  
		while (num>0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg))
		{
			for (int i=0; i<(length-c-neg); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		for (int i=0; i<c; i++)
		{
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;

	}

	printLCD(st,x,y,0);
}



void printNumF(double num, unsigned char dec, int x, int y, char divider, int length, char filler){
	char st[27];
	bool neg=false;

	if (dec<1)
		dec=1;
	else if (dec>5)
		dec=5;

	if (num<0)
		neg = true;

	convert_float(st, num, length, dec);

	if (divider != '.'){
		for (int i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' '){
		if (neg){
			st[0]='-';
			for (int i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		}
		else{
			for (int i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}

	printLCD(st,x,y,0);
}



void setFont(uint8_t* font){
	cfont.font=font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);
}



unsigned char * getFont(){
	return cfont.font;
}



unsigned char getFontXsize(){
	return ((unsigned char)cfont.x_size);
}



unsigned char getFontYsize(){
	return ((unsigned char)cfont.y_size);
}



void drawBitmap2(int x, int y, int sx, int sy, bitmapdatatype data, int scale){
	unsigned int col;
	int tx, ty, tc, tsx, tsy;

	if (scale==1){
		if (orient==PORTRAIT){
			CS(0);
			setXY(x, y, x+sx-1, y+sy-1);
			for (tc=0; tc<(sx*sy); tc++){
				col=pgm_read_word(&data[tc]);
				LCD_Write_DATA(col>>8,col & 0xff);
			}
			CS(1);
		}
		else{
			CS(0);
			for (ty=0; ty<sy; ty++){
				setXY(x, y+ty, x+sx-1, y+ty);
				for (tx=sx-1; tx>=0; tx--){
					col=pgm_read_word(&data[(ty*sx)+tx]);
					LCD_Write_DATA(col>>8,col & 0xff);
				}
			}
			CS(1);
		}
	}
	else{
		if (orient==PORTRAIT){
			CS(0);
			for (ty=0; ty<sy; ty++){
				setXY(x, y+(ty*scale), x+((sx*scale)-1), y+(ty*scale)+scale);
				for (tsy=0; tsy<scale; tsy++)
					for (tx=0; tx<sx; tx++){
						col=pgm_read_word(&data[(ty*sx)+tx]);
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA(col>>8,col & 0xff);
					}
			}
			CS(1);
		}
		else{
			CS(0);
			for (ty=0; ty<sy; ty++){
				for (tsy=0; tsy<scale; tsy++){
					setXY(x, y+(ty*scale)+tsy, x+((sx*scale)-1), y+(ty*scale)+tsy);
					for (tx=sx-1; tx>=0; tx--){
						col=pgm_read_word(&data[(ty*sx)+tx]);
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA(col>>8,col & 0xff);
					}
				}
			}
			CS(1);
		}
	}
	clrXY();
}



void drawBitmap(int x, int y, int sx, int sy, bitmapdatatype data, int deg, int rox, int roy){
	unsigned int col;
	int tx, ty, newx, newy;
	double radian;
	radian=deg*0.0175;  

	if (deg==0)
		drawBitmap2(x, y, sx, sy, data,0);
	else{
		CS(0);
		for (ty=0; ty<sy; ty++)
			for (tx=0; tx<sx; tx++){
				col=pgm_read_word(&data[(ty*sx)+tx]);

				newx=x+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=y+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));

				setXY(newx, newy, newx, newy);
				LCD_Write_DATA(col>>8,col & 0xff);
			}
		CS(1);
	}
	clrXY();
}



void lcdOff(){
	CS(0);
	switch (display_model){
	case PCF8833:
		LCD_Write_COM(0x28);
		break;
	case CPLD:
		LCD_Write_COM_DATA(0x01,0x0000);
		LCD_Write_COM(0x0F);   
		break;
	}
	CS(1);
}



void lcdOn(){
	CS(0);
	switch (display_model){
	case PCF8833:
		LCD_Write_COM(0x29);
		break;
	case CPLD:
		LCD_Write_COM_DATA(0x01,0x0010);
		LCD_Write_COM(0x0F);   
		break;
	}
	CS(1);
}



void setContrast(unsigned char c){
	CS(0);
	switch (display_model){
	case PCF8833:
		if (c>64) c=64;
		LCD_Write_COM(0x25);
		LCD_Write_DATA(0x00,c);
		break;
	}
	CS(1);
}



int getDisplayXSize(){
	if (orient==PORTRAIT)
		return disp_x_size+1;
	else
		return disp_y_size+1;
}



int getDisplayYSize(){
	if (orient==PORTRAIT)
		return disp_y_size+1;
	else
		return disp_x_size+1;
}



void setBrightness(unsigned char br){
	CS(0);
	switch (display_model){
	case CPLD:
		if (br>16) br=16;
		LCD_Write_COM_DATA(0x01,br);
		LCD_Write_COM(0x0F);   
		break;
	}
	CS(1);
}



void setDisplayPage(unsigned char page){
	CS(0);
	switch (display_model){
	case CPLD:
		if (page>7) page=7;
		LCD_Write_COM_DATA(0x04,page);
		LCD_Write_COM(0x0F);   
		break;
	}
	CS(1);
}



void setWritePage(unsigned char page){
	CS(0);
	switch (display_model){
	case CPLD:
		if (page>7) page=7;
		LCD_Write_COM_DATA(0x05,page);
		LCD_Write_COM(0x0F);   
		break;
	}
	CS(1);
}
