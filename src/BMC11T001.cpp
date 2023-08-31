/*****************************************************************
File:        BMC11T001.cpp
Author:      XIAO, BESTMODULES
Description: UART communication with the BMC11T001 and obtain the corresponding value  
History:         
V1.0.1   -- initial version；2023-08-22；Arduino IDE : v1.8.16
******************************************************************/
#include "BMC11T001.h"
/**********************************************************
Description: Constructor
Parameters:  *theSerial: Wire object if your board has more than one UART interface      
                         parameter range: &Serial4(fixed)
Return:          
Others:     
**********************************************************/
BMC11T001::BMC11T001(HardwareSerial *theSerial)
{
   _serial = theSerial;
}

/**********************************************************
Description: Shield Initial
Parameters:  baudRate: Set the Shield  baudRate(Unique Value 115200)      
Return:          
Others:         
**********************************************************/
void BMC11T001::begin(uint32_t baud)
{     
   _serial->begin(baud);
}

/**********************************************************
Description: ISO14443A Initial
Parameters:     
Return:          
Others:         
**********************************************************/
void BMC11T001::begin_ISO14443A()
{  
  uint8_t buff[10];
  uint8_t Config_String[] = "a -setup\r\n";
  writeBySerial(Config_String,sizeof(Config_String));  
  delay(70);
  getEvent(NFCA_Initial,buff); 
}

/**********************************************************
Description: Get ISO14443A UID
Parameters:  buff[] : Used to store ISO14443A UID         
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getUID_ISO14443A(uint8_t buff[])
{
  int Length; 
  uint8_t buffTemp[20];
  uint8_t GetUID_String[] = "a -getuid\r\n";
  writeBySerial(GetUID_String,sizeof(GetUID_String));
  delay(20);
  Length = getEvent(NFCA_ScanUID,buff); 
  return Length;
}

/**********************************************************
Description: Read Type 2 Tag Data
Parameters:  addr : EEPROM Address(00h ~ 2Ch) 
             buff[] : Used to store Type 2 Tag Data
Return:      Length : The length of the board reply string 
Others:         
**********************************************************/
uint8_t BMC11T001::readType2Tag_ISO14443A(uint8_t addr,uint8_t buff[])
{
  int Length; 
  uint8_t buffTemp[50];
  uint8_t Read_String[] = "t2t -rd 00\r\n";
  uint8_t Data[1];
  Data[0] = addr;
  byteToChar(&Data[0],&Read_String[8],&Read_String[9]);
  writeBySerial(Read_String, sizeof(Read_String));
  delay(40);
  Length = getEvent(NFCA_Type2Read,buff);
  return Length;
}

/**********************************************************
Description: Write Type 2 Tag Data
Parameters:  addr : EEPROM Address(04h ~ 27h) 
             data : Data written to EEPROM(1 byte)
Return:      0:Write Type 2 Tag Data succeed
             1:Write Type 2 Tag Data fail
Others:         
**********************************************************/
uint8_t BMC11T001::writeType2Tag_ISO14443A(uint8_t addr,long data)
{
  uint8_t buff[20];
  uint8_t  Write_String[]= "t2t -wr 00 12345678\r\n";;
  uint8_t  Data[4];
  Data[0] = addr;
  byteToChar(&Data[0],&Write_String[8],&Write_String[9]);
  Data[0] =(data & 0xFF000000) >> 24;
  Data[1] =(data & 0x00FF0000) >> 16;
  Data[2] =(data & 0x0000FF00) >> 8;
  Data[3] =(data & 0x000000FF);
  byteToChar(&Data[0],&Write_String[11],&Write_String[12]);
  byteToChar(&Data[1],&Write_String[13],&Write_String[14]);
  byteToChar(&Data[2],&Write_String[15],&Write_String[16]);
  byteToChar(&Data[3],&Write_String[17],&Write_String[18]);
  writeBySerial(Write_String,sizeof(Write_String));  
  delay(40);
  getEvent(NFCA_Type2Write,buff);
  if (buff[0] == 'R'&&buff[1] == 'e'&&buff[2] == 's')
 { 
  return 0;
  }
  else
 {
  return 1;
  }
  
}

/**********************************************************
Description: Read Mifare card EEPROM Data
Parameters:  *KeyType : only 2 types,"ka" or "kb" 
             *Key : 12 strings,defaut = "FFFFFFFFFFFF"
             addr : 
                   EEPROM Address(Mifare 1K):00h ~ 3Fh
                   EEPROM Address(Mifare 4K):00h ~ FFh
             buff[] : Used to store Mifare card EEPROM Data
Return:      Length : The length of the board reply string 
Others:         
**********************************************************/
uint8_t BMC11T001::readMifareBlock_ISO14443A(uint8_t keyType[],uint8_t key[],uint8_t addr,uint8_t buff[])
{
  int i;
  int Length; 
  uint8_t buffTemp[50];
  uint8_t ReadBlock_String[36] = "mifare -cread ka 123456789ABC 000\r\n";
  ReadBlock_String[14] = keyType[0];
  ReadBlock_String[15] = keyType[1];
  for(i = 0;i <= 11;i++)
  {
    ReadBlock_String[17 + i] = key[0 + i];
  }
  ReadBlock_String[30] = (addr/100%10) + '0';
  ReadBlock_String[31] = (addr/10%10) + '0';
  ReadBlock_String[32] = (addr/1%10) + '0' ;
  writeBySerial(ReadBlock_String,sizeof(ReadBlock_String));
  delay(40);
  Length = getEvent(Mifare_ReadBlock,buff);
  return  Length;
}

