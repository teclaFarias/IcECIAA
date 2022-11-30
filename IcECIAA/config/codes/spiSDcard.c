

//*********************************************************************************************
// Archivo SdInfo.h

// SD card commands
//  GO_IDLE_STATE - init card in spi mode if CS low 
uint8_t const SD_CMD0 = 0X00;
//  SEND_IF_COND - verify SD Memory Card interface operating condition.
uint8_t const CMD8 = 0X08;
//  SEND_CSD - read the Card Specific Data (CSD register) 
uint8_t const CMD9 = 0X09;
//  SEND_CID - read the card identification information (CID register) 
uint8_t const CMD10 = 0X0A;
//  SEND_STATUS - read the card status register 
uint8_t const CMD13 = 0X0D;
//  READ_BLOCK - read a single data block from the card 
uint8_t const CMD17 = 0X11;
//  WRITE_BLOCK - write a single data block to the card 
uint8_t const CMD24 = 0X18;
//  WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION 
uint8_t const CMD25 = 0X19;
//  ERASE_WR_BLK_START - sets the address of the first block to be erased 
uint8_t const CMD32 = 0X20;
//  ERASE_WR_BLK_END - sets the address of the last block of the continuous
//  range to be erased
uint8_t const CMD33 = 0X21;
//  ERASE - erase all previously selected blocks 
uint8_t const CMD38 = 0X26;
//  APP_CMD - escape for application specific command 
uint8_t const CMD55 = 0X37;
//  READ_OCR - read the OCR register of a card 
uint8_t const CMD58 = 0X3A;
//  SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be
//     pre-erased before writing 
uint8_t const ACMD23 = 0X17;
//  SD_SEND_OP_COMD - Sends host capacity support information and
//    activates the card's initialization process 
uint8_t const ACMD41 = 0X29;
//------------------------------------------------------------------------------
//  status for card in the ready state 
uint8_t const R1_READY_STATE = 0X00;
//  status for card in the idle state 
uint8_t const R1_IDLE_STATE = 0X01;
//  status bit for illegal command 
uint8_t const SD_R1_ILLEGAL_COMMAND = 0X04;
//  start data token for read or write single block
uint8_t const DATA_START_BLOCK = 0XFE;
//  stop token for write multiple blocks
uint8_t const STOP_TRAN_TOKEN = 0XFD;
//  start data token for write multiple blocks
uint8_t const WRITE_MULTIPLE_TOKEN = 0XFC;
//  mask for data response tokens after a write block operation 
uint8_t const DATA_RES_MASK = 0X1F;
//  write data accepted token 
uint8_t const DATA_RES_ACCEPTED = 0X05;
//------------------------------------------------------------------------------
typedef struct CID {
  // byte 0
  uint8_t mid;  // Manufacturer ID
  // byte 1-2
  char oid[2];  // OEM/Application ID
  // byte 3-7
  char pnm[5];  // Product name
  // byte 8
  unsigned prv_m : 4;  // Product revision n.m
  unsigned prv_n : 4;
  // byte 9-12
  uint32_t psn;  // Product serial number
  // byte 13
  unsigned mdt_year_high : 4;  // Manufacturing date
  unsigned reserved : 4;
  // byte 14
  unsigned mdt_month : 4;
  unsigned mdt_year_low :4;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}cid_t;
//------------------------------------------------------------------------------
// CSD for version 1.00 cards
typedef struct CSDV1 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  uint8_t c_size_mid;
  // byte 8
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low :2;
  // byte 9
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved4 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved5: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}csd1_t;
//------------------------------------------------------------------------------
// CSD for version 2.00 cards
typedef struct CSDV2 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  // byte 8
  uint8_t c_size_mid;
  // byte 9
  uint8_t c_size_low;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved7: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}csd2_t;
//------------------------------------------------------------------------------
// union of old and new style CSD register
union csd_t {
  csd1_t v1;
  csd2_t v2;
};

//*******************************************************************************************************
// Archivo Sd2Card.h

// SPI pin definitions
//

