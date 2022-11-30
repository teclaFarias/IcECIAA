
// **********************************************************************
//                Rutinas de Interrupción para el Cortex M4
// **********************************************************************

#if defined ( __IRQs_M4__ )

//Interrupción de SysTick

void SysTick_Handler(void){
	if(systickcount > 0) 
		systickcount--;	// variable global declarada en funciones.c 
}

// **********************************************************************
//Atención para la Interrupción del Cortex M0 al Cortex M4

void M0APP_IRQHandler(void)
{
		signal_M0= true;
	   CREG_M0TXEVENT = 0x0;  //Apago la señal para que el M0 pueda volver a interrumpir
}

// **********************************************************************
//Genero señal interrupción desde el Cortex M4

void signal_m4(void)
{
	__DSB();
	__SEV();
}

// **********************************************************************
//Atención para la Interrupción del GPIO0: TEC 1  


void Reset_IRQHandler(void){
	main();
}


// **********************************************************************
//Atención para la Interrupción del GPIO0: TEC 1  

void GPIO0_IRQHandler(void){
	GPIO_PIN_INT->IST |= (1 << 0);  //Bloqueo el resto de IRQs.
		tec1= true;
	Led_ON(1);
}

// **********************************************************************
//Atención para la Interrupción del GPIO1: TEC 2  

void GPIO1_IRQHandler(void){
	GPIO_PIN_INT->IST |= (1 << 0);  //Bloqueo el resto de IRQs.
	tec2= true;
	Led_OFF(1);
}

// **********************************************************************
//Atención para la Interrupción del GPIO2: TEC 3  

void GPIO2_IRQHandler(void){
	GPIO_PIN_INT->IST |= (1 << 0);  //Bloqueo el resto de IRQs.
	tec3= true;
}

// **********************************************************************
//Atención para la Interrupción del GPIO3: TEC 4  

void GPIO3_IRQHandler(void){
	GPIO_PIN_INT->IST |= (1 << 0);  //Bloqueo el resto de IRQs.
	tec4= true;
}


#endif


// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************


// **********************************************************************
//                Rutinas de Interrupción para el Cortex M0
// **********************************************************************

#if defined ( __IRQs_M0__ )


// **********************************************************************
//Interrupción del RIT Timer

void RIT_IRQHandler(void)
{
	if(Chip_RIT_GetIntStatus(LPC_RITIMER) == SET) {

		if(ritcount > 0) 
			ritcount--;

		Chip_RIT_ClearInt(LPC_RITIMER);
		NVIC_ClearPendingIRQ(RITIMER_IRQn);
	}
}

// **********************************************************************
//Atención para la Interrupción del Cortex M4 al Cortex M0


void MX_CORE_IRQHandler(void)
{
	//	signal_M4= true;
	switch(MemoriaIPC.comando2_M0){
		case NUEVO_MENSAJE:
				m4print();
				MemoriaIPC.comando2_M0= STOP;
				break;
		case NUEVA_LECTURA:
				sensor=1;
				MemoriaIPC.comando2_M0= STOP;
				break;
		default:
			break;
	}
	CREG_M4TXEVENT = 0x0; //Apago la señal para que el M4 pueda volver a interrumpir
} 


// **********************************************************************
//Genero señal interrupción desde el Cortex M0

void signal_m0(void)
{
	__DSB();
	__SEV();
}


// **********************************************************************
//Atención para la Interrupción del Puerto Serie 2  

void UART2_IRQHandler(void){
	unsigned char caracterRx_UART2;

	caracterRx_UART2= Chip_UART_ReadByte(USART2);  //Obtenemos caracter
	if(cargarBuffer == true){  // && ( estadoBufferSerie == false)){
		bufferSerie[indiceSerie]= caracterRx_UART2;
		indiceSerie++;
		if(indiceSerie == BUFFER_SERIE){
			indiceSerie= 0;
			estadoBufferSerie= true;   //Buffer lleno
			cargarBuffer = false;
		}
	}
	else{		
			dataRx_UART2[contadorRx_UART2]= caracterRx_UART2;

			if(contadorRx_UART2 < 4)			
				indiceFunciones |= dataRx_UART2[contadorRx_UART2] << (24 - (contadorRx_UART2 * 8)); 

			if(contadorRx_UART2 > 3)
				valorFuncion |= dataRx_UART2[contadorRx_UART2] << (56 - (contadorRx_UART2 * 8)); 

			contadorRx_UART2++;

			if(contadorRx_UART2 == BUFFER_RX_UART2){  // Si se extiende del límite del buffer se descarta el dato y se limpia el buffer.
				limpiaBufferUART2();
				cmdProcesar[contCmd][0]= indiceFunciones;
				cmdProcesar[contCmd][1]= valorFuncion; 
				indiceFunciones=0;
				valorFuncion=0;
				contCmd++;
				instruccionPendiente++;   // Esta lista la instrucción para ser procesada.
				contadorRx_UART2=0;
			}
			
			if(contCmd == BUFFER_COMANDOS_SERIE)
				contCmd=0;

			if(instruccionPendiente == BUFFER_INSTRUCCIONES_SERIE)
				instruccionPendiente=0;
	}
}


#endif








