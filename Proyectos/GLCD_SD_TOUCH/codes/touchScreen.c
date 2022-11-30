#include "board.h"
#include "stdio.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define LPC_SSP           LPC_SSP1
#define SSP_IRQ           SSP1_IRQn
#define LPC_GPDMA_SSP_TX  GPDMA_CONN_SSP1_Tx
#define LPC_GPDMA_SSP_RX  GPDMA_CONN_SSP1_Rx
#define SSPIRQHANDLER	  SSP1_IRQHandler

#define BUFFER_SIZE                         (0x01)
#define SSP_DATA_BITS                       (SSP_BITS_8)
#define SSP_DATA_BIT_NUM(databits)          (databits+1)
#define SSP_DATA_BYTES(databits)            (((databits) > SSP_BITS_8) ? 2:1)
#define SSP_LO_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? 0xFF:(0xFF>>(8-SSP_DATA_BIT_NUM(databits))))
#define SSP_HI_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? (0xFF>>(16-SSP_DATA_BIT_NUM(databits))):0)

#define SSP_MODE_SEL                        (0x31)
#define SSP_TRANSFER_MODE_SEL               (0x32)
#define SSP_MASTER_MODE_SEL                 (0x31)
#define SSP_SLAVE_MODE_SEL                  (0x32)
#define SSP_POLLING_SEL                     (0x31)
#define SSP_INTERRUPT_SEL                   (0x32)
#define SSP_DMA_SEL                         (0x33)



// control del XPT2046

#define TSC2046_CONVERSION_BITS            12
#define TSC_START                          (0x01 << 7)
#define TSC_CHANNEL_X                      (0x05 << 4)		/* differential */
#define TSC_CHANNEL_Y                      (0x01 << 4)		/* differential */
#define TSC_CHANNEL_Z1                     (0x03 << 4)		/* differential */
#define TSC_CHANNEL_Z2                     (0x04 << 4)		/* differential */
#define TSC_8BIT                           (0x01 << 3)
#define TSC_12BIT                          (0x00 << 3)
#define TSC_PD                              0x00
#define TSC_ADC_ON                          0x01
#define TSC_REF_ON                          0x02
#if (TSC2046_CONVERSION_BITS == 12)
#define TSC_CONVERSION_MODE                 TSC_12BIT
#else
#define TSC_CONVERSION_MODE                 TSC_8BIT
#endif

#define TSC_SER_MODE                       (0x01 << 2)	/* Single-Ended Reference Mode */
#define TSC_DFR_MODE                       (0x00 << 2)	/* Differential Reference Mode */


#define X_MEASURE      (TSC_START | TSC_CHANNEL_X  | TSC_8BIT | 0x00)  
#define Y_MEASURE      (TSC_START | TSC_CHANNEL_Y  | TSC_8BIT | 0x00)  
#define Z1_MEASURE     (TSC_START | TSC_CHANNEL_Z1 | TSC_8BIT | 0x00)
#define Z2_MEASURE     (TSC_START | TSC_CHANNEL_Z2 | TSC_8BIT | 0x00)

#define PWRDOWN          (TSC_START | TSC_CHANNEL_Y  | TSC_CONVERSION_MODE | TSC_DFR_MODE | TSC_PD)




// ****************************************************************************************
// Control del XPT2046
// ****************************************************************************************


// NOTE: FOSC = 475.5 MHz  CCLK = FOSC / 4 = 118 MHz


#define T_CLK_PIN		(1<<7)		// P0.7
#define T_CS_PIN		(1<<6)		// P0.6
#define T_DIN_PIN		(1<<9)		// P0.9
#define T_DOUT_PIN	(1<<8)		// P0.8
#define T_IRQ_PIN		(1<<0)		// P0.0

#define SSP1_PIN_MASK	0x000FF000
#define SSP1_CLK_MASK	0x00300000

#define SSP1_FIFO_SIZE 	8
#define FIFO_TNF		(1 << 1)
#define FIFO_BSY		(1 << 4)