/**********************************************************
Description: Write Mifare card EEPROM Data
Parameters:  *KeyType : only 2 types,"ka" or "kb" 
             *Key : 12 strings,defaut = "FFFFFFFFFFFF"
             addr : 
                   EEPROM Address(Mifare 1K):01h ~ 3Fh
                   EEPROM Address(Mifare 4K):01h ~ FFh
             *data : Data written to EEPROM(4 bytes)
Return:      0:Write Type 2 Tag Data succeed
             1:Write Type 2 Tag Data fail
Others:         
**********************************************************/
uint8_t BMC11T001::writeMifareBlock_ISO14443A(uint8_t keyType[],uint8_t key[],uint8_t addr,uint8_t data[])
{
  uint8_t buff[20];
  uint8_t i;
  uint8_t Data[8];
  uint8_t WriteBlock_String[] = "mifare -cwrite ka 123456789ABC 000 00112233445566778899AABBCCDDEEFF\r\n";
  WriteBlock_String[15] = keyType[0];
  WriteBlock_String[16] = keyType[1];
  for(i = 0;i <= 11;i++)
  {
    WriteBlock_String[18 + i] = key[0 + i];
  }
  WriteBlock_String[31] = (addr/100%10) + '0';
  WriteBlock_String[32] = (addr/10%10) + '0';
  WriteBlock_String[33] = (addr/1%10) + '0';
  
  for(i = 0;i <= 31;i++)
  {
    WriteBlock_String[35 + i] = data[0 + i];
  }
  writeBySerial(WriteBlock_String,sizeof(WriteBlock_String));
  delay(40);
  getEvent(Mifare_WriteBlock,buff);
  if (buff[0] == 'O'&&buff[1] == 'K')
 { 
  return 0;
  }
  else
 {
  return 1;
  }
}

/**********************************************************
Description: ISO14443B Initial
Parameters:     
Return:          
Others:         
**********************************************************/
void BMC11T001::begin_ISO14443B(void)
{
  uint8_t buff[10];
  uint8_t Config_String[11] = "b -setup\r\n";
  writeBySerial(Config_String,10);
  delay(30);
  getEvent(NFCB_Initial,buff); 
}

/**********************************************************
Description: Get ISO14443B(standard calorie) UID
Parameters:  buff[] : Used to store ISO14443B(standard calorie) UID         
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getUID_ISO14443B(uint8_t buff[])
{
  int Length; 
  uint8_t GetUID_String[12] = "b -getuid\r\n";
  writeBySerial(GetUID_String,11);
  delay(15);
  Length = getEvent(NFCB_ScanUID,buff); 
  return Length;
}

/**********************************************************
Description: Initial ST25TB512 and get one random number from the card.
             And it convert byte of random number to 2 characters for output.
Parameters:  buff[]: Store 2 Characters of Random number   
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::initiate_ST25TB512_ISO14443B(uint8_t buff[])
{
  int Length; 
  uint8_t Initiate_String[21] = "b -trans -crc 0600\r\n";
  writeBySerial(Initiate_String,20);
  delay(15);
  Length = getEvent(NFCB_ST25TB512_Initiate,buff);
  return  Length;
}

/**********************************************************
Description: Select the ST25TB512 Tag Chip ID
Parameters:  Random[] : 2 Characters of Random number
                        (Obtain the value from the initiate_ST25TB512_ISO14443B () function)
             buff[]: Store 2 Characters of Random number    
Return:      Length : The length of the board reply string     
Others:         
**********************************************************/
uint8_t BMC11T001::selectChipID_ST25TB512_ISO14443B(uint8_t random[],uint8_t buff[])
{
  int Length; 
  uint8_t  SelectChipID_String[21] = "b -trans -crc 0E00\r\n";;
  SelectChipID_String[16] = random[10];
  SelectChipID_String[17] = random[11];
  writeBySerial(SelectChipID_String, 20);
  delay(15);
  Length = getEvent(NFCB_ST25TB512_SelectChipID,buff);
  return  Length;
}

/**********************************************************
Description: Get ST25TB512 UID
Parameters:  buff[] : Used to store ST25TB512 UID         
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getUID_ST25TB512_ISO14443B(uint8_t buff[])
{
  int Length; 
  uint8_t  GetUID_String[19] = "b -trans -crc 0B\r\n";
  writeBySerial(GetUID_String, 18);
  delay(15);
  Length = getEvent(NFCB_ST25TB512_GetUID,buff); 
  return Length ;
}

/**********************************************************
Description: Read ST25TB512 card EEPROM Data
Parameters:  addr : EEPROM Address:00h ~ 0Fh
             buff[] : Used to store ST25TB512 card EEPROM Data
Return:      Length : The length of the board reply string 
Others:         
**********************************************************/
uint8_t BMC11T001::readBlock_ST25TB512_ISO14443B(uint8_t addr,uint8_t buff[])
{
  int Length; 
  uint8_t  ReadBlock_String[21] = "b -trans -crc 0800\r\n";
  uint8_t Data[1];
  Data[0] = addr;
  byteToChar(&Data[0],&ReadBlock_String[16],&ReadBlock_String[17]);
  writeBySerial(ReadBlock_String, 20);
  delay(15);
  Length = getEvent(NFCB_ST25TB512_ReadBlock,buff);
  return Length;
}

