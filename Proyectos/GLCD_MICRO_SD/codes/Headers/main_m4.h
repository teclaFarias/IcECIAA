// #define	NO_LIBS
//#include "../../lpcopen_lpc4337/inc/board.h"
//#include <stdbool.h>


#define SYSTEM_BAUD_RATE 115200
//#define CIAA_BOARD_UART USART2
#define CIAA_BOARD_UART LPC_USART2

enum LEDS {LED1, LED2, LED3, LEDR, LEDG, LEDB}; 

#define MASCARA_PINES      (0x0 << 3) | (0x1 << 6) | (0x1 << 7)
#define PULSADORES    4

/** led number to toggle */
#define LED 3

#define DAC_BIAS_1MHz	((unsigned int) (0 << 16))
#define DAC_BIAS_400kHz ((unsigned int) (1 << 16))

/************************************************************************************
 GPIO
 ************************************************************************************/

#define GPIO_PORT_BASE			0x400F4000	// direccion base del registro (pag. 455 / tabla 245)
#define GPIO_PORT0_B_OFFSET		0x0000		// tabla 259
#define GPIO_PORT1_B_OFFSET		0x0020		// tabla 259
#define GPIO_PORT0_DIR_OFFSET	0x2000		// tabla 261
#define GPIO_PORT1_DIR_OFFSET	0x2004		// tabla 261
#define GPIO_PORT0_SET_OFFSET	0x2200		// tabla 265
#define GPIO_PORT1_SET_OFFSET	0x2204		// tabla 265
#define GPIO_PORT0_CLR_OFFSET	0x2280		// tabla 266
#define GPIO_PORT1_CLR_OFFSET	0x2284		// tabla 266

#define PIN_INT_BASE			0x40087000		// Tabla 242, pag. 453
#define GPIO_PIN_INT			((PIN_INT_T *) PIN_INT_BASE)


#define GPIO_PORT				((GPIO_T *)	GPIO_PORT_BASE)

/************************************************************************************
 System Control Unit (SCU)
 ************************************************************************************/

#define SCU_BASE			0x40086000

// Offset de los registros de configuracion para los pines (pag. 405)
// Leds		
#define	SFSP2_0				0x100	// pin P2_0  -> LED0_R (Puerto 5, Pin 0)
#define	SFSP2_1				0x104	// pin P2_1  -> LED0_G (Puerto 5, Pin 1)
#define	SFSP2_2				0x108	// pin P2_2  -> LED0_B (Puerto 5, Pin 2)
#define	SFSP2_10			0x128	// pin P2_10 -> LED1   (Puerto 0, Pin 14)
#define	SFSP2_11			0x12C	// pin P2_11 -> LED2   (Puerto 1, Pin 11)
#define	SFSP2_12			0x130	// pin P2_12 -> LED3   (Puerto 1, Pin 12)

// Funcion y modo (pag. 413)
#define SCU_MODE_EPD		(0x0 << 3)		// habilita la resistencia de pull-down (deshabilita con 0)
#define SCU_MODE_EPUN		(0x0 << 4)		// habilita la resistencia de pull-up (deshabilita con 1)
#define SCU_MODE_DES		(0x2 << 3)		// deshabilita las resistencias de pull-down y pull-up
#define SCU_MODE_EHZ		(0x1 << 5)		// 1 Rapido (ruido medio con alta velocidad)
												// 0 Lento (ruido bajo con velocidad media)
#define SCU_MODE_EZI		(0x1 << 6)		// habilita buffer de entrada (deshabilita con 0)
#define SCU_MODE_ZIF_DIS	(0x1 << 7)		// deshabilita el filtro anti glitch de entrada (habilita con 1)

