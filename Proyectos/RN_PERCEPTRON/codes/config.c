// **********************************************************************
//                Función para Inicializar el Cortex
// **********************************************************************

static void initHardware(void)
{
	Board_Init();


}



// **********************************************************************
//                Función de Configuración RIT Timer
// **********************************************************************
void cfgRITTimer(void)
{
		DEBUGINIT();
	SystemCoreClockUpdate();
	/* Activate Repetitive Interrupt Timer (RIT) for periodic IRQs */
	Chip_RIT_Init(LPC_RITIMER);
	Chip_RIT_SetTimerInterval(LPC_RITIMER, 1); /* 1ms Period */
	Chip_RIT_Enable(LPC_RITIMER);
	/* Set lowest priority for RIT */
	NVIC_SetPriority(RITIMER_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	/* Enable IRQ for RIT */
	NVIC_EnableIRQ(RITIMER_IRQn);
}



// **********************************************************************
//                Funciones de Configuración Cortex-M0 Esclavo
// **********************************************************************
#define CR_START_M0_H_
#define SLAVE_M0APP 0
#define SLAVE_M0SUB 1
#define RGU_RESET_CTRL1	          (*((volatile uint32_t *) 0x40053104))
#define RGU_RESET_ACTIVE_STATUS1  (*((volatile uint32_t *) 0x40053154))
#define RGU_RESET_CTRL0	          (*((volatile uint32_t *) 0x40053100))
#define RGU_RESET_ACTIVE_STATUS0  (*((volatile uint32_t *) 0x40053150))
#define CREG_M0APPMEMMAP	        (*((volatile uint32_t *) 0x40043404))
#define CREG_M0SUBMEMMAP          (*((volatile uint32_t *) 0x40043308))

/*******************************************************************
 * Static function to Release SLAVE processor from reset
 *******************************************************************/
static void startSlave(uint32_t slavenum) {

	volatile uint32_t u32REG, u32Val;

	if (slavenum <= SLAVE_M0SUB) {

		if (slavenum == SLAVE_M0APP) {
			/* Release Slave from reset, first read status */
			/* Notice, this is a read only register !!! */
			u32REG = RGU_RESET_ACTIVE_STATUS1;

			/* If the M0 is being held in reset, release it */
			/* 1 = no reset, 0 = reset */
			while (!(u32REG & (1u << 24))) {
				u32Val = (~(u32REG) & (~(1 << 24)));
				RGU_RESET_CTRL1 = u32Val;
				u32REG = RGU_RESET_ACTIVE_STATUS1;
			};
		}
		else { // (slavenum == SLAVE_M0SUB)
			/* Release Slave from reset, first read status */
			/* Notice, this is a read only register !!! */
			u32REG = RGU_RESET_ACTIVE_STATUS0;

			/* If the M0 is being held in reset, release it */
			/* 1 = no reset, 0 = reset */
			while (!(u32REG & (1u << 12))) {
				u32Val = (~(u32REG) & (~(1 << 12)));
				RGU_RESET_CTRL0 = u32Val;
				u32REG = RGU_RESET_ACTIVE_STATUS0;
			};

		}
	}
}

/*******************************************************************
 * Static function to put the SLAVE processor back in reset
 *******************************************************************/
static void haltSlave(uint32_t slavenum) {

	volatile uint32_t u32REG, u32Val;

	if (slavenum <= SLAVE_M0SUB) {

		if (slavenum == SLAVE_M0APP) {

			/* Check if M0 is reset by reading status */
			u32REG = RGU_RESET_ACTIVE_STATUS1;

			/* If the M0 has reset not asserted, halt it... */
			/* in u32REG, status register, 1 = no reset */
			while ((u32REG & (1u << 24))) {
				u32Val = ((~u32REG) | (1 << 24));
				RGU_RESET_CTRL1 = u32Val;
				u32REG = RGU_RESET_ACTIVE_STATUS1;
			}
		} else { // (slavenum == SLAVE_M0SUB)
			/* Check if M0 is reset by reading status */
			u32REG = RGU_RESET_ACTIVE_STATUS0;

			/* If the M0 has reset not asserted, halt it... */
			/* in u32REG, status register, 1 = no reset */
			while ((u32REG & (1u << 12))) {
				u32Val = ((~u32REG) | (1 << 12));
				RGU_RESET_CTRL0 = u32Val;
				u32REG = RGU_RESET_ACTIVE_STATUS0;
			}

		}
	}

}

/*******************************************************************
 * Function to start required CM0 slave cpu executing
 *******************************************************************/
void cr_start_m0(uint32_t slavenum, uint8_t *CM0image_start)  {

	if (slavenum <= SLAVE_M0SUB) {

		// Make sure M0 is not running
		haltSlave(slavenum);

		// Set M0's vector table to point to start of M0 image
		if (slavenum == SLAVE_M0APP) {
			CREG_M0APPMEMMAP = (uint32_t) CM0image_start;
		} else { // (slavenum == SLAVE_M0SUB)
			CREG_M0SUBMEMMAP = (uint32_t) CM0image_start;
		}
		// Release M0 from reset
		startSlave(slavenum);
	}
}




// **********************************************************************
//                Función de Configuración SysTick
// **********************************************************************


void Cfg_SysTick(void) {
	SysTick->CTRL = 0x00;
	SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL = 0x00; 
	SysTick->CTRL = 0x05;
   SysTick->CTRL = 0x07;
}


// **********************************************************************
//                Función de Configuración Entradas
// **********************************************************************

// Información tabla 191, pág. 412 

void Cfg_Pulsador(void) {
	SCU->SFSP[1][0] = ((SCU_MODE_FUNC0) | (1 << 6) | (0 << 3));
	SCU->SFSP[1][1] = ((SCU_MODE_FUNC0) | (1 << 6) | (0 << 3)); // Pulsador 1
	SCU->SFSP[1][2] = ((SCU_MODE_FUNC0) | (1 << 6) | (0 << 3)); // Pulsador 2
	SCU->SFSP[7][2] = ((SCU_MODE_FUNC0) | (1 << 6) | (0 << 3)); // Pulsador 3

	SCU->SFSP[1][6] = ((SCU_MODE_FUNC0) | (1 << 6) | (0 << 3)); // Pulsador 3

//	SCU->PINTSEL[0]= 0x04 | (0x08 << 8) | (0x09 << 16) | (1 << 29) | (9 << 24);	//	habilitaciòn de las 4 teclas
	SCU->PINTSEL[0]= 0x04 | (0x01 << 11) | (0x6A << 16); //TEC_1 , TEC_2 y GPIO(0,3)

  // GPIO_PORT->DIR[0]	= 0xff;
   GPIO_PORT->DIR[0] &= ~((1 << 4) | (1 << 8) | (1 << 9));	//Seteo de GPIO0_4, GPIO0_8, GPIO0_9 -> Entradas
	GPIO_PORT->DIR[1] |= ~(1 << 9);  // Seteo de GPIO1_9 como entrada
	
	GPIO_PIN_INT->ISEL &= 0x00;    // Interrupción sensible a flancos
	GPIO_PIN_INT->SIENR = 0x0f;	// habilitacion de las interrupciones por nivel o flanco		
   GPIO_PIN_INT->IENR= 0x0F;	
	
	_NVIC->IP[0x20] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_0
	_NVIC->IP[0x21] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_1 
	_NVIC->IP[0x22] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_2
	_NVIC->IP[0x23] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_3 


//Habilita Interrupcion en GPIO_0, GPIO_1, GPIO_2, GPIO_3
   _NVIC->ISER[1] |= (uint32_t) ((1 << (0x1F & 32)) | (1 << (0x1F & 33)) | (1 << (0x1F & 34)) | (1 << (0x1F & 35)));

}


// **********************************************************************
//                Función de Configuración Salidas
// **********************************************************************

void Cfg_Led(){

	// Configuracion del pin (LED1) como GPIO, con pull-up
	
	SCU->SFSP[2][10] = (SCU_MODE_EPUN | SCU_MODE_FUNC0); // P2_10, GPIO0[14], LED1
	SCU->SFSP[2][0] = (SCU_MODE_DES | SCU_MODE_FUNC4); // P2_10, GPIO5[0], LED0_R
	SCU->SFSP[2][1] = (SCU_MODE_DES | SCU_MODE_FUNC4); // P2_10, GPIO5[0], LED0_G
	SCU->SFSP[2][2] = (SCU_MODE_DES | SCU_MODE_FUNC4); // P2_10, GPIO5[0], LED0_B
	SCU->SFSP[2][11] = (SCU_MODE_DES | SCU_MODE_FUNC0);// P2_11, GPIO0[14], LED2
	SCU->SFSP[2][12] = (SCU_MODE_DES | SCU_MODE_FUNC0);// P2_12, GPIO0[14], LED3



	// Configuracion del pin (LED1) como salida 
	
	GPIO_PORT->DIR[0] |= 1 << 14;    // led Rojo
	GPIO_PORT->DIR[1] |= 1 << 11;    // Led Amarillo
	GPIO_PORT->DIR[1] |= 1 << 12;    // Led Verde
	GPIO_PORT->DIR[5] |= 1 << 0;
	GPIO_PORT->DIR[5] |= 1 << 1;
	GPIO_PORT->DIR[5] |= 1 << 2;

	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2));
	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0,(1<<14));
	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 1,(1<<12));
}