/**********************************************************
Description: Write ST25TB512 card EEPROM Data
Parameters:  addr : EEPROM Address:07h ~ 0Fh
             *data : Data written to EEPROM(1 byte)
Return:      
Others:         
**********************************************************/
void BMC11T001::writeBlock_ST25TB512_ISO14443B(uint8_t addr,long data)
{ 
  uint8_t buff[20];
  uint8_t  WriteBlock_String[29] = "b -trans -crc 090012345678\r\n";
  uint8_t  Data[4];
  Data[0] = addr;
  byteToChar(&Data[0],&WriteBlock_String[16],&WriteBlock_String[17]);
  Data[0] =(data & 0xFF000000) >> 24;
  Data[1] =(data & 0x00FF0000) >> 16;
  Data[2] =(data & 0x0000FF00) >> 8;
  Data[3] =(data & 0x000000FF);
  
  byteToChar(&Data[0],&WriteBlock_String[18],&WriteBlock_String[19]);
  byteToChar(&Data[1],&WriteBlock_String[20],&WriteBlock_String[21]);
  byteToChar(&Data[2],&WriteBlock_String[22],&WriteBlock_String[23]);
  byteToChar(&Data[3],&WriteBlock_String[24],&WriteBlock_String[25]);
  writeBySerial(WriteBlock_String, 28);
  delay(15); 
  getEvent(NFCB_ST25TB512_WriteBlock,buff);
}

/**********************************************************
Description: Initial ISO15693
Parameters:     
Return:          
Others:         
**********************************************************/
void BMC11T001::begin_ISO15693(void)
{
  uint8_t buff[10];
  uint8_t Config_String[11] = "v -setup\r\n";
  writeBySerial(Config_String,sizeof(Config_String));
  delay(30);
  getEvent(NFCV_Initial,buff); 
}

/**********************************************************
Description: Get ISO15693 UID
Parameters:  buff[] : Used to store ISO15693 UID         
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getSingleUID_ISO15693(uint8_t buff[])
{
  int Length;
  uint8_t GetUID_String[10] = "v -inv1\r\n";
  writeBySerial(GetUID_String,sizeof(GetUID_String));
  delay(15);
  Length = getEvent(NFCV_ScanUID_TimeSlots_1,buff); 
  return Length;
}

/**********************************************************
Description: Get Multiple UID,up to 16
Parameters:  buff[] : Used to store Multiple UID       
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getMultipleUID_ISO15693(uint8_t buff[])
{
  int Length;
  uint8_t GetUID_String[11] = "v -inv16\r\n";
  writeBySerial(GetUID_String,sizeof(GetUID_String));
  delay(100);
  Length = getEvent(NFCV_ScanUID_TimeSlots_16,buff); 
  return Length;
}

/**********************************************************
Description: Read ISO15693 card EEPROM Data
Parameters:  addr : EEPROM Address:00h ~ 1Bh
             *UID : single UID
                   (Obtain the value from the getSingleUID_ISO15693()orgetMultipleUID_ISO15693() function)
             buff[] : Used to store ISO15693 card EEPROM Data
Return:      Length : The length of the board reply string 
Others:         
**********************************************************/
uint8_t BMC11T001::readBlock_ISO15693(uint8_t addr,uint8_t uid[],uint8_t buff[])
{
  int Length;
  uint8_t ReadBlock_String[28] = "v -rd 00 0123456789ABCDEF\r\n";
  uint8_t Data[8];
  uint8_t i = 0;
  Data[0] = addr;
  byteToChar(&Data[0],&ReadBlock_String[6],&ReadBlock_String[7]);
  for(i=0;i<=15;i++)
  {
    ReadBlock_String[9 + i] = uid[7 + i];
  }
  writeBySerial(ReadBlock_String,sizeof(ReadBlock_String));
  delay(15);
  Length = getEvent(NFCV_ReadBlock,buff);
  return Length;
}

/**********************************************************
Description: Write ISO15693 card EEPROM Data
Parameters:  addr : EEPROM Address:00h ~ 1Bh
             data : Data written to EEPROM(1 byte)
             UID : single UID
                   (Obtain the value from the getSingleUID_ISO15693()orgetMultipleUID_ISO15693() function)
Return:      0:Write ISO15693 card EEPROM Data succeed
             1:Write ISO15693 card EEPROM Data fail    
Others:     
**********************************************************/
uint8_t BMC11T001::writeBlock_ISO15693(uint8_t addr,long data,uint8_t uid[])
{
  uint8_t buff[40];
  uint8_t WriteBlock_String[37] = "v -wr 00 12345678 0123456789ABCDEF\r\n";
  uint8_t Data[8];
  uint8_t  i = 0;
  Data[0] = addr;
  byteToChar(&Data[0],&WriteBlock_String[6],&WriteBlock_String[7]);
  Data[0] =(data & 0xFF000000) >> 24;
  Data[1] =(data & 0x00FF0000) >> 16;
  Data[2] =(data & 0x0000FF00) >> 8;
  Data[3] =(data & 0x000000FF); 
  byteToChar(&Data[0],&WriteBlock_String[9],&WriteBlock_String[10]);
  byteToChar(&Data[1],&WriteBlock_String[11],&WriteBlock_String[12]);
  byteToChar(&Data[2],&WriteBlock_String[13],&WriteBlock_String[14]);
  byteToChar(&Data[3],&WriteBlock_String[15],&WriteBlock_String[16]);
  for(i=0;i<=15;i++)
  {
    WriteBlock_String[18 + i] = uid[7 + i];
  }
  writeBySerial(WriteBlock_String,sizeof(WriteBlock_String));
  delay(30);
  getEvent(NFCV_WriteBlock,buff);
  if (buff[10] == 'S'&&buff[11] == 'u'&&buff[12] == 'c')
 { 
  return 0;
  }
  else
 {
  return 1;
  }
}
  
