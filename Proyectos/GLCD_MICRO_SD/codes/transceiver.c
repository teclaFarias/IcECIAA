
double cantBloques=0;
double bloqueSD=0;
double indiceSD=0;
int indiceBloqueSD=0;
unsigned char str[512];
unsigned char ctr[512];



void chipSelectHigh(void){
	GPIO_PORT->SET[0] |= (1 << 13);  // TXD0
}


void chipSelectLow(void){
	GPIO_PORT->CLR[0] |= (1 << 13);  // TXD0
}


unsigned char ReadSPI(void){
	uint8_t data_in=0;
	Chip_SSP_ReadFrames_Blocking(LPC_SSP1, &data_in, sizeof(data_in));
	xf_setup.rx_data= data_in;
	//data= Chip_SSP_ReceiveFrame(LPC_SSP1); 
   return data_in;
}

signed char WriteSPI( unsigned char data_out ){

	xf_setup.tx_data = data_out;
 	Chip_SSP_WriteFrames_Blocking(LPC_SSP1, &data_out, sizeof(data_out));
	//Chip_SSP_SendFrame(LPC_SSP1, data_out);
   return(0);
}

void Command(char cmd, unsigned long int arg, char CRCbits){
   unsigned char temp;
	chipSelectLow();
   WriteSPI(0xff);
   WriteSPI(cmd);
   WriteSPI((unsigned char) (arg >> 24));
   WriteSPI((unsigned char) (arg >> 16));
   WriteSPI((unsigned char) (arg >> 8));
   WriteSPI((unsigned char)  arg);
   WriteSPI(CRCbits);
	chipSelectHigh();
}

/*SOFTWARE RESET FUNCTION FOR SD_CARD*/
unsigned char response(){
   unsigned char buff=0x00;
   chipSelectLow(); 
   buff=ReadSPI();   //read buffer (R1) should be 0x01 = idle mode   
   chipSelectHigh();
   return buff;
}


void dummy_clocks(int n){
   int i = 0;
   delay_rit_ms(1);
	chipSelectHigh();
   for(i=0; i<n; i++){     
   	WriteSPI(0xFF);
   }
   chipSelectLow();
}


void proceed(){
    chipSelectLow(); 
    WriteSPI(0xFF); // Give Time For SD_CARD To Proceed
    chipSelectHigh();
}



// *************************************************************************
// Escritura y lectura de bloques de memoria



void write_block()  
{
    unsigned char buff; 
    int i=0;

    dummy_clocks(8);    
    Command(0x58, 0x00000A00, 0x00); //CMD24
    proceed();
    do{   
        buff = response();
      }while(buff!=0x00);

      sprintf_mio(aux,"CMD24 Accepted");
		DEBUGSTR(aux);	
		limpiaBufferAUX();

      delay_rit_ms(1000);
      dummy_clocks(8);    
      WriteSPI_(0xFE); //START TOKEN
      
      for(i=0;i<(512*2);i++) //DATA BLOCK
      {
          WriteSPI_(0x41);
          i++;
      }
      
      WriteSPI_(0xFF); //CRC
      WriteSPI_(0xFF); //CRC
      
    dummy_clocks(1);
      
 
      delay_rit_ms(1000);
      dummy_clocks(8);    
      Command(0x4D,0x00000000,0xFF);
    proceed();
      do{   
        buff = response();
        }while(buff!=0x00);
      sprintf_mio(aux,"BLOCK WRITE OVER");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
return;      
}