#define SCU_MODE_FUNC0		0x0				// seleccion de la funcion 0 del pin
#define SCU_MODE_FUNC1		0x1				// seleccion de la funcion 1 del pin
#define SCU_MODE_FUNC2		0x2				// seleccion de la funcion 2 del pin
#define SCU_MODE_FUNC3		0x3				// seleccion de la funcion 3 del pin
#define SCU_MODE_FUNC4		0x4				// seleccion de la funcion 4 del pin
#define SCU_MODE_FUNC5		0x5				// seleccion de la funcion 5 del pin
#define SCU_MODE_FUNC6		0x6				// seleccion de la funcion 6 del pin

#define SCU					((SCU_T         *) 	SCU_BASE)

#define _NVIC_BASE			0xE000E100		// NVIC Base Address (Tabla 81, pag. 115)
#define _NVIC				((_NVIC_Type    *)  _NVIC_BASE)

/************************************************************************************
 General Purpose DMA (GPDMA)
 ************************************************************************************/
#define GPDMA_BASE  	0x40002000
#define GPDMA			((GPDMA_T*) GPDMA_BASE)

/************************************************************************************
	DAC	(Conversor Digital-Analogico)																	    
 ************************************************************************************/
#define DAC_BASE		0x400E1000
#define DAC				((DAC_T	*)	DAC_BASE)
//Registro de Control
#define AHB_M1       (0x01<<25)
#define SRCADDR_INC  (0x01<<26)
#define DESTWIDTH_32 (0x02<<21)
#define SRCWIDTH_32  (0x02<<18)

// DCAR DACCTRL mask bit
//#define DAC_DACCTRL_MASK_MIO    ((unsigned int) (0x0F))

// DMA request mask
#define DAC_INT_DMA_REQ_MASK_MIO	((unsigned int) (1 << 0))
// Doble buffer DMA mask
#define DAC_DBLBUF_ENA_MIO		((unsigned int) (1 << 1))
// DAC/DMA Time out count enable
#define DAC_CNT_ENA_MIO				((unsigned int) (1 << 2))
// DMA ena access		
#define DAC_DMA_ENA_MIO				((unsigned int) (1 << 3))


// Macro para el calculo de direcciones
#define ADDRESS(x, offset) (*(volatile int *)(volatile char *) ((x)+(offset)))

/************************************************************************************
 UART / USART
 ************************************************************************************/
/*
#define USART0_BASE		0x40081000
#define USART2_BASE		0x400C1000
#define USART3_BASE		0x400C2000
#define UART1_BASE		0x40082000

#define USART0			((USART_T	*) USART0_BASE)
#define USART2			((USART_T	*) USART2_BASE)
#define USART3			((USART_T	*) USART3_BASE)
#define UART1			((USART_T	*) UART1_BASE)  */

/************************************************************************************
 *	UART / USART																	*
 ************************************************************************************/
#define USART0_BASE           0x40081000
#define USART2_BASE           0x400C1000
#define USART3_BASE           0x400C2000
#define UART1_BASE            0x40082000

#define USART0				((USART_T	*) LPC_USART0_BASE)
#define USART2				((USART_T	*) USART2_BASE)
#define USART3				((USART_T	*) LPC_USART3_BASE)
#define UART1				((USART_T	*) LPC_UART1_BASE)

#define _UART_IER_RBRINT	(1 << 0)	// RBR Interrupt enable

// *********************************************************************************
// Impresión
// *********************************************************************************

#define PAD_RIGHT 1
#define PAD_ZERO 2

/************************************************************************************
 Systtick
 ************************************************************************************/
#define _SysTick_BASE		0xE000E010	// Systick Base Address
#define _SysTick			((SysTick_T 	*) 	_SysTick_BASE )

// SysTick CTRL: Mascaras (ARM pag. 4-33/4-35)
#define _SysTick_CTRL_CLKSOURCE_PROC_Msk	(1 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_CLKSOURCE_EXT_Msk		(0 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_TICKINT_Msk	(1 << 1)	// SysTick CTRL: TICKINT Mask
#define _SysTick_CTRL_ENABLE_Msk	(1 << 0)	// SysTick CTRL: ENABLE Mask