// Commands to get readings
//  | S | A2 | A1 | A0 | Mode | SFR/DFR | PD1 | PD0 |
// S = start bit = 1
// A[2:0] = 001		// sample Y
// A[2:0] = 101		// sample X
// A[2:0] = 010
// A[2:0] = 110
// Mode = 12bit(low) / 8bit(high)
// SFR/DFR = 0		// Differential Reference Mode
// PD[1:0] = 00		// Power down between conversions

#define T_CMD_X		0xD0
#define T_CMD_Y		0x90
#define T_CMD_PRESS	0xB0
#define T_CMD_NOP		0x00
#define MIN_PRESS		200
#define MAX_PRESS		1000
#define CAL_X			0
#define CAL_Y			240
#define PROMEDIO_TOUCH	1


// BLoques TOUCHs

#define	RANGO_X		240
#define	RANGO_Y		320
#define	LIM_INF_X	308
#define	LIM_SUP_X	LIM_INF_X + RANGO_X
#define	LIM_INF_Y	39
#define	LIM_SUP_Y	LIM_INF_Y + RANGO_Y




typedef struct {
	int16_t ad_left;						/* left margin */
	int16_t ad_right;						/* right margin */
	int16_t ad_top;							/* top margin */
	int16_t ad_bottom;						/* bottom margin */
	int16_t lcd_width;						/* lcd horizontal size */
	int16_t lcd_height;						/* lcd vertical size */
	uint8_t swap_xy;						/* 1: swap x-y coords */
}TSC2046_Init_T;


/** Local variables */
TSC2046_Init_T TSC_Config = { 3914, 2100,  3000, 1163, 240, 320, 1 };



/* Tx buffer */
uint8_t Tx_Buf[BUFFER_SIZE];

/* Rx buffer */
uint8_t Rx_Buf[BUFFER_SIZE];

SSP_ConfigFormat ssp_format;
Chip_SSP_DATA_SETUP_T xf_setup;
uint8_t  isXferCompleted = 0;
uint8_t dmaChSSPTx, dmaChSSPRx;
uint8_t isDmaTxfCompleted = 0;
uint8_t isDmaRxfCompleted = 0;

unsigned int contadorTouch=0;
unsigned int tmpX=0;
unsigned int tmpY=0;

// *************************************************************************************





/* Initialize buffer */
void Buffer_Init(void){
	uint16_t i;
	uint8_t ch = 0;
	for (i = 0; i < BUFFER_SIZE; i++) {
	  	Tx_Buf[i] = ch++;
	  	Rx_Buf[i] = 0xAA;
  	}
}


	/* Verify buffer after transfer */
uint8_t Buffer_Verify(void){
  uint16_t i;
  uint8_t *src_addr = (uint8_t *) &Tx_Buf[0];
  uint8_t *dest_addr = (uint8_t *) &Rx_Buf[0];
  for ( i = 0; i < BUFFER_SIZE; i++ ) {
  if (((*src_addr) & SSP_LO_BYTE_MSK(ssp_format.bits)) != ((*dest_addr) & SSP_LO_BYTE_MSK(ssp_format.bits))) {
  return 1;
  }
  src_addr++;
  dest_addr++;
  
if (SSP_DATA_BYTES(ssp_format.bits) == 2) {
  if (((*src_addr) & SSP_HI_BYTE_MSK(ssp_format.bits)) != ((*dest_addr) & SSP_HI_BYTE_MSK(ssp_format.bits))) {
  return 1;
  }
  src_addr++;
  dest_addr++;
  i++;
  }
}
  return 0;
}







// ***************** Definición de Sectores Touch ************************

#define  ADDR_PORTADA			1075200		// Dirección de la imágen de Portada
#define  ADDR_SLIDER_0			1228800		// Dirección del primer Slider
#define	ADDR_SLIDER_FIN		2150400		// Dirección del ultimo Slider
#define  ADDR_CONFIRMA_CUENTA	2304000		// Dirección de aviso para confirmar "Pedir la cuenta"

#define CANT_SECTORES_TOUCH    20
//{X0,Y0,X1,Y1}

unsigned int sectoresTouch[3][4]={ { 60,150,180, 210}, { 1,280,59, 319}, {180,280,239,319} };