//  The default chip select pin for the SD card is SS. 
//uint8_t const  SD_CHIP_SELECT_PIN = SS_PIN;
// The following three pins must not be redefined for hardware SPI.
//  SPI Master Out Slave In pin 
//uint8_t const  SPI_MOSI_PIN = MOSI_PIN;
//  SPI Master In Slave Out pin 
//uint8_t const  SPI_MISO_PIN = MISO_PIN;
//  SPI Clock pin 
//uint8_t const  SPI_SCK_PIN = SCK_PIN;
//  optimize loops for hardware SPI 



//------------------------------------------------------------------------------
//  Protect block zero from write if nonzero 
#define SD_PROTECT_BLOCK_ZERO 1
//  init timeout ms 
uint16_t const SD_INIT_TIMEOUT = 2000;
//  erase timeout ms 
uint16_t const SD_ERASE_TIMEOUT = 10000;
//  read timeout ms 
uint16_t const SD_READ_TIMEOUT = 300;
//  write time out ms 
uint16_t const SD_WRITE_TIMEOUT = 600;
//------------------------------------------------------------------------------
// SD card errors
//  timeout error for command SD_CMD0 
uint8_t const SD_CARD_ERROR_SD_CMD0 = 0X1;
//  CMD8 was not accepted - not a valid SD card
uint8_t const SD_CARD_ERROR_CMD8 = 0X2;
//  card returned an error response for CMD17 (read block) 
uint8_t const SD_CARD_ERROR_CMD17 = 0X3;
//  card returned an error response for CMD24 (write block) 
uint8_t const SD_CARD_ERROR_CMD24 = 0X4;
//   WRITE_MULTIPLE_BLOCKS command failed 
uint8_t const SD_CARD_ERROR_CMD25 = 0X05;
//  card returned an error response for CMD58 (read OCR) 
uint8_t const SD_CARD_ERROR_CMD58 = 0X06;
//  SET_WR_BLK_ERASE_COUNT failed 
uint8_t const SD_CARD_ERROR_ACMD23 = 0X07;
//  card's ACMD41 initialization process timeout 
uint8_t const SD_CARD_ERROR_ACMD41 = 0X08;
//  card returned a bad CSR version field 
uint8_t const SD_CARD_ERROR_BAD_CSD = 0X09;
//  erase block group command failed 
uint8_t const SD_CARD_ERROR_ERASE = 0X0A;
//  card not capable of single block erase 
uint8_t const SD_CARD_ERROR_ERASE_SINGLE_BLOCK = 0X0B;
//  Erase sequence timed out 
uint8_t const SD_CARD_ERROR_ERASE_TIMEOUT = 0X0C;
//  card returned an error token instead of read data 
uint8_t const SD_CARD_ERROR_READ = 0X0D;
//  read CID or CSD failed 
uint8_t const SD_CARD_ERROR_READ_REG = 0X0E;
//  timeout while waiting for start of read data 
uint8_t const SD_CARD_ERROR_READ_TIMEOUT = 0X0F;
//  card did not accept STOP_TRAN_TOKEN 
uint8_t const SD_CARD_ERROR_STOP_TRAN = 0X10;
//  card returned an error token as a response to a write operation 
uint8_t const SD_CARD_ERROR_WRITE = 0X11;
//  attempt to write protected block zero 
uint8_t const SD_CARD_ERROR_WRITE_BLOCK_ZERO = 0X12;
//  card did not go ready for a multiple block write 
uint8_t const SD_CARD_ERROR_WRITE_MULTIPLE = 0X13;
//  card returned an error to a CMD13 status check after a write 
uint8_t const SD_CARD_ERROR_WRITE_PROGRAMMING = 0X14;
//  timeout occurred during write programming 
uint8_t const SD_CARD_ERROR_WRITE_TIMEOUT = 0X15;
//  incorrect rate selected 
uint8_t const SD_CARD_ERROR_SCK_RATE = 0X16;
//------------------------------------------------------------------------------
// card types
//  Standard capacity V1 SD card 
uint8_t const SD_CARD_TYPE_SD1 = 1;
//  Standard capacity V2 SD card 
uint8_t const SD_CARD_TYPE_SD2 = 2;
//  High Capacity SD card 
uint8_t const SD_CARD_TYPE_SDHC = 3;
//------------------------------------------------------------------------------


  uint32_t block_=0;
  uint8_t chipSelectPin_=0;
  uint8_t errorCode_=0;
  uint8_t inBlock_=0;
  uint16_t offset_=0;
  uint8_t partialBlockRead_=0;
  uint8_t status_=0;
  uint8_t type_=0;
  uint8_t writeCRC_=0;