/*
#ifdef NO_LIBS
typedef enum {
	// -------------------------  Cortex-M4 Processor Exceptions Numbers  ----------------------------- 
	Reset_IRQn                = -15, //  1  Reset Vector, invoked on Power up and warm reset
	NonMaskableInt_IRQn       = -14, //  2  Non maskable Interrupt, cannot be stopped or preempted
	HardFault_IRQn            = -13, //  3  Hard Fault, all classes of Fault
	MemoryManagement_IRQn     = -12, //  4  Memory Management, MPU mismatch, including Access Violation and No Match
	BusFault_IRQn             = -11, //  5  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory related Fault
	UsageFault_IRQn           = -10, //  6  Usage Fault, i.e. Undef Instruction, Illegal State Transition
	SVCall_IRQn               =  -5, // 11  System Service Call via SVC instruction
	DebugMonitor_IRQn         =  -4, // 12  Debug Monitor                    
	PendSV_IRQn               =  -2, // 14  Pendable request for system service
	SysTick_IRQn              =  -1, // 15  System Tick Timer

	// ----------------------  Numero de interrupciones especificas del LPC4337  ---------------------- 
	DAC_IRQn                  =   0, //  0  DAC                              
	M0APP_IRQn                =   1, //  1  M0APP Core interrupt             
	DMA_IRQn                  =   2, //  2  DMA                              
	RESERVED1_IRQn            =   3, //  3  EZH/EDM                          
	RESERVED2_IRQn            =   4,
	ETHERNET_IRQn             =   5, //  5  ETHERNET                         
	SDIO_IRQn                 =   6, //  6  SDIO                             
	LCD_IRQn                  =   7, //  7  LCD                              
	USB0_IRQn                 =   8, //  8  USB0                             
	USB1_IRQn                 =   9, //  9  USB1                             
	SCT_IRQn                  =  10, // 10  SCT                              
	RITIMER_IRQn              =  11, // 11  RITIMER                          
	TIMER0_IRQn               =  12, // 12  TIMER0                           
	TIMER1_IRQn               =  13, // 13  TIMER1                           
	TIMER2_IRQn               =  14, // 14  TIMER2                           
	TIMER3_IRQn               =  15, // 15  TIMER3                           
	MCPWM_IRQn                =  16, // 16  MCPWM                            
	ADC0_IRQn                 =  17, // 17  ADC0                             
	I2C0_IRQn                 =  18, // 18  I2C0                             
	I2C1_IRQn                 =  19, // 19  I2C1                             
	SPI_INT_IRQn              =  20, // 20  SPI_INT                          
	ADC1_IRQn                 =  21, // 21  ADC1                             
	SSP0_IRQn                 =  22, // 22  SSP0                             
	SSP1_IRQn                 =  23, // 23  SSP1                             
	USART0_IRQn               =  24, // 24  USART0                           
	UART1_IRQn                =  25, // 25  UART1                            
	USART2_IRQn               =  26, // 26  USART2                           
	USART3_IRQn               =  27, // 27  USART3                           
	I2S0_IRQn                 =  28, // 28  I2S0                             
	I2S1_IRQn                 =  29, // 29  I2S1                             
	RESERVED4_IRQn            =  30,
	SGPIO_INT_IRQn            =  31, // 31  SGPIO_IINT                       
	GPIO0_IRQn 		           =  32, // 32  PIN_INT0                         
	GPIO1_IRQn                =  33, // 33  PIN_INT1                         
	GPIO2_IRQn                =  34, // 34  PIN_INT2                         
	GPIO3_IRQn                =  35, // 35  PIN_INT3                         
	PIN_INT4_IRQn             =  36, // 36  PIN_INT4                         
	PIN_INT5_IRQn             =  37, // 37  PIN_INT5                         
	PIN_INT6_IRQn             =  38, // 38  PIN_INT6                         
	PIN_INT7_IRQn             =  39, // 39  PIN_INT7                         
	GINT0_IRQn                =  40, // 40  GINT0                            
	GINT1_IRQn                =  41, // 41  GINT1                            
	EVENTROUTER_IRQn          =  42, // 42  EVENTROUTER                      
	C_CAN1_IRQn               =  43, // 43  C_CAN1                           
	RESERVED6_IRQn            =  44,
	ADCHS_IRQn                =  45, // 45  ADCHS interrupt                  
	ATIMER_IRQn               =  46, // 46  ATIMER                           
	RTC_IRQn                  =  47, // 47  RTC                              
	RESERVED8_IRQn            =  48,
	WWDT_IRQn                 =  49, // 49  WWDT                             
	M0SUB_IRQn                =  50, // 50  M0SUB core interrupt             
	C_CAN0_IRQn               =  51, // 51  C_CAN0                           
	QEI_IRQn                  =  52, // 52  QEI                              
} IRQn_Type;
#endif

*/