/**********************************************************
Description: Get FW Version
Parameters:  buff[] : Used to store Firmware Version
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getFWVer_ADVANCED(uint8_t buff[])
{
  int Length; 
  uint8_t GetFWVer_String[] = "fw -ver\r\n";
  writeBySerial(GetFWVer_String,sizeof(GetFWVer_String));
  delay(5);
  Length = getEvent(Advanced_FWVersion,buff);
  return Length ;
}


/**********************************************************
Description: Turn off RF
Parameters:   
Return:
Others:            
**********************************************************/
void BMC11T001::turnoffRFField_ADVANCED(void)
{
  uint8_t buff[40];
  uint8_t Field_String[] = "rf -off\r\n";
  writeBySerial(Field_String,sizeof(Field_String));
  delay(5);
  getEvent(Advanced_Field_Off,buff);
}

/**********************************************************
Description: Reset RF
Parameters:   
Return:
Others:            
**********************************************************/
void BMC11T001::resetRFField_ADVANCED(void)
{
  uint8_t buff[40];
  uint8_t Field_String[] = "rf -reset\r\n";
  writeBySerial(Field_String,sizeof(Field_String));
  delay(10);
  getEvent(Advanced_Field_Reset,buff);
}

/**********************************************************
Description: Turn On buzzer 
Parameters:  
Return:                        
Others:            
**********************************************************/
void BMC11T001::turnOnBuzzer_ADVANCED(void)
{
  int Length; 
  uint8_t buff[20];
  uint8_t TurnOn_String[] = "buzz -turnon\r\n";
  writeBySerial(TurnOn_String,sizeof(TurnOn_String));
  delay(2);
  getEvent(Advanced_Buzzer_Turn_On,buff); 
}

/**********************************************************
Description: Turn Off buzzer 
Parameters:  
Return:                        
Others:            
**********************************************************/
void BMC11T001::turnOffBuzzer_ADVANCED(void)
{
  int Length; 
  uint8_t buff[20];
  uint8_t TurnOff_String[] = "buzz -turnoff\r\n";
  writeBySerial(TurnOff_String,sizeof(TurnOff_String));
  delay(2);
  getEvent(Advanced_Buzzer_Turn_Off,buff); 
}

/**********************************************************
Description: Get Single Card Detect Result
Parameters:  buff[] : Used to store Card Detect Result
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getSingleCardDetectResult_ADVANCED(void)
{
  uint8_t buff[10];
  int Length;
  uint8_t GetResult_String[] = "cdr -rr\r\n";
  writeBySerial(GetResult_String,sizeof(GetResult_String));
  delay(12);
  Length = getEvent(Advanced_Multi_Card_Detect_Result,buff); 
  if (buff[5] == 'I'&& buff[6] == 'n')
  { 
    return 1;
  }
  else
  {
    return 0;
  }
}

/**********************************************************
Description: Get Multi Card Detect Result
Parameters:  buff[] : Used to store Card Detect Result
Return:      Length : The length of the board reply string    
Others:         
**********************************************************/
uint8_t BMC11T001::getMultiCardDetectResult_ADVANCED(void)
{
  int Length;
  uint8_t buff[20];
  uint8_t GetResult_String[] = "cdr -mrr\r\n";
  writeBySerial(GetResult_String,sizeof(GetResult_String));
  delay(80);
  Length = getEvent(Advanced_Single_Card_Detect_Result,buff); 
  if (buff[5] == 'I'&& buff[6] == 'n')
  { 
    return 1;
  }
  else
  {
    return 0;
  }
}
/**********************************************************
Description: In CD mode, select MCU working mode 
Parameters:  powermode : Sleep/DEEP_SLEEP/Normal
Return:                           
Others:            
**********************************************************/
void BMC11T001::setMCUPowerMode_ADVANCED(uint8_t powermode)
{
  uint8_t Enter[] = "\r\n"; 
  uint8_t buff[40];
  uint8_t Powemode_String1[] = "cd -lpmcu -sleep1\r\n";
  uint8_t Powemode_String2[] = "cd -lpmcu -sleep2\r\n";
  uint8_t Powemode_String3[] = "cd -lpmcu -normal\r\n";
  switch(powermode)
  {
    case SLEEP:
         writeBySerial(Powemode_String1,sizeof(Powemode_String1));
    break;
    
    case DEEP_SLEEP:
         writeBySerial(Powemode_String2,sizeof(Powemode_String2));
    break;

    case NORMAL:
         writeBySerial(Powemode_String3,sizeof(Powemode_String3));
    break;
  }
  delay(10);
  getEvent(Advanced_MCU_Power_Mode,buff);
}