//*******************************************************************************************************
// Archivo Sd2Card.cpp


int8_t mosiPin_, misoPin_, clockPin_;
volatile RwRegmosiport,clkport,misoport;
uint32_t mosipinmask, clkpinmask, misopinmask;



//------------------------------------------------------------------------------
//  nop to tune soft SPI timing 
#define nop asm volatile ("nop\n\t")

void chipSelectHigh(void){
	GPIO_PORT->SET[0] |= (1 << 13);  // TXD0
}


void chipSelectLow(void){
	GPIO_PORT->CLR[0] |= (1 << 13);  // TXD0
}


/* 
// Read a cards CSD register. The CSD contains Card-Specific Data that
// provides information regarding access to the card's contents. 
uint8_t readCSD(void * csd){
    return readRegister(CMD9, csd);
}
*/  



// Receive a byte from the card 
uint8_t spiRec(void){
	uint8_t data=0;
	data= Chip_SSP_ReceiveFrame(LPC_SSP1); 
   return data;
}


// Soft SPI send 
void spiSend(uint8_t data) {
		//sprintf_mio(aux,"DATO 8bits: %X \n\r", data);
		//DEBUGSTR(aux);	
		//limpiaBufferAUX();
		//delay_rit_ms(200);
	Chip_SSP_SendFrame(LPC_SSP1, data);
}


//------------------------------------------------------------------------------
//  Skip remaining data in a block when in partial block read mode. 
void readEnd(void) {
  if (inBlock_) {
      // skip data and crc
    while (offset_++ < 514) spiRec();
    chipSelectHigh();
    inBlock_ = 0;
  }
}


// send command and return error code.  Return zero for OK
uint8_t cardCommand(uint8_t cmd, uint32_t arg) {
  // end read if in partialBlockRead mode

  readEnd();

  // select card
  chipSelectLow();

  // wait up to 300 ms if busy
  delay_rit_ms(300);

  // send command
  spiSend(cmd | 0x40);

  // send argument
  for (int8_t s = 24; s >= 0; s -= 8){ 
		spiSend(arg >> s);
	}

  // send CRC
  uint8_t crc = 0XFF;
  if (cmd == SD_CMD0) 
		crc = 0X95;  // correct crc for SD_CMD0 with arg 0
  if (cmd == CMD8) 
		crc = 0X87;  // correct crc for CMD8 with arg 0X1AA
  spiSend(crc);

  // wait for response
  while(((status_ = spiRec()) & 0x80) == 0x80){
		delay_rit_ms(10);
  //for (uint8_t i = 0; ((status_ = spiRec()) & 0X80) && i != 0XFF; i++);
	}
		sprintf_mio(aux,"Sale cardCommand: %X \n\r", status_);
		DEBUGSTR(aux);	
		limpiaBufferAUX();
		//
  return status_;

}