// Registros para el canal del GPDMA
typedef struct {
	unsigned int  SRCADDR;		// DMA Channel Source Address Register
	unsigned int  DESTADDR;		// DMA Channel Destination Address Register
	unsigned int  LLI;			// DMA Channel Linked List Item Register
	unsigned int  CONTROL;		// DMA Channel Control Register
	unsigned int  CONFIG;		// DMA Channel Configuration Register
	unsigned int  RESERVED1[3];
} _GPDMA_CH_T;

// // Registros para el GPDMA
typedef struct {
	unsigned int  INTSTAT;			// DMA Interrupt Status Register
	unsigned int  INTTCSTAT;		// DMA Interrupt Terminal Count Request Status Register
	unsigned int  INTTCCLEAR;		// DMA Interrupt Terminal Count Request Clear Register
	unsigned int  INTERRSTAT;		// DMA Interrupt Error Status Register
	unsigned int  INTERRCLR;		// DMA Interrupt Error Clear Register
	unsigned int  RAWINTTCSTAT;		// DMA Raw Interrupt Terminal Count Status Register
	unsigned int  RAWINTERRSTAT;	// DMA Raw Error Interrupt Status Register
	unsigned int  ENBLDCHNS;		// DMA Enabled Channel Register
	unsigned int  SOFTBREQ;			// DMA Software Burst Request Register
	unsigned int  SOFTSREQ;			// DMA Software Single Request Register
	unsigned int  SOFTLBREQ;		// DMA Software Last Burst Request Register
	unsigned int  SOFTLSREQ;		// DMA Software Last Single Request Register
	unsigned int  CONFIG;			// DMA Configuration Register
	unsigned int  SYNC;				// DMA Synchronization Register
	unsigned int  RESERVED0[50];
	_GPDMA_CH_T   CH[GPDMA_NUMBER_CHANNELS];
} GPDMA_T;

// Registros para el DAC
typedef struct {			// DAC Structure
	unsigned int	CR;		// DAC register. Holds the conversion data.
	unsigned int	CTRL;	// DAC control register
	unsigned int	CNTVAL;	// DAC counter value register
} DAC_T;

 // GPIO Register
typedef struct {				// Estructura para GPIO
	unsigned char B[128][32];	// Offset 0x0000: Byte pin registers ports 0 to n; pins PIOn_0 to PIOn_31 */
	int W[32][32];				// Offset 0x1000: Word pin registers port 0 to n
	int DIR[32];				// Offset 0x2000: Direction registers port n
	int MASK[32];				// Offset 0x2080: Mask register port n
	int PIN[32];				// Offset 0x2100: Portpin register port n
	int MPIN[32];				// Offset 0x2180: Masked port register port n
	int SET[32];				// Offset 0x2200: Write: Set register for port n Read: output bits for port n
	int CLR[32];				// Offset 0x2280: Clear port n
	int NOT[32];				// Offset 0x2300: Toggle port n
} GPIO_T;