/**********************************************************
Description: In CD mode, select MCU wakeup time  
Parameters:  wakeuptime : Period_100ms/Period_200ms/Period_500ms/Period_1000ms
Return:                           
Others:            
**********************************************************/
void BMC11T001::setMCUWakeupTime_ADVANCED(uint8_t wakeuptime)
{
  uint8_t Enter[] = "\r\n"; 
  uint8_t buff[40];
  uint8_t Wakeuptime_String1[] = "cd -wkuptime 100\r\n";
  uint8_t Wakeuptime_String2[] = "cd -wkuptime 200\r\n";
  uint8_t Wakeuptime_String3[] = "cd -wkuptime 500\r\n";
  uint8_t Wakeuptime_String4[] = "cd -wkuptime 1000\r\n";
  switch(wakeuptime)
  {
    case PERIOD_100MS:
         writeBySerial(Wakeuptime_String1,sizeof(Wakeuptime_String1));
    break;
    
    case PERIOD_200MS:
         writeBySerial(Wakeuptime_String2,sizeof(Wakeuptime_String2));
    break;

    case PERIOD_500MS:
         writeBySerial(Wakeuptime_String3,sizeof(Wakeuptime_String3));
    break;
    
    case PERIOD_1000MS:
         writeBySerial(Wakeuptime_String4,sizeof(Wakeuptime_String4));
    break;
  }
  delay(10); 
  getEvent(Advanced_MCU_Wakeup_Time,buff);
}

/**********************************************************
Description: RF Calibration before Card Detect Mode 
Parameters:  
Return:                           
Others:            
**********************************************************/
void BMC11T001::setCDCalibration_ADVANCED(void)
{
  uint8_t Enter[] = "\r\n"; 
  uint8_t buff[40];
  uint8_t Calibration_String[] = "cd -cal\r\n";
  writeBySerial(Calibration_String,sizeof(Calibration_String));
  delay(2000); 
  getEvent(Advanced_CD_Calibration,buff);
}

/**********************************************************
Description: Start Card Detect Mode
Parameters:  CardType : 0x01 = NFC-A 
                              0x02 = NFC-B
                              0x03 = NFC-A & NFC-B
                              0x04 = NFC-V
                              0x05 = NFC-A & NFC-V
                              0x06 = NFC-B & NFC-V
                              0x07 = NFC-A & NFC-B & NFC-V
Return:                           
Others:            
**********************************************************/
void BMC11T001::startLowPowerCardDetection_ADVANCED(uint8_t cardType)
{
  uint8_t NFC_Event_Length = 0;
  uint8_t buff[40];
  uint8_t Enter[] = "\r\n"; 
  uint8_t CD_String[] = "scan -start 00\r\n";
  uint8_t Data[1];
  Data[0] = cardType;
  byteToChar(&Data[0],&CD_String[12],&CD_String[13]);
  writeBySerial(CD_String,sizeof(CD_String));
  while(NFC_Event_Length == 0)
  {
   NFC_Event_Length = getEvent(Advanced_LOW_Power_Card_Detection_Start,buff); 
  } 
}

/**********************************************************
Description: Scaning Tag in Card Detect Mode
Parameters:  buff[]: Used to Store the UID&MCU working mode string returned by the expansion board 
Return:      Length : The length of the board reply string                   
Others:            
**********************************************************/
uint8_t BMC11T001::scaningLowPowerCardDetection_ADVANCED(uint8_t buff[])
{
   uint8_t Length = 0;
   while(Length == 0)
  {
   Length = getEvent(Advanced_LOW_Power_Card_Detection_Scaning,buff); 
  }
  return Length;
}

/**********************************************************
Description: Stop Card Detect Mode
Parameters:  
Return:                        
Others:            
**********************************************************/
void BMC11T001::stopLowPowerCardDetection_ADVANCED(void)
{
  uint8_t buff[40];
  uint8_t Enter[] = "\r\n"; 
  uint8_t CD_String[] = "s\r\n";
  writeBySerial(CD_String,sizeof(CD_String));
  delay(10);
  getEvent(Advanced_LOW_Power_Card_Detection_Stop,buff);
}