/*
//------------------------------------------------------------------------------
//  Determine if card supports single block erase.

// return The value one, true, is returned if single block erase is supported.
// The value zero, false, is returned if single block erase is not supported.
 
uint8_t eraseSingleBlockEnable(void){
	csd_t csd;
	return readCSD(&csd) ? csd.v1.erase_blk_en : 0;
}








//------------------------------------------------------------------------------
// 
// Enable or disable partial block reads.

// Enabling partial block reads improves performance by allowing a block
// to be read over the SPI bus as several sub-blocks.  Errors may occur
// if the time between reads is too long since the SD card may timeout.
// The SPI SS line will be held low until the entire block is read or
// readEnd() is called.

// Use this for applications like the Adafruit Wave Shield.

// param[in] value The value TRUE (non-zero) or FALSE (zero).)
 

void partialBlockRead(uint8_t value){
	readEnd();
   partialBlockRead_ = value;
}


void enableCRC(uint8_t mode) {
  writeCRC_ = mode;
}


// Read a 512 byte block from an SD card device.
// param[in] block Logical block to be read.
// param[out] dst Pointer to the location that will receive the data.
// return The value one, true, is returned for success and
// the value zero, false, is returned for failure.

uint8_t readBlock(uint32_t block, uint8_t* dst){
	return readData(block, 0, 512, dst);
}





//********************************************************************************************************
  
// class Sd2Card
// brief Raw access to SD and SDHC flash memory cards.

  // return error code for last error. See Sd2Card.h for a list of error codes.

  uint8_t errorCode(void){
		return errorCode_;
	}


  // \return error data for last error.
  uint8_t errorData(void){
		return status_;
	}


// Initialize an SD flash memory card with default clock rate and chip
// select pin.  See sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin).

  uint8_t init(void){
    return init(SPI_FULL_SPEED, SD_CHIP_SELECT_PIN);
  }


// Returns the current value, true or false, for partial block read.
  uint8_t partialBlockRead(void){
		return partialBlockRead_;
	}

*/
// Return the card type: SD V1, SD V2 or SDHC 
  uint8_t typeR(void){
		return type_;
	}

  // private functions
uint8_t cardAcmd(uint8_t cmd, uint32_t arg){
  cardCommand(CMD55, 0);
  return cardCommand(cmd, arg);
}

void error(uint8_t code){
	errorCode_ = code;
}


void type(uint8_t value){
	type_ = value;
}

