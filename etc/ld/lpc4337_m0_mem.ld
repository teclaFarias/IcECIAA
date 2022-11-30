/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2015
 * (c) NXP Semiconductors 2013-2015
 * Linker script memory definitions
 * Created from LinkMemoryTemplate
 * By LPCXpresso v7.6.2 [Build 326] [2015-02-02]  on Wed Aug 26 15:59:13 ART 2015)
*/

MEMORY
{
  /* Define each memory region */
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes */
  RamAHB_ETB16 (rx)  : ORIGIN = 0x20002000, LENGTH = 0x1000    /*  ORIGIN = 0x2000c000, LENGTH = 0x00400  */
  Misvariables (rwx) : ORIGIN = 0x20003000, LENGTH = 0x8000      /* 32KB   ORIGIN = 0x2200c400, LENGTH = 0x3C00     //13K bytes */


}
  /* Define a symbol for the top of each memory region */
  __top_MFlashB512 = 0x1b000000 + 0x80000;
  __top_RamAHB_ETB16 = 0x20002000 + 0x1000;
  __top_Misvariables = 0x20003000 +  0x8000; 
