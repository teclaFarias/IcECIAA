
// **********************************************************************
//                Funciones de Pines I/O
// **********************************************************************


void GPIO_SetPinDIROutput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin) {
	pGPIO->DIR[puerto] |= 1 << pin;
}

void GPIO_SetPinDIRInput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin) {
	pGPIO->DIR[puerto] &= ~(1 << pin);
}

int GPIO_GetPinState(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin){
	 int var; 
	 var = (GPIO_PORT->PIN[puerto] >> pin) & 0x0001;
	 return(var) ;
}


void Led_ON(unsigned char led) {
	
	switch(led) {
		case 0:
			GPIO_PORT->SET[0] |= (1 << 14);
			break;
		case 1:
			GPIO_PORT->SET[1] |= (1 << 11);
			break;
		case 2:
			GPIO_PORT->SET[1] |= 1 << 12;
			break;
		case 3:
			GPIO_PORT->SET[5] |= 1 << 0;
			break;
		case 4:
			GPIO_PORT->SET[5] |= 1 << 1;
			break;
		case 5:
			GPIO_PORT->SET[5] |= 1 << 2;
			break;
	}
}

void Led_OFF(unsigned char led) {
	
	switch(led) {
		case 0:
			GPIO_PORT->CLR[0] |= (1 << 14);
			break;
		case 1:
			GPIO_PORT->CLR[1] |= (1 << 11);
			break;
		case 2:
			GPIO_PORT->CLR[1] |= 1 << 12;
			break;
		case 3:
			GPIO_PORT->CLR[5] |= 1 << 0;
			break;
		case 4:
			GPIO_PORT->CLR[5] |= 1 << 1;
			break;
		case 5:
			GPIO_PORT->CLR[5] |= 1 << 2;
			break;
	}
}



bool EstadoPulsador(unsigned char puls){
	unsigned char tmp= false;
	switch(puls){
           case 0:
        	      tmp= GPIO_GetPinState(GPIO_PORT,0, 4);
           	   break;
           case 1:
        	      tmp= GPIO_GetPinState(GPIO_PORT,0, 8);  
               break;
           case 2:
        	      tmp= GPIO_GetPinState(GPIO_PORT,0, 9);
               break;
           case 3:
        	      tmp= GPIO_GetPinState(GPIO_PORT,1, 9);
               break;
		}
	return tmp;
}


unsigned char EstadoPulsadores(){
   unsigned char i=0;
   unsigned char valor=0;
   for(i=0; i< 4; i++){
      if(EstadoPulsador(i) == 0){
         valor |= (1<< i);
      }
   }
   return valor;
}



// **********************************************************************
//                Funciones de Demora
// **********************************************************************

unsigned int systickcount=0;

void delay_systick_ms(unsigned int tiempo){
	systickcount= tiempo;
	while(systickcount != 0){
		__WFI();   //espera por una interrupción. Manual Cortex-M4 pág 47.
	}
}

unsigned int ritcount=0;

void delay_rit_ms(unsigned int tiempo){
	ritcount= tiempo;
	while(ritcount != 0){
		__WFI();  
	}
}


/*
static void pausems(uint32_t t)
{
	pausems_count = t;
	while (pausems_count != 0) {
		__WFI();
	}
}*/

// **********************************************************************
//                Funciones de Impresión UART
// **********************************************************************

void printchar(char **str, int c)
{
	if (str) {
		**str = c;
		++(*str);
	}
	else 
		Chip_UART_SendByte(CIAA_BOARD_UART, c);
}



int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

#define PRINT_BUF_LEN 120

int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}


int print(char **out, int *varg)
{
	register int width, pad;
	register int pc = 0;
	register char *format = (char *)(*varg++);
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = *((char **)varg++);
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, *varg++, 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, *varg++, 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, *varg++, 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, *varg++, 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				// char are converted to int then pushed on the stack 
				scr[0] = *varg++;
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	return pc;
}


int sprintf_mio(char *out, const char *format, ...)
{
	register int *varg = (int *)(&format);
	return print(&out, varg);
}


/*
int print2M0(char *out, const char *format, ...)
{
	register int *varg = (int *)(&format);
	//return print(&out, varg);
	for(int i=0; i< 12; i++){
		MemoriaIPC.mensaje[i]= *(varg +i);
	}
}
*/

int m4print(){
	sprintf_mio(aux,"%s", MemoriaIPC.mensaje);
	DEBUGSTR(aux);	
	return 1;
}



/************************************************************************
 *          Funciones para seteo de Interrupciones
 ************************************************************************/

void NVIC_SetPri(IRQn_Type IRQn, unsigned int priority){
	if(IRQn < 0) {
	}
	else {
		_NVIC->IP[(unsigned int)(IRQn)] = ((priority << (8 - 2)) & 0xff);
	}
}

void NVIC_EnaIRQ(IRQn_Type IRQn){
	_NVIC->ISER[(unsigned int)((int)IRQn) >> 5] = (unsigned int)(1 << ((unsigned int)((int32_t)IRQn) & (unsigned int)0x1F));
}


// ************************** **** **************************************



void limpiaBufferAUX(){
	for(int a=0; a< LONGITUD_MENSAJE; a++)
		aux[a]= "";
}

void limpiaBufferUART2(){
	for(int a=0; a< BUFFER_RX_UART2; a++)
		dataRx_UART2[a]= "0";
}