void multi_write()
{
    unsigned char data = 0x41,buff; 
    int i=0,j;

    dummy_clocks(8);    

    Command(0x59, 0x00000A00, 0x00); //CMD25
    proceed();
    do{   
        buff = response();
      }while(buff!=0x00);
    /*SOFTWARE RESET RESPONSE BIT OBTAINED (0X01)*/
      sprintf_mio(aux,"CMD25 Accepted\n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();

      delay_rit_ms(2000);

      dummy_clocks(8);
      while(i!=4)
      {
      WriteSPI_(0xFC); //START TOKEN
      i++;      
      for(j=0;j<1024;j++) //DATA BLOCK
      {
          WriteSPI_(data);
          j++;
      }
      data++;
      WriteSPI_(0xFF); // CRC 2 BYTES
      WriteSPI_(0xFF);
    dummy_clocks(1);
      delay_rit_ms(1000);
      dummy_clocks(8);
      }


      WriteSPI_(0xFD); //STOP TOKEN
    dummy_clocks(1);
      delay_rit_ms(1000);
      dummy_clocks(8);    
      Command(0x4D,0x00000000,0x00); //CMD13
    proceed();
      do{   
        buff = response();
        }while(buff!=0x00);
      sprintf_mio(aux,"MULTI WRITE OVER\n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();

      delay_rit_ms(2000);
return;      
}

void multi_read()
{
    unsigned char ptr=0x00,buff;
    unsigned long int start_add;
    int length,count;
    /*Read Sector*/
    delay_rit_ms(1000);
    dummy_clocks(8);    

    Command(0x52,0x00000A00,0x00); //CMD18
    proceed();
    do{   
        buff = response();
      }while(buff!=0x00);

    dummy_clocks(1);
      do{
        buff = response();
      }while(buff!=0xFE);
      
      count = 2;
      length = 0;
            
      while(count){
      while ( length < 512 )               
        {
            ctr[length] = response();
            length++;                   
        }
      count--;
      length = 0;
      while ( length < 512 )               
        {
            str[length] = response(); /* A different array is used to store the read data due to memory constraints
                                       * Please use same array, if memory is not an issue*/
            length++;                   
        }
      count--;
      }
    length = 0;
    
    
    delay_rit_ms(1000);
    dummy_clocks(8);    
    Command(0x4C,0x00000600,0x00); //CMD12
    proceed();
    do{   
        buff = response();
      }while(buff!=0xFF);

	   sprintf_mio(aux,"\r\n %s \r\n", ctr);
		DEBUGSTR(aux);	
		limpiaBufferAUX();

    
    delay_rit_ms(1000);
    sprintf_mio(aux,"\r\n");
	 DEBUGSTR(aux);	
	 limpiaBufferAUX();

    length = 0;
    
	   sprintf_mio(aux,"\r\n %s \r\n", str);
		DEBUGSTR(aux);	
		limpiaBufferAUX();

    delay_rit_ms(1000);
      sprintf_mio(aux,"\r\n\r\nMulti Read Over\n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
}


void read_block()
{
    unsigned char buff, readdata;
    int length,count,i=0;
    delay_rit_ms(1000);
    dummy_clocks(8);    
    count = 6;
    Command(0x51,0x00000000,0xFF);

    do{   
        buff = response();
      }while(buff!=0x00);
      do{   
        buff = response();
      }while(buff!=0xFE);

          length = 0;
          chipSelectLow(); // CS
          
            while(length<512)
            {
                str[length]=response();  //read buffer (R1) should be 0x01 = idle mode   
                length++;
            }
          length = 0;

	   sprintf_mio(aux,"\r\n 0xFF \r\n");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	   sprintf_mio(aux,"\r\n 0xFF \r\n");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
          
            delay_rit_ms(1000);
            dummy_clocks(8);    
            Command(0x4C,0x00000000,0x00); //COMMAND12 MANDATORY
            proceed();
            do{   
                buff = response();
              }while(buff!=0xFF);

          sprintf_mio(aux,"BLOCK READ OVER\n\r");
			 DEBUGSTR(aux);	
			 limpiaBufferAUX();

      delay_rit_ms(2000);
      length = 0;
      i = 0;
      
	   sprintf_mio(aux,"\r\n %s \r\n", str);
		DEBUGSTR(aux);	
		limpiaBufferAUX();

return;
}


// ***********************************************************************************************
// Procesos de INICIALIZACION

/*
bool reset(){
	int i = 0, count = 0;
   unsigned char buff=0x00;
	unsigned char intentosConexion=5;
   do{
		dummy_clocks(100);        
		Command(0x40, 0x00000000, 0x95); //CMD0
		proceed(); // tiempo para que procese
		do{   
		    buff = response();
		    count++;
		  }while((buff!=0x01) && (count<10)); //Wait Till SD Card is Inserted
		intentosConexion--;
	}while(intentosConexion > 0 && buff != 0x01);
	

	if(buff == 0x01)
   	return true;  // Micro SD presente y se logró el RESET de la tarjeta
	else
		return false; // Quizá no exista o no esté conectada la Micro SD
}

bool reset2(){
	int i = 0, count = 0;
   unsigned char buff=0x00;
	unsigned char intentosConexion=5;
   do{
		dummy_clocks(1);        
		Command(0x48, 0x1AA, 0x87); //CMD8
		proceed(); // tiempo para que procese
		do{   
		    buff = response();
		    count++;
		  }while((buff!=0x01) && (count<10));
		intentosConexion--;
	}while(intentosConexion > 0 && buff != 0x01);
	

	if(buff == 0x01)
   	return true;  // Respuesta del CMD 8
	else
		return false; // No responde CMD 8
}


bool reset3(){
	int i = 0, count = 0;
   unsigned char buff=0x00;
	unsigned char intentosConexion=5;
   do{
		dummy_clocks(1);        
		Command(0x41, 0x000, 0x0); //CMD1
		proceed(); // tiempo para que procese
		do{   
		    buff = response();
		    count++;
		  }while((buff!=0x01) && (count<10));
		intentosConexion--;
	}while(intentosConexion > 0 && buff != 0x01);
	

	if(buff == 0x01)
   	return true;  // Respuesta del CMD 1
	else
		return false; // No responde CMD 1
}

bool reset4(){
	int i = 0, count = 0;
   unsigned char buff=0x00;
	unsigned char intentosConexion=5;
   do{
		dummy_clocks(1);        
		Command(0x56, 512, 0x0); //CMD1
		proceed(); // tiempo para que procese
		do{   
		    buff = response();
		    count++;
		  }while(((buff >> 0x07) == 0x01) && (count<10));
		intentosConexion--;
	}while(intentosConexion > 0 && ((buff >> 0x07) == 0x01));
	

	if(((buff >> 0x07) == 0x00))
   	return true;  // Respuesta del CMD 16
	else
		return false; // No responde CMD 16
}

*/

/*Comando X con respuesta de Tipo R1 */
bool comandoR1(unsigned char comando, unsigned long int argumento, unsigned char crc){
	int i = 0, count = 0;
   unsigned char buff=0x00;
	unsigned char intentosConexion=5;
   do{
		dummy_clocks(1);        
		Command((comando | 0x40), argumento, crc); //CMD X
		proceed(); // tiempo para que procese
		do{   
		    buff = response();
		    count++;
		  }while(((buff >> 0x07) == 0x01) && (count<20));
		intentosConexion--;
	}while(intentosConexion > 0 && ((buff >> 0x07) == 0x01));
	

	if(((buff >> 0x07) == 0x00))
   	return true;  // Respuesta del CMD X
	else
		return false; // No responde CMD X
}




// Lectura de Bloque de Memoria

uint8_t read_data_block(uint8_t* buf, uint16_t n){
	uint16_t k=0;
	uint8_t res=0;
	uint8_t crc=0;

	res=0xFF; 
	res=response();
	res=response();

	for(k=0;k<n;k++)
		buf[k]=response(); 	// read n bytes
	crc=response();
	crc=response();   		// Read CRC (16 bits) and ignore it
	return(0);
}




uint8_t read_CSD(unsigned char* buf){
	uint8_t res=0;

	chipSelectLow(); 
   if (comandoR1(0x49, 0, 0)){ 
		res=read_data_block(buf,16);
	}
	chipSelectHigh(); 

	return res;
}




uint8_t read_CID(unsigned char* buf){
	uint8_t res=0;

	chipSelectLow();	
   if (comandoR1(0x4A, 0, 0)){ 
		res=read_data_block(buf,16);
	}
	chipSelectHigh();

	return res;
}



void dump_bytes(uint8_t* buf, uint16_t n){
	uint16_t k=0;
	int i=0;
	int contador=0;

	for(i=0; i < (n/8); i++){
		for (k=0;k < 8;k++){ 
			sprintf_mio(aux," %d ",buf[contador]);
			DEBUGSTR(aux);	
	 		limpiaBufferAUX();
			contador++;
		}
		sprintf_mio(aux,"\n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}  
}



void show_CID_data(uint8_t* CID){
	uint8_t k=0,rev=0;
	uint32_t sn=0;
	uint16_t date=0;

	sn = CID[9]; 
	for (k=10;k<=12;k++)
		sn=(sn<<8)+CID[k];

	date = CID[13]&0x0F;
	date<<=8;
	date+=CID[14]; 
	rev=CID[8];

	CID[8]=0;

	sprintf_mio(aux,"Manufacturer & Application ID %d/%c%c \n\r",CID[0],(char)CID[1],(char)CID[2]);
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	sprintf_mio(aux,"Product: %s \n\r",CID+3);
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	sprintf_mio(aux,"(%d.%d) S/N: %d \n\r",rev>>4,rev&0x0F,(unsigned int)sn);
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	sprintf_mio(aux,"Manufacturing date: %d/%d \n\r",date&0x0F,2000+(date>>4));
	DEBUGSTR(aux);	
	limpiaBufferAUX();
 
}




void show_CSD_data(uint8_t* CSD){
	uint16_t MBYTE=7;	
	uint32_t NSEC=0;
	uint16_t CSIZE=0,MULT=0,BYTES_PER_SECTOR=0;
	uint8_t READ_BL_PART=0,CSIZEMULT=0, BL=0;
	uint8_t version=0, SPI_speed=0;
	uint8_t k=0;

	//dump_bytes(CSD,16);
	version = (CSD[0]>>6)+1;

	if(CSD[3] == 0x32) 
		SPI_speed=25; 
	if(CSD[3] == 0x5A) 
		SPI_speed=50; 

	BL = CSD[5]&0x0F; 
	BYTES_PER_SECTOR=1; 
	BYTES_PER_SECTOR <<= BL;

	READ_BL_PART = CSD[6] >> 7;

	if(version==1){
		CSIZE = (CSD[6] & 0x03); 
		CSIZE <<= 8; 
		CSIZE += CSD[7]; 
		CSIZE <<= 2; 
		CSIZE += (CSD[8] >> 6);
		CSIZEMULT= ((CSD[9] << 1) + (CSD[10] >> 7)) & 0x07;
		NSEC = CSIZE; 
		NSEC <<= (CSIZEMULT+2);
		MBYTE = (NSEC >> (20-BL));
   }
  

	if(version == 2){
		CSIZE = CSD[7] & 0x03; 
		CSIZE <<= 8; 
		CSIZE += CSD[8]; 
		CSIZE <<= 8; 
		CSIZE += CSD[9]; 
		CSIZE++;
		MBYTE = (CSIZE >> 1);
		NSEC = CSIZE; 
		NSEC <<= 10; 
   }

	sprintf_mio(aux,"Version %d: Max SPI speed %d MHz \n\r",version,SPI_speed);
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	if(READ_BL_PART){ 
		sprintf_mio(aux,"Partial Reads allowed \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	else{ 
		sprintf_mio(aux,"Only block access -> SDHC card \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}

	sprintf_mio(aux,"Sector size = %d bytes. # sectors = %d \n\r",(unsigned int)BYTES_PER_SECTOR, (unsigned int)NSEC);
	DEBUGSTR(aux);	
	limpiaBufferAUX();


	sprintf_mio(aux,"Total Capacity %d Mbytes \n\r",MBYTE);
	DEBUGSTR(aux);	
	limpiaBufferAUX();

	if(MBYTE == 971){
		Chip_SSP_SetBitRate(LPC_SSP1, 400000);   //32000);
		sprintf_mio(aux,"Clock SPI reconfigurado a 400kHz \n\r",MBYTE);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
}



bool writeBlockSD(unsigned long int address, unsigned char * data ){
	unsigned char buff=0; 
	unsigned char count=0;
	int i=0;

	dummy_clocks(8);  	
   if(comandoR1(0x58, address, 0x00)){  // CMD 24
		chipSelectLow();
      WriteSPI(0xFE); //START TOKEN
      
      for(i=0; i < BUFFER_SERIE; i++){
          WriteSPI(data[i]);
      }
      
      WriteSPI(0xFF); //CRC
      WriteSPI(0xFF); //CRC
      chipSelectHigh();

		do{   
		    buff = response();
		    count++;
		  }while(((buff >> 0x04) == 0x01) && (count<20));

      dummy_clocks(8);  

		if((buff & 0x1F) == 0x05){  
			return true;
		}
	}
	chipSelectHigh();   
	return false;
}





bool readBlockSD(unsigned long int address){
	unsigned char buff=0, readdata=0;
	int length=0,count=0,i=0;
   dummy_clocks(8);    
   count = 6;
	if(comandoR1(17, address ,0xFF)){

      do{
        buff = response();
      }while(buff!=0xFE);

	   length = 0;
      while(length < BUFFER_SERIE){
	      str[length]=response();  //read buffer (R1) should be 0x01 = idle mode   
         length++;
      }
		length = 0;
		  
		dummy_clocks(8);    
		if(comandoR1(0x4C,0x00000000,0x00)){   //COMMAND12 MANDATORY
			return true;
		}
      delay_rit_ms(100);
      length = 0;
      i = 0;
	}
	return false;
}




unsigned char extraerByteSD(unsigned long int dato ){
	unsigned int bloqueElemento=0;
	unsigned int desplazamiento=0;
	unsigned char lectura=0;
	bloqueElemento = (int)(addressSD / BUFFER_SERIE); // Obtengo el bloque del elemento respectivo
	desplazamiento= (int)(addressSD % BUFFER_SERIE);  // Obtengo el desplazamiento dentro del bloque 
	readBlockSD(bloqueElemento * BUFFER_SERIE);
	lectura= str[(unsigned int)desplazamiento];
	return (lectura);
}


bool escribirByteSD(unsigned char dato ){
	unsigned int bloqueElemento=0;
	unsigned int desplazamiento=0;
	bloqueElemento = (int)(addressSD / BUFFER_SERIE); // Obtengo el bloque del elemento respectivo
	desplazamiento= (int)(addressSD % BUFFER_SERIE);  // Obtengo el desplazamiento dentro del bloque 
	readBlockSD(bloqueElemento * BUFFER_SERIE);				 // Obtengo el estado actual del bloque en str
	str[(unsigned int)desplazamiento]= dato;			 // Modifico str
	if(writeBlockSD(bloqueElemento * BUFFER_SERIE, &str)) // Escribo el bloque con los datos str actualizados
		return true;
	else
		return false;
}



bool initSD(){
   int i = 0;
   unsigned char buff=0x01;
	char salidaCmd[59];

	for(i=0; i< 59; i++)
		salidaCmd[i]=-1;
    
   if(comandoR1(0,0x00,0x95))  
		salidaCmd[0]=1;
	else{
		salidaCmd[0]=0;
		return false;
	}


   if(comandoR1( 8, 0x1AA, 0x87))  
		salidaCmd[8]=1;
	else{
		salidaCmd[8]=0;
		return false;
	}


   if(comandoR1( 1, 0x000, 0x0))  
		salidaCmd[1]=1;
	else{
		salidaCmd[1]=0;
		return false;
	}

   if(comandoR1(22, 512, 0x0))  
		salidaCmd[22]=1;
	else{
		salidaCmd[22]=0;
		return false;
	}


	for(i=0; i < 59; i++){
		if(salidaCmd[i] == 0){
			sprintf_mio(aux,"\n\rComando [%d]: ERROR \n\r", i);
	 		DEBUGSTR(aux);	
	 		limpiaBufferAUX();
		}
		if(salidaCmd[i] == 1){
			sprintf_mio(aux,"\n\rComando [%d]: OK \n\r", i);
	 		DEBUGSTR(aux);	
	 		limpiaBufferAUX();
		}
	}
	return true;
}



bool lecturaRegSD(){
	uint8_t data[32];
	read_CID(data); 
	show_CID_data(data);
	read_CSD(data); 
	show_CSD_data(data);
	return true;
}



bool actualizarDatos(unsigned char * datos){
		for(int i=0; i < 512; i++){
			if(i< 255)
				datos[i]= 0x34;
			else
				datos[i]= 0x80; //i - 256;
		}
	return true;
}


bool subirImagenSD(){
	float bloque=0;
	double i=0;
	bool salida=false;

	for(i=1; i < 3; i++){		
		salida=writeBlockSD(bloque, &str);
		if(!salida){
			sprintf_mio(aux,"\r\n Ocurrio un Error al transferir el Bloque: %d \r\n", i);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
			return false;
		}
		bloque += (i* 512); 
		actualizarDatos(&str);	
	}
	return true;
}



bool leerImagenSD(float bloque){
	int i=0;
	for(i=1; i < 3; i++){		
		if(!readBlockSD(bloque)){
			sprintf_mio(aux,"\r\n Ocurrio un Error al leer el Bloque: %d \r\n", i);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
		}
		bloque += (i* 512);
		dump_bytes(str, 512);
	} 
	return true;
}



bool cargarArraySD(unsigned long int bloque){
	int i=0;
	for(i=0; i < 20; i++){		
		if(!readBlockSD(bloque)){
			sprintf_mio(aux,"\r\n Ocurrio un Error al leer el Bloque: %d \r\n", i);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
			return false;
		}
		for(int b=0; b< BUFFER_SERIE; b++)
			MemoriaIPC.mensaje[(i* BUFFER_SERIE)+b]= str[b];
		bloque += BUFFER_SERIE;
	} 
	return true;
}




bool importarImagenSD(){
	bool salida=false;

	if(cantBloques){
		salida=writeBlockSD(bloqueSD, &str);
		if(!salida){
			sprintf_mio(aux,"\r\n Ocurrio un Error al transferir el Bloque: %d \r\n", indiceSD);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
			return false;
		}
		indiceSD++;
		bloqueSD += (indiceSD * 512);
		cantBloques--; 

		if(cantBloques > 0){
			sprintf_mio(aux,"\r\n Resta transferir %d Bloques \r\n", cantBloques);
			DEBUGSTR(aux);	
			limpiaBufferAUX();
		}
		else{
			sprintf_mio(aux,"\r\n Finalizo la transferencia de Bloques \r\n");
			DEBUGSTR(aux);	
			limpiaBufferAUX();
		}
		return true;
	}
	else
		return false;
}




