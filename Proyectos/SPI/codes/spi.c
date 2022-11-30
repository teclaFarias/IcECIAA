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

#define BUFFER_SIZE                         (0x100)
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

/* Tx buffer */
static uint8_t Tx_Buf[BUFFER_SIZE];

/* Rx buffer */
static uint8_t Rx_Buf[BUFFER_SIZE];

static SSP_ConfigFormat ssp_format;
static Chip_SSP_DATA_SETUP_T xf_setup;
static volatile uint8_t  isXferCompleted = 0;
static uint8_t dmaChSSPTx, dmaChSSPRx;
static volatile uint8_t isDmaTxfCompleted = 0;
static volatile uint8_t isDmaRxfCompleted = 0;

unsigned int contadorTouch=0;
unsigned int tmpX=0;
unsigned int tmpY=0;


#if defined(DEBUG_ENABLE)
static char sspWaitingMenu[] = "SSP Polling: waiting for transfer ...\n\r";
static char sspIntWaitingMenu[]  = "SSP Interrupt: waiting for transfer ...\n\r";
static char sspDMAWaitingMenu[]  = "SSP DMA: waiting for transfer ...\n\r";

static char sspPassedMenu[] = "SSP: Transfer PASSED\n\r";
static char sspFailedMenu[] = "SSP: Transfer FAILED\n\r";

static char sspTransferModeSel[] = "\n\rPress 1-3 or 'q' to exit\n\r"
								   "\t 1: SSP Polling Read Write\n\r"
								   "\t 2: SSP Int Read Write\n\r"
								   "\t 3: SSP DMA Read Write\n\r";

static char helloMenu[] = "Hello NXP Semiconductors \n\r";
static char sspMenu[] = "SSP demo \n\r";
static char sspMainMenu[] = "\t 1: Select SSP Mode (Master/Slave)\n\r"
							"\t 2: Select Transfer Mode\n\r";
static char sspSelectModeMenu[] = "\n\rPress 1-2 to select or 'q' to exit:\n\r"
								  "\t 1: Master \n\r"
								  "\t 2: Slave\n\r";
#endif /* defined(DEBUG_ENABLE) */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initialize buffer */
static void Buffer_Init(void)
{
	uint16_t i;
	uint8_t ch = 0;

	for (i = 0; i < BUFFER_SIZE; i++) {
		Tx_Buf[i] = ch++;
		Rx_Buf[i] = 0xAA;
	}
}

/* Verify buffer after transfer */
static uint8_t Buffer_Verify(void)
{
	uint16_t i;
	uint8_t *src_addr = (uint8_t *) &Tx_Buf[0];
	uint8_t *dest_addr = (uint8_t *) &Rx_Buf[0];

	for ( i = 0; i < BUFFER_SIZE; i++ ) {

		if (((*src_addr) & SSP_LO_BYTE_MSK(ssp_format.bits)) !=
				((*dest_addr) & SSP_LO_BYTE_MSK(ssp_format.bits))) {
				return 1;
		}
		src_addr++;
		dest_addr++;

		if (SSP_DATA_BYTES(ssp_format.bits) == 2) {
			if (((*src_addr) & SSP_HI_BYTE_MSK(ssp_format.bits)) !=
				  ((*dest_addr) & SSP_HI_BYTE_MSK(ssp_format.bits))) {
					return 1;
			}
			src_addr++;
			dest_addr++;
			i++;
		}
	}
	return 0;
}

/* Select the Transfer mode : Polling, Interrupt or DMA */
static void appSSPTest(void)
{
	int key;

//	DEBUGOUT(sspTransferModeSel);

	dmaChSSPTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, LPC_GPDMA_SSP_TX);
	dmaChSSPRx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, LPC_GPDMA_SSP_RX);

	xf_setup.length = BUFFER_SIZE;
	xf_setup.tx_data = Tx_Buf;
	xf_setup.rx_data = Rx_Buf;