/*




//  Read part of a 512 byte block from an SD card.
// param[in] block Logical block to be read.
// param[in] offset Number of bytes to skip at start of block
// param[out] dst Pointer to the location that will receive the data.
// param[in] count Number of bytes to read
// return The value one, true, is returned for success and
// the value zero, false, is returned for failure.


uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst){
  uint16_t n;
	if (count == 0) 
		return true;
	if((count + offset) > 512){
   	goto fail;
	}
	if (!inBlock_ || block != block_ || offset < offset_){
   	block_ = block;
    // use address if not SDHC card
		if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
		if (cardCommand(CMD17, block)) {
      	error(SD_CARD_ERROR_CMD17);
      	goto fail;
    	}
		if (!waitStartBlock()){
			goto fail;
		}
		offset_ = 0;
		inBlock_ = 1;
	}

  // skip data before offset
	for (;offset_ < offset; offset_++) {
		spiRec();
	}
  // transfer data
	for (uint16_t i = 0; i < count; i++) {
		dst[i] = spiRec();
	}
  offset_ += count;
  if (!partialBlockRead_ || offset_ >= 512) {
    // read rest of data, checksum and set chip select high
    readEnd();
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}



//------------------------------------------------------------------------------
//  read CID or CSR register 
uint8_t readRegister(uint8_t cmd, void* buf) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);
  if (cardCommand(cmd, 0)) {
    error(SD_CARD_ERROR_READ_REG);
    goto fail;
  }
  if (!waitStartBlock()) goto fail;
  // transfer data
  for (uint16_t i = 0; i < 16; i++) dst[i] = spiRec();
  spiRec();  // get first crc byte
  spiRec();  // get second crc byte
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}


  // 
// Read a cards CID register. The CID contains card identification
// information such as Manufacturer ID, Product name, Product serial
// number and Manufacturing date. 

  uint8_t readCID(cid_t* cid){
    return readRegister(CMD10, cid);
  }


//------------------------------------------------------------------------------
// 
// Set the SPI clock rate.
// param[in] sckRateID A value in the range [0, 6].
// The SPI clock will be set to F_CPU/pow(2, 1 + sckRateID). The maximum
// SPI rate is F_CPU/2 for \a sckRateID = 0 and the minimum rate is F_CPU/128
// for \a scsRateID = 6.
// return The value one, true, is returned for success and the value zero,
// false, is returned for an invalid value of \a sckRateID.

uint8_t setSckRate(uint8_t sckRateID) {
  if (sckRateID > 6) {
    error(SD_CARD_ERROR_SCK_RATE);
    return false;
  }
#ifndef USE_SPI_LIB
  // see avr processor datasheet for SPI register bit definitions
  if ((sckRateID & 1) || sckRateID == 6) {
    SPSR &= ~(1 << SPI2X);
  } else {
    SPSR |= (1 << SPI2X);
  }
  SPCR &= ~((1 <<SPR1) | (1 << SPR0));
  SPCR |= (sckRateID & 4 ? (1 << SPR1) : 0)
    | (sckRateID & 2 ? (1 << SPR0) : 0);
#else // USE_SPI_LIB
  int v;
#ifdef SPI_CLOCK_DIV128
  switch (sckRateID) {
    case 0: v=SPI_CLOCK_DIV2; break;
    case 1: v=SPI_CLOCK_DIV4; break;
    case 2: v=SPI_CLOCK_DIV8; break;
    case 3: v=SPI_CLOCK_DIV16; break;
    case 4: v=SPI_CLOCK_DIV32; break;
    case 5: v=SPI_CLOCK_DIV64; break;
    case 6: v=SPI_CLOCK_DIV128; break;
  }
#else // SPI_CLOCK_DIV128
  v = 2 << sckRateID;
#endif // SPI_CLOCK_DIV128
  SPI.setClockDivider(v);
#endif // USE_SPI_LIB
  return true;
}


//------------------------------------------------------------------------------
// wait for card to go not busy
uint8_t delay_rit_ms(uint16_t timeoutMillis) {
  uint16_t t0 = millis();
  do {
    if (spiRec() == 0XFF) return true;
  }
  while (((uint16_t)millis() - t0) < timeoutMillis);
  return false;
}
//------------------------------------------------------------------------------
//  Wait for start block token 
uint8_t waitStartBlock(void) {
  uint16_t t0 = millis();
  while ((status_ = spiRec()) == 0XFF) {
    if (((uint16_t)millis() - t0) > SD_READ_TIMEOUT) {
      error(SD_CARD_ERROR_READ_TIMEOUT);
      goto fail;
    }
  }
  if (status_ != DATA_START_BLOCK) {
    error(SD_CARD_ERROR_READ);
    goto fail;
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}


//------------------------------------------------------------------------------
// 
// Writes a 512 byte block to an SD card.

// param[in] blockNumber Logical block to be written.
// param[in] src Pointer to the location of the data to be written.
// return The value one, true, is returned for success and
// the value zero, false, is returned for failure.
 
uint8_t writeBlock(uint32_t blockNumber, const uint8_t* src) {
#if SD_PROTECT_BLOCK_ZERO
  // don't allow write to first block
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
#endif  // SD_PROTECT_BLOCK_ZERO

  // use address if not SDHC card
  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD24, blockNumber)) {
    error(SD_CARD_ERROR_CMD24);
    goto fail;
  }
  if (!writeData(DATA_START_BLOCK, src)) goto fail;

  // wait for flash programming to complete
  if (!delay_rit_ms(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_TIMEOUT);
    goto fail;
  }
  // response is r2 so get and check two bytes for nonzero
  if (cardCommand(CMD13, 0) || spiRec()) {
    error(SD_CARD_ERROR_WRITE_PROGRAMMING);
    goto fail;
  }
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}


//------------------------------------------------------------------------------
//  Write one data block in a multiple block write sequence 
uint8_t writeData(const uint8_t* src) {
  // wait for previous write to finish
  if (!delay_rit_ms(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_MULTIPLE);
    chipSelectHigh();
    return false;
  }
  return writeData(WRITE_MULTIPLE_TOKEN, src);
}


//------------------------------------------------------------------------------
// send one block of data for write block or write multiple blocks
uint8_t writeData(uint8_t token, const uint8_t* src) {
  
  // CRC16 checksum is supposed to be ignored in SPI mode (unless
  // explicitly enabled) and a dummy value is normally written.
  // A few funny cards (e.g. Eye-Fi X2) expect a valid CRC anyway.
  // Call setCRC(true) to enable CRC16 checksum on block writes.
  // This has a noticeable impact on write speed. :(
  int16_t crc;
  if(writeCRC_) {
    int16_t i, x;
    // CRC16 code via Scott Dattalo www.dattalo.com
    for(crc=i=0; i<512; i++) {
      x   = ((crc >> 8) ^ src[i]) & 0xff;
      x  ^= x >> 4;
      crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
    }
  } else {
    crc = 0xffff; // Dummy CRC value
  }

#ifdef OPTIMIZE_HARDWARE_SPI

  // send data - optimized loop
  SPDR = token;

  // send two byte per iteration
  for (uint16_t i = 0; i < 512; i += 2) {
    while (!(SPSR & (1 << SPIF)));
    SPDR = src[i];
    while (!(SPSR & (1 << SPIF)));
    SPDR = src[i+1];
  }

  // wait for last data byte
  while (!(SPSR & (1 << SPIF)));

#else  // OPTIMIZE_HARDWARE_SPI
  spiSend(token);
  for (uint16_t i = 0; i < 512; i++) {
    spiSend(src[i]);
  }
#endif  // OPTIMIZE_HARDWARE_SPI
  
  spiSend(crc >> 8); // Might be dummy value, that's OK
  spiSend(crc);

  status_ = spiRec();
  if ((status_ & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
    error(SD_CARD_ERROR_WRITE);
    chipSelectHigh();
    return false;
  }
  return true;
}


//------------------------------------------------------------------------------
//  Start a write multiple blocks sequence.
// param[in] blockNumber Address of first block in sequence.
// param[in] eraseCount The number of blocks to be pre-erased.
// note This function is used with writeData() and writeStop()
// for optimized multiple block writes.
// return The value one, true, is returned for success and
// the value zero, false, is returned for failure.
 
uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount) {
#if SD_PROTECT_BLOCK_ZERO
  // don't allow write to first block
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
#endif  // SD_PROTECT_BLOCK_ZERO
  // send pre-erase count
  if (cardAcmd(ACMD23, eraseCount)) {
    error(SD_CARD_ERROR_ACMD23);
    goto fail;
  }
  // use address if not SDHC card
  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD25, blockNumber)) {
    error(SD_CARD_ERROR_CMD25);
    goto fail;
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}


//------------------------------------------------------------------------------
//  End a write multiple blocks sequence.

// return The value one, true, is returned for success and
// the value zero, false, is returned for failure.
 
uint8_t writeStop(void) {
  if (!delay_rit_ms(SD_WRITE_TIMEOUT)) goto fail;
  spiSend(STOP_TRAN_TOKEN);
  if (!delay_rit_ms(SD_WRITE_TIMEOUT)) goto fail;
  chipSelectHigh();
  return true;

 fail:
  error(SD_CARD_ERROR_STOP_TRAN);
  chipSelectHigh();
  return false;
}



//***********************************************************************************************************************
*/