/**********************************************************
Description: NFC_SerialGetData
Parameters:  command: The character corresponding to the previous instruction
             *event:  The string that stores the reply from the expansion board
Return:      The length of the board reply string                    
Others:            
**********************************************************/
int BMC11T001::getEvent(uint8_t command,uint8_t *event) 
{  
  int  i = 0;
  int  j = 0;
  int  count = 0;
  int  count1 = 0;
  int  n = 0;
  uint8_t  NFC_inChar = 0; 
  uint8_t  temp[255];
  uint8_t  Array[4096];
  bool     IsDataAvailable = false;
  bool     CD_Calibration_over = false;
  switch(command)
  {
      case NFCA_Initial:
           n = 2;
      break;
    
      case NFCA_ScanUID:
           n = 3;
      break;

      case NFCA_Type2Read:
           n = 4;
      break;

      case NFCA_Type2Write:
           n = 4;
      break;

      case Mifare_ReadBlock:
           n = 5;
      break;

      case Mifare_WriteBlock:
           n = 5;
      break;

      case NFCB_Initial:
           n = 2;
      break; 
       
      case NFCB_ScanUID:
           n = 3;
      break;  

      case NFCB_ST25TB512_Initiate:
           n = 4;
      break;
      
      case NFCB_ST25TB512_SelectChipID:
           n = 4;
      break;

      case NFCB_ST25TB512_GetUID:
           n = 4;
      break;
      
      case NFCB_ST25TB512_ReadBlock:
           n = 4;
      break;

      case NFCB_ST25TB512_WriteBlock:
           n = 3;
      break;

      case NFCV_Initial:
           n = 2;
      break;
      
      case NFCV_ScanUID_TimeSlots_1:
           n = 3;
      break;

      case NFCV_ReadBlock:
           n = 3;
      break;

      case NFCV_WriteBlock:
           n = 3;
      break;
      
      case Advanced_Read_Register:
           n = 4;
      break;
      
      case Advanced_Write_Register :
           n = 4;
      break;

      case Advanced_Write_DefaultValue_Register :
           n = 3;
      break;
      
      case Advanced_Field_Off:
           n = 2;
      break;

      case Advanced_Field_Reset:
           n = 2;
      break;
      
      case Advanced_MCU_Power_Mode:
           n = 3;
      break;

      case Advanced_MCU_Wakeup_Time:
           n = 3;
      break;

      case Advanced_LOW_Power_Card_Detection_Start:
           n = 4;
      break;

      case Advanced_LOW_Power_Card_Detection_Scaning:
           n = 1;
      break;

      case Advanced_LOW_Power_Card_Detection_Stop:
           n = 1;
      break;

      case Advanced_FWVersion:
           n = 3;
      break;

      case Advanced_Buzzer_Turn_On:
           n = 2;
      break;

      case Advanced_Buzzer_Turn_Off:
           n = 2;
      break;

      case Advanced_Single_Card_Detect_Result:
           n = 3;
      break;

      case Advanced_Multi_Card_Detect_Result:
           n = 3;
      break;
  } 
  
  if(_softSerial != NULL)
  {
     while(_softSerial->available())
     {    
      NFC_inChar = (char)_softSerial->read();
      NFC_ReturnEvent[Event_Length]= NFC_inChar;
      Event_Length++;    
      if(NFC_inChar=='\n')
      {
       n_count++; 
       switch(command)
       {
        case NFCV_ScanUID_TimeSlots_16 :
             if(Is_same(&NFC_ReturnEvent[Event_Length - 4],&CompletedEvent[0],2))
             { 
              Number_Cards = (int)(NFC_ReturnEvent[Event_Length-8]-'0')*10 +(int)(NFC_ReturnEvent[Event_Length-7]-'0');
              n = n_count;
             }     
        break;

        case Mifare_ReadBlock :
             if(n_count == 2 && Is_same(&NFC_ReturnEvent[Event_Length - 7],&ErrorEvent8[0],7))
             {
              NFC_Error = true;
              n = n_count;
             } 
        break;

        case Mifare_WriteBlock :
             if(n_count == 4 && Is_same(&NFC_ReturnEvent[Event_Length - 7],&ErrorEvent8[0],7))
             {
              NFC_Error = true;
              n = n_count;
             } 
        break; 

        case Advanced_CD_Calibration :
             if(Is_same(&NFC_ReturnEvent[Event_Length - 4],&CompletedEvent[0],2))
             {
              n = n_count;       
             }      
        break;

        case Advanced_Read_All_Register :
             if(Is_same(&NFC_ReturnEvent[Event_Length - 4],&CompletedEvent[0],2))
             {
              OK_count++;     
              if(OK_count == 2)
              {    
                n = n_count; 
                OK_count = 0;    
              }  
             }   
        break;   
       }
     
       if(n_count==2)
       {
        i = 0;
        count = 0;
        while(count!=1)
        {
         if(NFC_ReturnEvent[i] == '\n') count++;
         i++;
        }
        j = 0; 
        while(NFC_ReturnEvent[i+j]!='\r')
        {
         temp[j] = NFC_ReturnEvent[i+j];
         j++;
        }
        NFC_Error = ReceiveEvent_ErrorCheck(temp,j);
       }
     
       if(NFC_Error && command != Mifare_ReadBlock && command != Mifare_WriteBlock)
       { 
        n = 3;
       }
     
       if(n_count == n)
       {
        IsDataAvailable = true;   
        i= 0;
        while(i!= Event_Length)
        {
         Array[i] = NFC_ReturnEvent[i];
         i++; 
        } 
       }
      }
     }
  }
  else
  {
     while(_serial->available())
     {    
      NFC_inChar = (char)_serial->read();
      NFC_ReturnEvent[Event_Length]= NFC_inChar;
      Event_Length++;    
      if(NFC_inChar=='\n')
      {
       n_count++; 
       switch(command)
       {
        case NFCV_ScanUID_TimeSlots_16 :
             if(Is_same(&NFC_ReturnEvent[Event_Length - 4],&CompletedEvent[0],2))
             { 
              Number_Cards = (int)(NFC_ReturnEvent[Event_Length-8]-'0')*10 +(int)(NFC_ReturnEvent[Event_Length-7]-'0');
              n = n_count;
             }     
        break;

        case Mifare_ReadBlock :
             if(n_count == 3 && Is_same(&NFC_ReturnEvent[Event_Length - 7],&ErrorEvent8[0],7))
             {
              NFC_Error = true;
              n = n_count;
             } 
        break;

        case Mifare_WriteBlock :
             if(n_count == 4 && Is_same(&NFC_ReturnEvent[Event_Length - 7],&ErrorEvent8[0],7))
             {
              NFC_Error = true;
              n = n_count;
             } 
        break; 

        case Advanced_CD_Calibration :
             if(Is_same(&NFC_ReturnEvent[Event_Length - 4],&CompletedEvent[0],2))
             {
              n = n_count;       
             }      
        break;

        case Advanced_Read_All_Register :
             if(Is_same(&NFC_ReturnEvent[Event_Length - 4],&CompletedEvent[0],2))
             {
              OK_count++;     
              if(OK_count == 2)
              {    
                n = n_count; 
                OK_count = 0;    
              }  
             }   
        break;   
       }
     
       if(n_count==2)
       {
        i = 0;
        count = 0;
        while(count!=1)
        {
         if(NFC_ReturnEvent[i] == '\n') count++;
         i++;
        }
        j = 0; 
        while(NFC_ReturnEvent[i+j]!='\r')
        {
         temp[j] = NFC_ReturnEvent[i+j];
         j++;
        }
        NFC_Error = ReceiveEvent_ErrorCheck(temp,j);
       }
     
       if(NFC_Error && command != Mifare_ReadBlock && command != Mifare_WriteBlock)
       { 
        n = 3;
       }
     
       if(n_count == n)
       {
        IsDataAvailable = true;   
        i= 0;
        while(i!= Event_Length)
        {
         Array[i] = NFC_ReturnEvent[i];
         i++; 
        } 
       }
      }
     }
  }



  
  if(IsDataAvailable)
  {
      Event_Length = 0;
      n_count = 0;
      i  = 0;
      memset(NFC_ReturnEvent,'\0',sizeof(NFC_ReturnEvent));
  }
  
  if(IsDataAvailable)
  {
      Event_Length = 0;
      n_count = 0;
      i  = 0;
      OK_count = 0;
      count = 0;
        
      memset(NFC_ReturnEvent,'\0',sizeof(NFC_ReturnEvent));    
      //return  IsDataAvailable; 
      switch(command)
     {
      case NFCA_Initial:
           n = 2;
      break;
    
      case NFCA_ScanUID:
           n = 2;
      break;

      case NFCA_Type2Read:
           n = 4;
      break;

      case NFCA_Type2Write:
           n = 4;
      break;

      case Mifare_ReadBlock:
           if(NFC_Error)
           {
            n = 3;
           }
           else
           {
            n = 4;
           }
      break;

      case Mifare_WriteBlock:
           if(NFC_Error)
           {
            n = 4;
           }
           else
           {
            n = 5;
           }
      break;

      case NFCB_Initial:
           n = 2;
      break; 
       
      case NFCB_ScanUID:
           n = 2;
      break; 

      case NFCB_ST25TB512_Initiate:
           n = 3;
      break;
      
      case NFCB_ST25TB512_SelectChipID:
           n = 3;
      break;

      case NFCB_ST25TB512_GetUID:
           n = 3;
      break;
      
      case NFCB_ST25TB512_ReadBlock:
           n = 3;
      break;

      case NFCB_ST25TB512_WriteBlock:
           n = 2;
      break;  

      case NFCV_Initial:
           n = 2;
      break;
      
      case NFCV_ScanUID_TimeSlots_1:
           n = 2;
      break;
    
      case NFCV_ScanUID_TimeSlots_16:
           n = Number_Cards; 
      break;
    
      case NFCV_ReadBlock:
           n = 3;
      break;

      case NFCV_WriteBlock:
           n = 2;
      break;
        
      case Advanced_Write_Register :
           n = 3;
      break;
    
      case Advanced_Write_DefaultValue_Register :
           n = 3;
      break;
    
      case Advanced_Field_Off:
           n = 2;
      break;

      case Advanced_Field_Reset:
           n = 2;
      break;
  
      case Advanced_MCU_Power_Mode:
           n = 2;
      break; 
    
      case Advanced_MCU_Wakeup_Time:
           n = 2;
      break;   

      case Advanced_LOW_Power_Card_Detection_Start:
           n = 2;
      break;

      case Advanced_LOW_Power_Card_Detection_Scaning:
           n = 1;
      break; 
      
      case Advanced_LOW_Power_Card_Detection_Stop:
           n = 1;
      break;

      case Advanced_FWVersion:
           n = 2;
      break;

      case Advanced_Buzzer_Turn_On:
           n = 2;
      break;

      case Advanced_Buzzer_Turn_Off:
           n = 2;
      break;

      case Advanced_Single_Card_Detect_Result:
           n = 2;
      break;

      case Advanced_Multi_Card_Detect_Result:
           n = 2;
      break;
      
     } 
     while(count!=1)
     {
      if(Array[i] == '\n') count++;
      i++;
     }
     j = 0; 
     while(Array[i+j]!='\r')
     {
      event[j]=Array[i+j];
      j++;
     }  
     if(NFC_Error && command != Mifare_ReadBlock && command != Mifare_WriteBlock)
     {
      NFC_Error = false;
      return j;
     } 
     else
     { 
       switch(command)
       {
        case NFCV_ScanUID_TimeSlots_16 :           
           count = 0;
           i = 0;
           do
           {
            if(Array[i] == '\n')
            {
              if(Is_same(&Array[i-4],&CompletedEvent[0],2))
              {
               n = n_count;
              } 
              count++;
            }
            i++;
           }while(count != 2);         
           count =0;
           j = 0;   
           do
           {
              event[j]=Array[i+j];
              j++;
              if(Array[i+j] == '\n')
              {
               count++;
              }
           }while(count != (Number_Cards + 1));
           return j; 
      break;

      case Advanced_CD_Calibration : 
           i=0;
           do
           {
            if(Array[i] == '\n')
            {
              if(Is_same(&Array[i-3],&CompletedEvent[0],2))
              {   
               CD_Calibration_over = true;
              } 
              count++;
            }
            i++;
           }while(CD_Calibration_over!=true);
           CD_Calibration_over = false;      
           count =0;
           j = 0;   
           do
           {
              event[j]=Array[i-4+j];
              j++;
           }while(j!= 2);
           return j;         
      break;

      case Advanced_LOW_Power_Card_Detection_Scaning :
           i = 0;
           count = 0;
           do
           {
            event[i] = Array[i];
            if(Array[i] == '\n')
            {
              count++;
            }
            i++;
           }while(count!=1);
           i = i -2;
           return i;                          
      break;

      case Advanced_LOW_Power_Card_Detection_Stop :
           i = 0;
           count = 0;
           do
           {
            event[i] = Array[i];
            if(Array[i] == '\n')
            {
              count++;
            }
            i++;
           }while(count!=1);
           i = i -2;
           return i;                          
      break;

      case Advanced_Read_Register :
           count = 0;
           i = 0;
           do
           {
            if(Array[i] == '\n')
            {
              count++;
            }
            i++;
           }while(count != 2);         
           count =0;
           j = 0;   
           do
           {
              event[j]=Array[i+j];
              j++;
              if(Array[i+j] == '\n')
              {
               count++;
              }
           }while(count != 1);
           return j; 
      
      break;

      case Advanced_Read_All_Register:
           count = 0;
           i = 0;
           do
           {
            if(Array[i] == '\n')
            {
              count++;
            }
            i++;
           }while(count != 1);         
           count =0;
           j = 0;   
           do
           {
              event[j]=Array[i+j];
              j++;
              if(Array[i+j] == '\n')
              {
               count++;
              }
           }while(count != 81);
           j = j-1;
           return j;  
      break;

      case Advanced_Write_Register :
           count = 0;
           i = 0;
           do
           {
            if(Array[i] == '\n')
            {
              count++;
            }
            i++;
           }while(count != 3);         
           count =0;
           j = 0;   
           do
           {
              event[j]=Array[i+j];
              j++;
              if(Array[i+j] == '\n')
              {
               count++;
              }
           }while(count != 1);
           return j; 
      
      break;

      
      default :          
           while(count!=n-1)
           {
             if(Array[i] == '\n') count++;
             i++;
           }
           j = 0; 
           while(Array[i+j]!='\r')
           {
            event[j]=Array[i+j];
            j++;
           }  
           return j;        
      break;
    }      
  }
 }
}