// **********************************************************************
//                Función de Configuración UARTx
// **********************************************************************

void Cfg_UART2(void){
/*
   //Inicializa Periferico
   Chip_UART_Init(CIAA_BOARD_UART);
   //Seteo del Baudrate
   Chip_UART_SetBaud(CIAA_BOARD_UART, SYSTEM_BAUD_RATE);

   //Seteo del Registro de Control de la FIFO
   Chip_UART_SetupFIFOS(CIAA_BOARD_UART, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);

   //Habilita la transmisión de la UART
   Chip_UART_TXEnable(CIAA_BOARD_UART);

   Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              // P7_1: UART2_TXD 
//   Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); // P7_2: UART2_RXD 
	SCU->SFSP[1][16] = ((SCU_MODE_FUNC1) | (1 << 6) | (0 << 3)); // UART2 RX



   //Habilita la Interrupción por Recepción de la UART en el registro Buffer
   Chip_UART_IntEnable(CIAA_BOARD_UART,UART_IER_RBRINT);  
   Chip_UART_IntEnable(CIAA_BOARD_UART,UART_IER_RLSINT);  

   // Enable UART line status interrupt
   //Chip_UART_IntEnable(CIAA_BOARD_UART,UART_IER_RLSINT ); //LPC43xx User manual page 1118
   NVIC_SetPriority(USART2_IRQn, 6);
	//_NVIC->IP[0x1A] = ((0x06 << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_0
   
   // Enable Interrupt for UART channel
   //NVIC_EnableIRQ(USART2_IRQn);
	//NVIC_EnaIRQ(USART2_IRQn);
	//_NVIC->ISER[1] |= (uint32_t)(1 << ((uint32_t)((int32_t)26) & (uint32_t)0x1F));  //Habilita Interrupcion de UART2  */

	Chip_UART_Init(USART2);

	Chip_UART_SetupFIFOS(USART2, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS ));

	Chip_UART_SetBaud(USART2, 115200);

	Chip_UART_TXEnable(USART2);

	Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              /* P2_3: UART3_TXD */
	Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); /* P2_4: UART3_RXD */

	Chip_UART_IntEnable(USART2, UART_IER_RBRINT);  //UART_IER_RBRINT);
	NVIC_EnableIRQ(USART2_IRQn);

}


#if defined ( __Cfg_M4__ )

void InicializaIPC (void) {
   LPC_RGU->RESET_CTRL1 = (1 << 24);
	LPC_CREG->M0APPMEMMAP = 0x1B000000;              // Dirección conde comienza a ejecutarse el código de M0;
	cr_start_m0(SLAVE_M0APP, (uint8_t *)0x1B000000);  	// Inicio Cortex-M0 Esclavo 
	LPC_RGU->RESET_CTRL1 = 0;
	NVIC_SetPriority(M0APP_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(M0APP_IRQn);
}

#endif


#if defined ( __Cfg_M0__ )

void InicializarIPC (void) {
	LPC_CREG->M0APPMEMMAP = 0x1B000000;    //M0_CODE_START;
	NVIC_SetPriority(M4_IRQn, ((0x01 << 3) | 0x01));  
   NVIC_EnableIRQ(M4_IRQn);  
}

#endif