// System Control Unit Register
typedef struct {
	int  SFSP[16][32];		// Los pines digitales estan divididos en 16 grupos (P0-P9 y PA-PF)
	int  RESERVED0[256];
	int  SFSCLK[4];			// Pin configuration register for pins CLK0-3
	int  RESERVED16[28];
	int  SFSUSB;			// Pin configuration register for USB
	int  SFSI2C0;			// Pin configuration register for I2C0-bus pins
	int  ENAIO[3];			// Analog function select registers
	int  RESERVED17[27];
	int  EMCDELAYCLK;		// EMC clock delay register
	int  RESERVED18[63];
	int  PINTSEL[2];		// Pin interrupt select register for pin int 0 to 3 index 0, 4 to 7 index 1
} SCU_T;

// SysTick (System Timer)
typedef struct {
  int CTRL;					// Offset: 0x000 (R/W)  SysTick Control and Status Register
  int LOAD;					// Offset: 0x004 (R/W)  SysTick Reload Value Register
  int VAL;					// Offset: 0x008 (R/W)  SysTick Current Value Register
  int CALIB;				// Offset: 0x00C (R/ )  SysTick Calibration Register
} SysTick_T;

// Pin Interrupt and Pattern Match register block structure
typedef struct {			
	int ISEL;				// Pin Interrupt Mode register
	int IENR;				// Pin Interrupt Enable (Rising) register
	int SIENR;				// Set Pin Interrupt Enable (Rising) register
	int CIENR;				// Clear Pin Interrupt Enable (Rising) register
	int IENF;				// Pin Interrupt Enable Falling Edge / Active Level register
	int SIENF;				// Set Pin Interrupt Enable Falling Edge / Active Level register
	int CIENF;				// Clear Pin Interrupt Enable Falling Edge / Active Level address
	int RISE;				// Pin Interrupt Rising Edge register
	int FALL;				// Pin Interrupt Falling Edge register
	int IST;				// Pin Interrupt Status register
} PIN_INT_T;

typedef struct {
	int ISER[8];			// Offset: 0x000 (R/W)  Interrupt Set Enable Register 
	int RESERVED0[24];
	int ICER[8];			// Offset: 0x080 (R/W)  Interrupt Clear Enable Register
	int RSERVED1[24];
	int ISPR[8];			// Offset: 0x100 (R/W)  Interrupt Set Pending Register
	int RESERVED2[24];
	int ICPR[8];			// Offset: 0x180 (R/W)  Interrupt Clear Pending Register
	int RESERVED3[24];
	int IABR[8];			// Offset: 0x200 (R/W)  Interrupt Active bit Register
	int RESERVED4[56];
	unsigned char IP[240];	// Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide)
    int RESERVED5[644];
	int STIR;				// Offset: 0xE00 ( /W)  Software Trigger Interrupt Register
} _NVIC_Type;