/*
	while (1) {
		key = 0xFF;
		do {
			key = DEBUGIN();
		} while ((key & 0xFF) == 0xFF);
*/
		Buffer_Init();

		switch (key) {
			case SSP_POLLING_SEL:	/* SSP Polling Read Write Mode */
				//DEBUGOUT(sspWaitingMenu);
				xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

				Chip_SSP_RWFrames_Blocking(LPC_SSP, &xf_setup);
/*
				if (Buffer_Verify() == 0) {
					DEBUGOUT(sspPassedMenu);
				}
				else {
					DEBUGOUT(sspFailedMenu);
				}*/
				break;

			case SSP_INTERRUPT_SEL:
				//DEBUGOUT(sspIntWaitingMenu);

				isXferCompleted = 0;
				xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

				Chip_SSP_Int_FlushData(LPC_SSP);/* flush dummy data from SSP FiFO */
				if (SSP_DATA_BYTES(ssp_format.bits) == 1) {
					Chip_SSP_Int_RWFrames8Bits(LPC_SSP, &xf_setup);
				}
				else {
					Chip_SSP_Int_RWFrames16Bits(LPC_SSP, &xf_setup);
				}

				Chip_SSP_Int_Enable(LPC_SSP);	/* enable interrupt */
				while (!isXferCompleted) {}
/*
				if (Buffer_Verify() == 0) {
					DEBUGOUT(sspPassedMenu);
				}
				else {
					DEBUGOUT(sspFailedMenu);
				}*/
				break;

			case SSP_DMA_SEL:	/* SSP DMA Read and Write: fixed on 8bits */
				//DEBUGOUT(sspDMAWaitingMenu);
				isDmaTxfCompleted = isDmaRxfCompleted = 0;

				Chip_SSP_DMA_Enable(LPC_SSP);
				/* data Tx_Buf --> SSP */
				Chip_GPDMA_Transfer(LPC_GPDMA, dmaChSSPTx,
								  (uint32_t) &Tx_Buf[0],
								  LPC_GPDMA_SSP_TX,
								  GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
								  BUFFER_SIZE);
				/* data SSP --> Rx_Buf */
				Chip_GPDMA_Transfer(LPC_GPDMA, dmaChSSPRx,
								  LPC_GPDMA_SSP_RX,
								  (uint32_t) &Rx_Buf[0],
								  GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
								  BUFFER_SIZE);

				while (!isDmaTxfCompleted || !isDmaRxfCompleted) {}
/*
				if (Buffer_Verify() == 0) {
					DEBUGOUT(sspPassedMenu);
				}
				else {
					DEBUGOUT(sspFailedMenu);
				}*/
				Chip_SSP_DMA_Disable(LPC_SSP);
				break;

			case 'q':
			case 'Q':
				Chip_GPDMA_Stop(LPC_GPDMA, dmaChSSPTx);
				Chip_GPDMA_Stop(LPC_GPDMA, dmaChSSPRx);
				return;

			default:
				break;
		}

	//	DEBUGOUT(sspTransferModeSel);
	//}

}

/* Select the SSP mode : Master or Slave */
static void appSSPSelectModeMenu(void)
{
	int key;

//	DEBUGOUT(sspSelectModeMenu);
/*
	while (1) {
		key = 0xFF;
		do {
			key = DEBUGIN();
		} while ((key & 0xFF) == 0xFF);
*/
		switch (key) {
			case SSP_MASTER_MODE_SEL:	/* Master */
				Chip_SSP_SetMaster(LPC_SSP, 1);
				//DEBUGOUT("Master Mode\n\r");
				return;

			case SSP_SLAVE_MODE_SEL:	/* Slave */
				Chip_SSP_SetMaster(LPC_SSP, 0);
				//DEBUGOUT("Slave Mode\n\r");
				return;

			case 'q':
				return;

			default:
				break;
		}
	//	DEBUGOUT(sspSelectModeMenu);
	//}

}

/* The main menu of the example. Allow user select the SSP mode (master or slave) and Transfer
   mode (Polling, Interrupt or DMA) */