/*
 // Determine the size of an SD flash memory card.
 // return The number of 512 byte data blocks in the card or zero if an error occurs.

uint32_t cardSize(void){
  csd_t csd;
  if (!readCSD(&csd)) 
		return 0;
  if(csd.v1.csd_ver == 0){
  		uint8_t read_bl_len = csd.v1.read_bl_len;
    	uint16_t c_size = (csd.v1.c_size_high << 10) | (csd.v1.c_size_mid << 2) | csd.v1.c_size_low;
    	uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1) | csd.v1.c_size_mult_low;
    	return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
  }else if(csd.v2.csd_ver == 1){
    	uint32_t c_size = ((uint32_t)csd.v2.c_size_high << 16) | (csd.v2.c_size_mid << 8) | csd.v2.c_size_low;
    	return (c_size + 1) << 10;
  		}else{
    		error(SD_CARD_ERROR_BAD_CSD);
    		return 0;
  		}
}



//------------------------------------------------------------------------------
//  Erase a range of blocks.

// param[in] firstBlock The address of the first block in the range.
// param[in] lastBlock The address of the last block in the range.

// note This function requests the SD card to do a flash erase for a
// range of blocks.  The data on the card after an erase operation is
// either 0 or 1, depends on the card vendor.  The card must support
// single block erase.

// return The value one, true, is returned for success and
// the value zero, false, is returned for failure.
 
uint8_t erase(uint32_t firstBlock, uint32_t lastBlock){
	if (!eraseSingleBlockEnable()){
   	error(SD_CARD_ERROR_ERASE_SINGLE_BLOCK);
   	goto fail;
	}
	if (type_ != SD_CARD_TYPE_SDHC) {
   	firstBlock <<= 9;
   	lastBlock <<= 9;
	}
	if (cardCommand(CMD32, firstBlock) || cardCommand(CMD33, lastBlock) || cardCommand(CMD38, 0)){ 
   	error(SD_CARD_ERROR_ERASE);
      goto fail;
	}
	if (!delay_rit_ms(SD_ERASE_TIMEOUT)){
   	error(SD_CARD_ERROR_ERASE_TIMEOUT);
   	goto fail;
	}
  chipSelectHigh();
  return true;

	fail:
		chipSelectHigh();
		return false;
}
*/

