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


	SCU->SFSP[6][4] = ((SCU_MODE_FUNC0) | (1 << 6) | (0 << 3)); // GPIO1 -> Entrada para el sensor de color
	SCU->SFSP[4][1] = ((SCU_MODE_FUNC0) | (1 << 6) | (0 << 3)); // T_FIL 1 -> GPIO(2,1)


//	SCU->PINTSEL[0]= 0x04 | (0x08 << 8) | (0x09 << 16) | (1 << 29) | (9 << 24);	//	habilitaciòn de las 4 teclas
	SCU->PINTSEL[0]= 0x04 | (0x01 << 11) | (0x6A << 16) | (0x41 << 24); //TEC_1 , TEC_2 , GPIO(0,3), GPIO(2,1)

  // GPIO_PORT->DIR[0]	= 0xff;
   GPIO_PORT->DIR[0] &= ~((1 << 4) | (1 << 8) | (1 << 9));	//Seteo de GPIO0_4, GPIO0_8, GPIO0_9 -> Entradas
	GPIO_PORT->DIR[1] |= ~(1 << 9);  // Seteo de GPIO1_9 como entrada
	GPIO_PORT->DIR[3] &= ~(1 << 3);  // Seteo de GPIO3_1 como entrada

	GPIO_PORT->DIR[2] &= ~(1 << 3);  // Seteo de GPIO2_1 como entrada
	
	GPIO_PIN_INT->ISEL &= 0x00;    // Interrupción sensible a flancos
	GPIO_PIN_INT->SIENR = 0x0f;	// habilitacion de las interrupciones por nivel o flanco		
   GPIO_PIN_INT->IENR= 0x0F;	
	
	_NVIC->IP[0x20] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_0
	_NVIC->IP[0x21] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_1 
	_NVIC->IP[0x22] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_2
	_NVIC->IP[0x23] = ((0x0f << (8 - 2)) & 0xff);	// Prioridad Interrupción en GPIO_3 


//Habilito en el Vector de Interrupciones: GPIO_0, GPIO_1, GPIO_2, GPIO_3
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


	SCU->SFSP[6][1] = (SCU_MODE_DES | SCU_MODE_FUNC0);// P6_1, GPIO3[0]  -> (Sensor de color)-> Pin: S1
	SCU->SFSP[6][5] = (SCU_MODE_DES | SCU_MODE_FUNC0);// P6_5, GPIO3[4]  -> (Sensor de color)-> Pin: S0
	SCU->SFSP[6][7] = (SCU_MODE_DES | SCU_MODE_FUNC4);// P6_7, GPIO5[15] -> (Sensor de color)-> Pin: S2
	SCU->SFSP[6][9] = (SCU_MODE_DES | SCU_MODE_FUNC0);// P6_9, GPIO3[5]  -> (Sensor de color)-> Pin: S3



	// Configuracion del pin (LED1) como salida 
	
	GPIO_PORT->DIR[0] |= 1 << 14;    // led Rojo
	GPIO_PORT->DIR[1] |= 1 << 11;    // Led Amarillo
	GPIO_PORT->DIR[1] |= 1 << 12;    // Led Verde
	GPIO_PORT->DIR[5] |= 1 << 0;
	GPIO_PORT->DIR[5] |= 1 << 1;
	GPIO_PORT->DIR[5] |= 1 << 2;

	GPIO_PORT->DIR[3] |= 1 << 0;
	GPIO_PORT->DIR[3] |= 1 << 4;
	GPIO_PORT->DIR[5] |= 1 << 15;
	GPIO_PORT->DIR[3] |= 1 << 5;


	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2));
	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0,(1<<14));
	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 1,(1<<12));

	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 3,(1<<0));
	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 3,(1<<4));
	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 5,(1<<15));
	Chip_GPIO_ClearValue(LPC_GPIO_PORT, 3,(1<<5));


}


// **********************************************************************
//                Función de Configuración UARTx
// **********************************************************************

void Cfg_UART2(void){

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
	NVIC_SetPriority( M0APP_IRQn, ((0x01 << 3) | 0x01));
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


