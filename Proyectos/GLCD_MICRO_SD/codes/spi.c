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

/*
#define X_MEASURE      (TSC_START | TSC_CHANNEL_X  | TSC_CONVERSION_MODE | TSC_DFR_MODE | TSC_ADC_ON)
#define Y_MEASURE      (TSC_START | TSC_CHANNEL_Y  | TSC_CONVERSION_MODE | TSC_DFR_MODE | TSC_ADC_ON)
#define Z1_MEASURE     (TSC_START | TSC_CHANNEL_Z1 | TSC_CONVERSION_MODE | TSC_DFR_MODE | TSC_ADC_ON)
#define Z2_MEASURE     (TSC_START | TSC_CHANNEL_Z2 | TSC_CONVERSION_MODE | TSC_DFR_MODE | TSC_ADC_ON)
*/
#define X_MEASURE      (TSC_START | TSC_CHANNEL_X  | 0x00)
#define Y_MEASURE      (TSC_START | TSC_CHANNEL_Y  | 0x00)
#define Z1_MEASURE     (TSC_START | TSC_CHANNEL_Z1 | 0x00)
#define Z2_MEASURE     (TSC_START | TSC_CHANNEL_Z2 | 0x00)

#define PWRDOWN          (TSC_START | TSC_CHANNEL_Y  | TSC_CONVERSION_MODE | TSC_DFR_MODE | TSC_PD)

typedef struct {
	int16_t ad_left;						/* left margin */
	int16_t ad_right;						/* right margin */
	int16_t ad_top;							/* top margin */
	int16_t ad_bottom;						/* bottom margin */
	int16_t lcd_width;						/* lcd horizontal size */
	int16_t lcd_height;						/* lcd vertical size */
	uint8_t swap_xy;						/* 1: swap x-y coords */
}TSC2046_Init_T;

#if (TSC2046_CONVERSION_BITS == 12)
#define TSC2046_COORD_MAX           (0xFFF)
#define TSC2046_DELTA_VARIANCE      (0x50)
#else
#define TSC2046_COORD_MAX           (0xFF)
#define TSC2046_DELTA_VARIANCE      (0x03)
#endif
#define COORD_GET_NUM                 (10)

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




//Bloque 0:
#define MIN_COORDX_B0	LIM_INF_X
#define MAX_COORDX_B0	LIM_SUP_X
#define MIN_COORDY_B0	LIM_INF_Y
#define MAX_COORDY_B0	LIM_INF_Y + 159

//Bloque 1:
#define MIN_COORDX_B1	LIM_INF_X
#define MAX_COORDX_B1	LIM_SUP_X
#define MIN_COORDY_B1	LIM_INF_Y + 160
#define MAX_COORDY_B1	LIM_SUP_Y


bool bloque0= false;
bool bloque1= false;


void spiInit();
void leerTouch(unsigned int * pX, unsigned int * pY);



// Funciones del TouchScreen

bool bloqueTouch( unsigned int * x, unsigned int * y ){
	if(*x == 0 && *y == 0)
		return false;
	if((*x >= MIN_COORDX_B0 ) && (*x <= MAX_COORDX_B0) && (*y >= MIN_COORDY_B0 ) && (*y <= MAX_COORDY_B0))
		bloque0= true;
	if((*x >= MIN_COORDX_B1 ) && (*x <= MAX_COORDX_B1) && (*y >= MIN_COORDY_B1 ) && (*y <= MAX_COORDY_B1))
		bloque1= true;
	return false;
}




void spiInit(){
	Board_SSP_Init(LPC_SSP1);
	//Chip_SSP_Init(LPC_SSP);
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_DATA_BITS;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
   Chip_SSP_SetFormat(LPC_SSP1, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_Enable(LPC_SSP1);
	Chip_SSP_SetMaster(LPC_SSP1, 1);
	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_SetBitRate(LPC_SSP1, 100000);   //32000);

	SCU->SFSP[1][18] = (SCU_PINIO_FAST | SCU_MODE_FUNC0);// P1_18, GPIO0[13]  -> TXD0 -> (Chip Select MicroSD)
	SCU->SFSP[1][20] = (SCU_MODE_DES | SCU_MODE_FUNC0);// P1_20, GPIO0[15]  -> TXD1 -> (Chip Select TouchScreen)

	GPIO_PORT->DIR[0] |= (1 << 13);
	GPIO_PORT->DIR[0] |= (1 << 15);

xf_setup.length = BUFFER_SIZE;
xf_setup.rx_cnt = xf_setup.tx_cnt = 0;
	Buffer_Init();
}


void leerTouch(unsigned int * pX, unsigned int * pY){
	unsigned int touchX = 0;
	unsigned int touchY = 0;
	unsigned int touchZ1 = 0;
	unsigned int touchZ2 = 0;
	unsigned int basura = 0;
	unsigned int touchX_low= 0;
	unsigned int touchY_low= 0;
	unsigned int touchZ1_low= 0;
	unsigned int touchZ2_low= 0;
	unsigned int tmpZ=0; 
	unsigned int tmpZ1=0;
	unsigned int tmpZ2=0;

	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_NOP);
	
	basura= Chip_SSP_ReceiveFrame(LPC_SSP1);

	Chip_SSP_SendFrame(LPC_SSP1, Y_MEASURE);
	touchY_low= Chip_SSP_ReceiveFrame(LPC_SSP1);
	touchY= Chip_SSP_ReceiveFrame(LPC_SSP1); 

	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_NOP);

	Chip_SSP_SendFrame(LPC_SSP1, X_MEASURE);
	touchX_low= Chip_SSP_ReceiveFrame(LPC_SSP1);
	touchX= Chip_SSP_ReceiveFrame(LPC_SSP1);

	Chip_SSP_SendFrame(LPC_SSP1, Z1_MEASURE);
	touchZ1_low= Chip_SSP_ReceiveFrame(LPC_SSP1);
	touchZ1= Chip_SSP_ReceiveFrame(LPC_SSP1); 

	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_NOP);

	Chip_SSP_SendFrame(LPC_SSP1, Z2_MEASURE);
	touchZ2_low= Chip_SSP_ReceiveFrame(LPC_SSP1);
	touchZ2= Chip_SSP_ReceiveFrame(LPC_SSP1); 

	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_PRESS);
	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_NOP);

	tmpX = (((touchX << 8) | touchX_low) >> 6) & 0xFFF;
	tmpY = (((touchY << 8) | touchY_low) >> 4) & 0xFFFF;
	tmpZ1 = (((touchZ1 << 8) | touchZ1_low) >> 4) & 0xFFFF;
	tmpZ2 = (((touchZ2 << 8) | touchZ2_low) >> 4) & 0xFFFF;

	tmpZ = (tmpX) * ((tmpZ2 / tmpZ1));
	if ((tmpZ <= 0) || (tmpZ > 35000)){
		*pY = 0;
		*pX = 0;
	}
	else{
		*pX = (unsigned int) (tmpX * 0.564705882);
		*pY = (unsigned int) (tmpY * 0.188679245);
	}
}