unsigned char sectorPresionado[CANT_SECTORES_TOUCH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char sectorHabilitado[CANT_SECTORES_TOUCH] = {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0};  // se utiliza para saber si tiene que ser procesado determinado sector touch


void chipSelectTouchHigh(void){
	GPIO_PORT->SET[0] |= (1 << 15);  // TXD1
}


void chipSelectTouchLow(void){
	GPIO_PORT->CLR[0] |= (1 << 15);  // TXD1
}



// Funciones del TouchScreen

bool analizarSectoresTouch( unsigned int * x, unsigned int * y ){
	int i=0;
	for(i=0; i < CANT_SECTORES_TOUCH; i++){
		if( (*x >= sectoresTouch[i][0]) && (*x <= sectoresTouch[i][2] ) && (*y >= sectoresTouch[i][1] ) && (*y <= sectoresTouch[i][3] ) )
			sectorPresionado[i]= 1;
		else
			sectorPresionado[i]= 0;
	}
// DEBUG
/*			sprintf_mio(aux,"\n Touch X: %d   -   Touch Y: %d  \n\n", *x, *y );
			DEBUGSTR(aux);	
			limpiaBufferAUX();
*/
	return true;
}




void spiInit(unsigned char frecSPI){
	unsigned long int frec=0;
	chipSelectHigh();
	chipSelectTouchHigh();
	if(frecSPI == 2)
		frec= 400000;
	if(frecSPI == 1)
		frec= 100000;
	if(frecSPI == 0)
		frec= 32000;



	Board_SSP_Init(LPC_SSP1);
	//Chip_SSP_Init(LPC_SSP);
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_DATA_BITS;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
   Chip_SSP_SetFormat(LPC_SSP1, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_Enable(LPC_SSP1);
	Chip_SSP_SetMaster(LPC_SSP1, 1);
	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_SetBitRate(LPC_SSP1, frec);  // 100000);   //32000);

	SCU->SFSP[1][18] = (SCU_PINIO_FAST | SCU_MODE_FUNC0);// P1_18, GPIO0[13]  -> TXD0 -> (Chip Select MicroSD)
	SCU->SFSP[1][20] = (SCU_PINIO_FAST | SCU_MODE_FUNC0);// P1_20, GPIO0[15]  -> TXD1 -> (Chip Select TouchScreen)

	GPIO_PORT->DIR[0] |= (1 << 13);
	GPIO_PORT->DIR[0] |= (1 << 15);

xf_setup.length = BUFFER_SIZE;
xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Buffer_Init();
}


unsigned char ReadSPI(void){
	uint8_t data_in=0;
	Chip_SSP_ReadFrames_Blocking(LPC_SSP1, &data_in, sizeof(data_in));
	xf_setup.rx_data= data_in;
	//data= Chip_SSP_ReceiveFrame(LPC_SSP1); 
   return data_in;
}

unsigned char WriteSPI( unsigned char data_out ){

	xf_setup.tx_data = data_out;
 	Chip_SSP_WriteFrames_Blocking(LPC_SSP1, &data_out, sizeof(data_out));
	//Chip_SSP_SendFrame(LPC_SSP1, data_out);
   return(0);
}



unsigned char leerTouch(unsigned int * pX, unsigned int * pY){
	unsigned int touchX = 0;
	unsigned int touchY = 0;
	unsigned int touchZ1 = 0;
	unsigned int touchZ2 = 0;
	unsigned int basura = 0;
	unsigned char touchX_low= 0;
	unsigned char touchY_low= 0;
	unsigned char touchZ1_low= 0;
	unsigned char touchZ2_low= 0;
	unsigned int tmpZ=0; 
	unsigned int tmpZ1=0;
	unsigned int tmpZ2=0;
	int valor = -1;
	unsigned char i=0;
	unsigned char columna=0;
	unsigned char fila=0;

	


   unsigned char temp;
	chipSelectHigh();
	chipSelectTouchLow();


   WriteSPI(T_CMD_NOP);
   WriteSPI(T_CMD_NOP);
	basura= ReadSPI();

   WriteSPI(Y_MEASURE);
	touchY= ReadSPI();
	touchY_low= ReadSPI();

	WriteSPI(X_MEASURE);
	touchX= ReadSPI();
	touchX_low= ReadSPI();

	WriteSPI(Z1_MEASURE);
	touchZ1= ReadSPI();
	touchZ1_low= ReadSPI();


	WriteSPI(Z2_MEASURE);
	touchZ2= ReadSPI();
	touchZ2_low= ReadSPI();


	tmpX = ((((0xFFFF & touchX) << 8) | touchX_low) >> 4);
	tmpY = ((((0xFFFF & touchY) << 8) | touchY_low) >> 4);
	tmpZ1 = ((((0xFFFF & touchZ1) << 8) | touchZ1_low) >> 4);
	tmpZ2 = ((((0xFFFF & touchZ2) << 8) | touchZ2_low) >> 4);

	//tmpZ = (tmpX) * ((tmpZ2 / tmpZ1));
	if ((tmpZ1 <= 2058) || (tmpZ2 > 3978)){
		*pY = 0;
		*pX = 0;
	}
	else{
//	En los extremos de X [0-240] se observó: Xmin= 2314 , Xmax= 3722  ==>> Delta_X= 1408  ==>> Factor de proporción= 5,87
// En los extremos de Y [0-320] se observó: Ymin= 2314 , Ymax= 3850  ==>> Delta_Y= 1536  ==>> Factor de proporción= 4,8

		*pX = 240 - ((unsigned int) ((tmpX - 2222) / 6.25)); //* 0.564705882);
		if(*pX < 0)
			*pX=0;
		if(*pX > 240)
			*pX=240;
		*pY = 300 - ((unsigned int) ((tmpY - 2250) / 4.7)); // * 0.188679245);
		if(*pY < 0)
			*pY=0;
		if(*pY > 300)
			*pY=300;

		if((*pX > 2) && (*pX < 58) && (*pY > 2) && (*pY < 58))
			valor= 1;
		if((*pX > 62) && (*pX < 118) && (*pY > 2) && (*pY < 58))
			valor= 2;
		if((*pX > 122) && (*pX < 178) && (*pY > 2) && (*pY < 58))
			valor= 3;
		if((*pX > 182) && (*pX < 238) && (*pY > 2) && (*pY < 58))
			valor= 4;

		if((*pX > 2) && (*pX < 58) && (*pY > 62) && (*pY < 118))
			valor= 5;
		if((*pX > 62) && (*pX < 118) && (*pY > 62) && (*pY < 118))
			valor= 6;
		if((*pX > 122) && (*pX < 178) && (*pY > 62) && (*pY < 118))
			valor= 7;
		if((*pX > 182) && (*pX < 238) && (*pY > 62) && (*pY < 118))
			valor= 8;

		if((*pX > 2) && (*pX < 58) && (*pY > 122) && (*pY < 178))
			valor= 9;
		if((*pX > 62) && (*pX < 118) && (*pY > 122) && (*pY < 178))
			valor= 10;
		if((*pX > 122) && (*pX < 178) && (*pY > 122) && (*pY < 178))
			valor= 11;
		if((*pX > 182) && (*pX < 238) && (*pY > 122) && (*pY < 178))
			valor= 12;


		if((*pX > 2) && (*pX < 58) && (*pY > 182) && (*pY < 238))
			valor= 13;
		if((*pX > 62) && (*pX < 118) && (*pY > 182) && (*pY < 238))
			valor= 14;
		if((*pX > 122) && (*pX < 178) && (*pY > 182) && (*pY < 238))
			valor= 15;
		if((*pX > 182) && (*pX < 238) && (*pY > 182) && (*pY < 238))
			valor= 16;


		if((*pX > 2) && (*pX < 58) && (*pY > 242) && (*pY < 315))
			valor= 17;
		if((*pX > 62) && (*pX < 118) && (*pY > 242) && (*pY < 315))
			valor= 18;
		if((*pX > 122) && (*pX < 178) && (*pY > 242) && (*pY < 315))
			valor= 19;
		if((*pX > 182) && (*pX < 238) && (*pY > 242) && (*pY < 315))
			valor= 20;

/*
		for(columna = 0; columna < 4; columna++){
			for(fila = 0; fila < 5; fila++){
				if( (*pX < columna * ) && () )
			}
		}
		

		columna= (int)(*pX / 60) + 1;
		fila= (int)(*pY / 60);
		valor= (int)( columna + (fila * 5));
		if (valor > 20)
			valor= -1;

			sprintf_mio(aux,"\n ( X: %d Y: %d ) ==>>> VALOR TOUCH: %d ", *pX, *pY, valor) ;   //Columna: %d ( pY: %d  Fila:  %d ) ---- VALOR TOUCH: %d  \n\n", columna, *pY, fila, valor );
			DEBUGSTR(aux);	
			limpiaBufferAUX();
*/

	}

	for(i=1; i <= CANT_SECTORES_TOUCH; i++){
		if( i == valor )
			sectorPresionado[i - 1] = 1;
		else
			sectorPresionado[i - 1] = 0;
	}

	chipSelectTouchHigh();

	return 1;

}




void validarSectorTouch(){

	if(addressSD == ADDR_PORTADA){
		sectorHabilitado[9]=1;  // Habilito sector de boton "Comenzar"
		sectorHabilitado[10]=1;  // Habilito sector de boton "Comenzar"
		sectorHabilitado[16]=0;  // Deshabilito sector de botón "Anterior"
		sectorHabilitado[17]=0;  // Deshabilito sector de botón "Mozo!!!"
		sectorHabilitado[18]=0;  // Deshabilito sector de botón "La cuenta por favor!!!"
		sectorHabilitado[19]=0;  // Deshabilito sector de botón "Siguiente"
	}
	
	if(addressSD == ADDR_SLIDER_0){
		sectorHabilitado[9]=0;  // Deshabilito sector de boton "Comenzar"
		sectorHabilitado[10]=0;  // Deshabilito sector de boton "Comenzar"
		sectorHabilitado[16]=0;  // Deshabilito sector de boton "Anterior"
		sectorHabilitado[17]=1;  // Habilito sector de botón "Mozo!!!"
		sectorHabilitado[18]=1;  // Habilito sector de botón "La cuenta por favor!!!"
		sectorHabilitado[19]=1;  // Habilito sector de botón "Siguiente"
	}

	if( (addressSD > ADDR_SLIDER_0) && (addressSD < ADDR_SLIDER_FIN) ){
		sectorHabilitado[9]=0;  // Deshabilito sector de boton "Comenzar"
		sectorHabilitado[10]=0;  // Deshabilito sector de boton "Comenzar"
		sectorHabilitado[16]=1;  // Habilito sector de boton "Anterior"
		sectorHabilitado[17]=1;  // Habilito sector de botón "Mozo!!!"
		sectorHabilitado[18]=1;  // Habilito sector de botón "La cuenta por favor!!!"
		sectorHabilitado[19]=1;  // Habilito sector de botón "Siguiente"
	}

	if(addressSD == ADDR_SLIDER_FIN){
		sectorHabilitado[9]=0;  // Deshabilito sector de boton "Comenzar"
		sectorHabilitado[10]=0;  // Deshabilito sector de boton "Comenzar"
		sectorHabilitado[16]=1;  // Habilito sector de boton "Anterior"
		sectorHabilitado[17]=1;  // Habilito sector de botón "Mozo!!!"
		sectorHabilitado[18]=1;  // Habilito sector de botón "La cuenta por favor!!!"
		sectorHabilitado[19]=0;  // Deshabilito sector de botón "Siguiente"
	}

	if(addressSD == ADDR_CONFIRMA_CUENTA){
		sectorHabilitado[16]=1;  // Habilito sector de botón "Aceptar"
		sectorHabilitado[17]=1;  // Habilito sector de botón "Aceptar"
		sectorHabilitado[18]=1;  // Habilito sector de botón "Cancelar"
		sectorHabilitado[19]=1;  // Habilito sector de botón "Cancelar"
	}


}