//------------------------------------------------------------------------------
// 
// Initialize an SD flash memory card.

// param[in] sckRateID SPI clock rate selector. See setSckRate().
// param[in] chipSelectPin SD chip select pin number.

// return The value one, true, is returned for success and
// the value zero, false, is returned for failure.  The reason for failure
// can be determined by calling errorCode() and errorData().
 
bool init(){
  writeCRC_ = errorCode_ = inBlock_ = partialBlockRead_ = type_ = 0;

  // 16-bit init start time allows over a minute
//  uint16_t t0 = (uint16_t)millis();
  uint32_t arg;

  // set pin modes
  chipSelectHigh();
  
  // must supply min of 74 clock cycles with CS high.
  for (uint8_t i = 0; i < 10; i++) 
		spiSend(0XFF);

  chipSelectLow();


		sprintf_mio(aux,"PASO 1... \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();

  // command to go idle in SPI mode
  int timeOut=12;
  while (((status_ = cardCommand(SD_CMD0, 0)) != R1_IDLE_STATE ) && timeOut--){
//    if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT){
//      error(SD_CARD_ERROR_SD_CMD0);
//      goto fail;
//    }
	  delay_rit_ms(10);
  }

	if(timeOut){
		sprintf_mio(aux,"PASO 2... \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}
	else{
		sprintf_mio(aux,"TimeOut... \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
	}



  // check SD version
  if ((cardCommand(CMD8, 0x1AA) & SD_R1_ILLEGAL_COMMAND)){
    type(SD_CARD_TYPE_SD1);
		sprintf_mio(aux,"Card Version: SD1 \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();
  } 
	else {
    // only need last byte of r7 response
    for (uint8_t i = 0; i < 4; i++) 
		status_ = spiRec();

    if (status_ != 0XAA) {
      error(SD_CARD_ERROR_CMD8);
      goto fail;
    }
		sprintf_mio(aux,"Card Version: SD2 \n\r");
		DEBUGSTR(aux);	
		limpiaBufferAUX();

    type(SD_CARD_TYPE_SD2);
  }

  chipSelectHigh();
  return true;

  // initialize card and send host supports SDHC if SD2
  arg = typeR() == SD_CARD_TYPE_SD2 ? 0X40000000 : 0;

  while ((status_ = cardAcmd(ACMD41, arg)) != R1_READY_STATE){
    // check for timeout
//    if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT){
//      error(SD_CARD_ERROR_ACMD41);
//      goto fail;
//    }
  }

  // if SD2 read OCR register to check for SDHC card
  if (typeR() == SD_CARD_TYPE_SD2){
    if (cardCommand(CMD58, 0)){
      error(SD_CARD_ERROR_CMD58);
      goto fail;
    }
    if ((spiRec() & 0XC0) == 0XC0) type(SD_CARD_TYPE_SDHC);
    // discard rest of ocr - contains allowed voltage range
    for (uint8_t i = 0; i < 3; i++) spiRec();
  }
  chipSelectHigh();

  return true;

 fail:
  chipSelectHigh();
  return false;
}