// USART Register
typedef struct {

	union {
		unsigned int  DLL; // Divisor Latch LSB. Least significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1).
		unsigned int  THR; // Transmit Holding Register. The next character to be transmitted is written here (DLAB = 0).
		unsigned int  RBR; // Receiver Buffer Register. Contains the next received character to be read (DLAB = 0).
	};

	union {
		unsigned int IER;	// Interrupt Enable Register. Contains individual interrupt enable bits for the 7 potential UART interrupts (DLAB = 0).
		unsigned int DLM;	// Divisor Latch MSB. Most significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1).
	};

	union {
		unsigned int FCR;	// FIFO Control Register. Controls UART FIFO usage and modes.
		unsigned int IIR;	// Interrupt ID Register. Identifies which interrupt(s) are pending.
	};

	unsigned int LCR;		// Line Control Register. Contains controls for frame formatting and break generation.
	unsigned int MCR;		// Modem Control Register. Only present on USART ports with full modem support.
	unsigned int LSR;		// Line Status Register. Contains flags for transmit and receive status, including line errors.
	unsigned int MSR;		// Modem Status Register. Only present on USART ports with full modem support.
	unsigned int SCR;		// Scratch Pad Register. Eight-bit temporary storage for software.
	unsigned int ACR;		// Auto-baud Control Register. Contains controls for the auto-baud feature.
	unsigned int ICR;		// IrDA control register (not all UARTS)
	unsigned int FDR;		// Fractional Divider Register. Generates a clock input for the baud rate divider.
	unsigned int OSR;		// Oversampling Register. Controls the degree of oversampling during each bit time. Only on some UARTS.
	unsigned int TER1;		// Transmit Enable Register. Turns off USART transmitter for use with software flow control.
	unsigned int RESERVED0[3];
    unsigned int HDEN;		// Half-duplex enable Register- only on some UARTs
	unsigned int RESERVED1[1];
	unsigned int SCICTRL;	// Smart card interface control register- only on some UARTs

	unsigned int RS485CTRL;	// RS-485/EIA-485 Control. Contains controls to configure various aspects of RS-485/EIA-485 modes.
	unsigned int RS485ADRMATCH;	// RS-485/EIA-485 address match. Contains the address match value for RS-485/EIA-485 mode.
	unsigned int RS485DLY;		// RS-485/EIA-485 direction control delay.

	union {
		unsigned int SYNCCTRL;	// Synchronous mode control register. Only on USARTs.
		unsigned int FIFOLVL;	// FIFO Level register. Provides the current fill levels of the transmit and receive FIFOs.
	};

	unsigned int TER2;			// Transmit Enable Register. Only on LPC177X_8X UART4 and LPC18XX/43XX USART0/2/3.
} USART_T;


STATIC INLINE void UART_SendByte(USART_T *pUART, unsigned char data){
	pUART->THR = (unsigned int) data;
}





typedef struct {                            /*!< (@ 0x40043000) CREG Structure         */
  __I  uint32_t  IRCTRM;                    /*!< (@ 0x40043000) IRC trim register      */
  __IO uint32_t  CREG0;                     /*!< (@ 0x40043004) Chip configuration register 32 kHz oscillator output and BOD control register. */
  __I  uint32_t  RESERVED1[62];
  __IO uint32_t  M4MEMMAP;                  /*!< (@ 0x40043100) ARM Cortex-M4 memory mapping */
  __I  uint32_t  RESERVED2[5];
  __IO uint32_t  CREG5;                     /*!< (@ 0x40043118) Chip configuration register 5. Controls JTAG access. */
  __IO uint32_t  DMAMUX;                    /*!< (@ 0x4004311C) DMA muxing control     */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  ETBCFG;                    /*!< (@ 0x40043128) ETB RAM configuration  */
  __IO uint32_t  CREG6;                     /*!< (@ 0x4004312C) Chip configuration register 6. */
  __IO uint32_t  M4TXEVENT;                 /*!< (@ 0x40043130) Cortex-M4 TXEV event clear */
  __I  uint32_t  RESERVED4[51];
  __I  uint32_t  CHIPID;                    /*!< (@ 0x40043200) Part ID                */
  __I  uint32_t  RESERVED5[127];
  __IO uint32_t  M0TXEVENT;                 /*!< (@ 0x40043400) Cortex-M0 TXEV event clear */
  __IO uint32_t  M0APPMEMMAP;               /*!< (@ 0x40043404) ARM Cortex-M0 memory mapping */
} LPC_CREG_Type;

#define CREG_M0TXEVENT	        (*((volatile uint32_t *) 0x40043400))
#define CREG_M4TXEVENT	        (*((volatile uint32_t *) 0x40043130))
// -------------------------------------  CREG_M4TXEVENT  -----------------------------------------
#define CREG_M4TXEVENT_TXEVCLR_Pos                            0                                                         /*!< CREG M4TXEVENT: TXEVCLR Position    */
#define CREG_M4TXEVENT_TXEVCLR_Msk                            (0x01UL << CREG_M4TXEVENT_TXEVCLR_Pos)                    /*!< CREG M4TXEVENT: TXEVCLR Mask        */