static void appSSPMainMenu(void)
{
	int key;
/*
	DEBUGOUT(helloMenu);
	DEBUGOUT(sspMenu);
	DEBUGOUT(sspMainMenu);

	while (1) {
		key = 0xFF;
		do {
			key = DEBUGIN();
		} while ((key & 0xFF) == 0xFF);
*/
		switch (key) {
		case SSP_MODE_SEL:	/* Select SSP Mode */
			appSSPSelectModeMenu();
			break;

		case SSP_TRANSFER_MODE_SEL:	/* Select Transfer Mode */
			appSSPTest();
			break;

		default:
			break;
		}
	//	DEBUGOUT(sspMainMenu);
//	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	SSP interrupt handler sub-routine
 * @return	Nothing
 */
void SSPIRQHANDLER(void)
{
	Chip_SSP_Int_Disable(LPC_SSP);	/* Disable all interrupt */
	if (SSP_DATA_BYTES(ssp_format.bits) == 1) {
		Chip_SSP_Int_RWFrames8Bits(LPC_SSP, &xf_setup);
	}
	else {
		Chip_SSP_Int_RWFrames16Bits(LPC_SSP, &xf_setup);
	}

	if ((xf_setup.rx_cnt != xf_setup.length) || (xf_setup.tx_cnt != xf_setup.length)) {
		Chip_SSP_Int_Enable(LPC_SSP);	/* enable all interrupts */
	}
	else {
		isXferCompleted = 1;
	}
}

/**
 * @brief	DMA interrupt handler sub-routine. Set the waiting flag when transfer is successful
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChSSPTx) == SUCCESS) {
		isDmaTxfCompleted = 1;
	}

	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChSSPRx) == SUCCESS) {
		isDmaRxfCompleted = 1;
	}
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

#define T_CMD_X		0xD3
#define T_CMD_Y		0x93
#define T_CMD_PRESS	0xB3
#define T_CMD_NOP		0x00
#define MIN_PRESS		200
#define MAX_PRESS		500
#define CAL_X			0
#define CAL_Y			240
#define PROMEDIO_TOUCH	5



void Touch_Init();
void Touch_Coordinates();
void Touch_Read( unsigned int * x, unsigned int * y );


void Touch_Init()
{
	char i,temp;

	// SSP
	// Power
//	LPC_SC->PCONP |= (1 << 10);	// SSP1

	// Clock
//	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~SSP1_CLK_MASK) | ( 0x0 & SSP1_CLK_MASK); // 00 - CCLK/4

	// Pin
//	LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~SSP1_PIN_MASK) | ( 0x000AA000 & SSP1_PIN_MASK); // SCK1, MOSI1, MISO1, SSEL1

	//Clear the Rx FIFO
	for( i = 0; i < SSP1_FIFO_SIZE; i++ ){
		temp = LPC_SSP1->DR; //data register
	}

	// Config
	// PCLK_SSP1 = CCLK / 4 = 30MHz
	// SPI_CLK = PCLK_SSP1 / [ CPSR * [ SCR + 1 ] ]  = 30MHz / [ 12 * [ 0 + 1 ]] = 2.5MHz    max frequency on the Touch SPI is 2.5MHz
	LPC_SSP1->CPSR = 0x0C; //even number to devide the SSP_PCLK by to create the bit clock		CPSR = 12
	LPC_SSP1->CR0 = 0x07; // 8bit, CPOL=0, CPHA=0	SCR = 0
	LPC_SSP1->CR1 = 0x02; // SSP1 enable, Master Mode

}

void Touch_Coordinates()
{

}



void leerTouch(unsigned int * x, unsigned int * y){
	unsigned int touchX = 0;
	unsigned int touchY = 0;
	unsigned int touchZ = 0;
	unsigned int basura = 0;
	unsigned int touchX_low= 0;
	unsigned int touchY_low= 0;
	unsigned int touchZ_low= 0;
	unsigned int tmpZ=0;
	
	basura= Chip_SSP_ReceiveFrame(LPC_SSP1);

	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_X);
	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_NOP);

	touchX= Chip_SSP_ReceiveFrame(LPC_SSP1);
	touchX_low= Chip_SSP_ReceiveFrame(LPC_SSP1);

	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_Y);
	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_NOP);

	touchY= Chip_SSP_ReceiveFrame(LPC_SSP1);
	touchY_low= Chip_SSP_ReceiveFrame(LPC_SSP1); 

	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_PRESS);
	Chip_SSP_SendFrame(LPC_SSP1, T_CMD_NOP);

	touchZ= Chip_SSP_ReceiveFrame(LPC_SSP1);
	touchZ_low= Chip_SSP_ReceiveFrame(LPC_SSP1); 
	tmpZ= ((touchZ << 0x04)& 0xff0) | touchZ_low;

	if((tmpZ > MIN_PRESS) && (tmpZ < MAX_PRESS)){
		if(contadorTouch < PROMEDIO_TOUCH){
			tmpX += (((touchX << 0x04)& 0xff0) | touchX_low) - CAL_X;
			tmpY += ((touchY << 0x04)& 0xff0) | touchY_low;
			contadorTouch++;
		}
		else{
			*x = tmpX / PROMEDIO_TOUCH;
			*y = tmpY / PROMEDIO_TOUCH;
			tmpX=0;
			tmpY=0;
			contadorTouch=0;
		}
	}
	else{
		*x=0;
		*y=0;
	}
		
}