/**********************************************************
Description: ReceiveEvent_ErrorCheck
Parameters:  *event: Event array
             *len: Event array legth
Return:      true: Is it an error event
             false: Is not an error event                  
Others:            
**********************************************************/
bool BMC11T001::ReceiveEvent_ErrorCheck(uint8_t *event, uint8_t len)
{
  if(Is_same(&*event,&ErrorEvent1[0],len) == true)return true;
  if(Is_same(&*event,&ErrorEvent2[0],len) == true)return true;
  if(Is_same(&*event,&ErrorEvent3[0],len) == true)return true;
  if(Is_same(&*event,&ErrorEvent4[0],len) == true)return true;
  if(Is_same(&*event,&ErrorEvent5[0],len) == true)return true;
  if(Is_same(&*event,&ErrorEvent6[0],len) == true)return true;
  if(Is_same(&*event,&ErrorEvent7[0],len) == true)return true;     
  return false;
}

/**********************************************************
Description: Write data through automatic serial port selection
Parameters:  *buff : Variables for storing buff data
             len   : buff length    
Return:          
Others:         
**********************************************************/
void BMC11T001::writeBySerial(uint8_t *buff,uint16_t len)
{
   _serial->write(buff,len); 
}

/*******************************************************************************
* Function Name  : Is_same
* Description    : Compare whether the two arrays are the same
* Input          : An array,
                   Another array
* Output         : None
* Return         : Result of judgment
                   -true
                   -false
*******************************************************************************/
bool BMC11T001::Is_same(uint8_t *saddr,const uint8_t *taddr,uint8_t len)
{  
  while(len)
  {
    if(*saddr != *taddr)  return false;
    saddr++;
    taddr++;
    len--;
  }
  return true;
}

/**********************************************************
Description: byteToChar
Parameters:  *inputByte: Convert byte
             *outputChar1 : The converted character         
             *outputChar2 : The converted character
Return:                       
Others:            
**********************************************************/
void BMC11T001::byteToChar(uint8_t *inputByte, uint8_t *outputChar1,uint8_t *outputChar2)
{
    char temp_dataH;
    char temp_dataL;

    /*Seperate 4 bits of data (first 4 bits and last 4 bits)*/
    temp_dataH = (*(inputByte) & 0xF0) >> 4;
    temp_dataL = (*(inputByte) & 0x0F);

    /*Check 4 bits high*/
    if(temp_dataH <= 0x09)
    {
        temp_dataH = temp_dataH + '0';
    }
    else
    {
        temp_dataH = (temp_dataH - 0x0A) + 'A';
    }

    /*Check 4 bits low*/
    if(temp_dataL <= 0x09)
    {
        temp_dataL = temp_dataL + '0';
    }
    else
    {
        temp_dataL = (temp_dataL - 0x0A) + 'A';
    }

    /*Assign to output buffer */
    *outputChar1 = temp_dataH;
    *outputChar2 = temp_dataL;
}