// ---------------------------------------  CREG_CHIPID  ------------------------------------------
#define CREG_CHIPID_ID_Pos                                    0                                                         /*!< CREG CHIPID: ID Position            */
#define CREG_CHIPID_ID_Msk                                    (0xffffffffUL << CREG_CHIPID_ID_Pos)                      /*!< CREG CHIPID: ID Mask                */

// -------------------------------------  CREG_M0TXEVENT  -----------------------------------------
#define CREG_M0TXEVENT_TXEVCLR_Pos                            0                                                         /*!< CREG M0TXEVENT: TXEVCLR Position    */
#define CREG_M0TXEVENT_TXEVCLR_Msk                            (0x01UL << CREG_M0TXEVENT_TXEVCLR_Pos)                    /*!< CREG M0TXEVENT: TXEVCLR Mask        */

// ------------------------------------  CREG_M0APPMEMMAP  ----------------------------------------
#define CREG_M0APPMEMMAP_M0APPMAP_Pos                         12                                                        /*!< CREG M0APPMEMMAP: M0APPMAP Position */
#define CREG_M0APPMEMMAP_M0APPMAP_Msk                         (0x000fffffUL << CREG_M0APPMEMMAP_M0APPMAP_Pos)           /*!< CREG M0APPMEMMAP: M0APPMAP Mask     */



// *********************************** Comunicación entre los Cortex M0 - M4 ***********************************

// Estado de las funciones del Cortex M0 - M4
typedef enum {
	NO_BUSY = 0,     //Procesador sin actividad
	BUSY = 1         //Procesador Ocupado
} estado_cortex_t;

// Comandos entre los Cortex M0 - M4
typedef enum {
	STOP = 0,			  // Finaliza tarea
	PENDIENTE= 1,		  // Indica si una tarea fue realizada y tiene que ser atendida por el otro procesador
	ELEMENTO_RGB = 2,    // Nuevo elemento RGB para procesar.
	NUEVO_MENSAJE= 3,
	OBTENER_COLOR= 4,
	NUEVA_LECTURA= 5
} comando_t;


typedef struct  
{
	unsigned char comando2_M0;      // Comando enviado al Cortex M0
	unsigned char comando2_M4;      // Comando enviado al Cortex M4
	bool estado_M0;        			  // Cortex M0:  0= no está ocupado, 1= se encuentra ocupado
	bool estado_M4;         		  // Cortex M4:  0= no está ocupado, 1= se encuentra ocupado
	float valor_M0;            // Variable datos de 8 bits
	float valor_M4;            // Variable datos de 8 bits
	uint8_t rgb[3];				// Variable para enviar valores RGB
	uint8_t mensaje[10240];		// Array para mensajes: 20 bloques de 512 bytes
} ipc_t;


// *************************************************************************************************************



/*********************************************************************************
   Encabezados de funciones
 *********************************************************************************/


void GPIO_SetPinDIROutput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin);
void GPIO_SetPinDIRInput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin); 
int GPIO_GetPinState(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin);
void LED_ON(enum LEDS led);
void LED_OFF(enum LEDS led);
void SCU_GPIOIntPinSel(unsigned char PortSel, unsigned char PortNum, unsigned char PinNum);
void NVIC_SetPri(IRQn_Type IRQn, unsigned int priority);
void NVIC_EnaIRQ(IRQn_Type IRQn);
void printchar(char **str, int c);
int prints(char **out, const char *string, int width, int pad);
int printi(char **out, int i, int b, int sg, int width, int pad, int letbase);
int print(char **out, int *varg);
int sprintf_mio(char *out, const char *format, ...);
void Cfg_UART2(void);
void Cfg_Pulsador(void);
void Cfg_SysTick(void);
void Cfg_Led();
void delayms(int tiempo);






