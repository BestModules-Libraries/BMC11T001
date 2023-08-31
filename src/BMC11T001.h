/*****************************************************************
File:             BMC11T001.h
Author:           XIAO, BESTMODULES
Description:      Define classes and required variables
History：         
V1.0.1   -- initial version；2023-08-22；Arduino IDE : v1.8.16
******************************************************************/
#ifndef _BMC11T001_
#define _BMC11T001_

#include <Arduino.h>
#include <SoftwareSerial.h>

#define   BMC11T001_BAUD  115200

const uint8_t ErrorEvent1[25] = "ERROR: Unknown command\r\n";
const uint8_t ErrorEvent2[19] = "Invalid argument\r\n";
const uint8_t ErrorEvent3[23] = "Unavailable argument\r\n";
const uint8_t ErrorEvent4[14] = "No Response\r\n";
const uint8_t ErrorEvent5[10] = "Timeout\r\n";
const uint8_t ErrorEvent6[19] = "Operation Failed\r\n";
const uint8_t ErrorEvent7[16] = "Receive Error\r\n";
const uint8_t ErrorEvent8[8]  = "ERROR\r\n";
const uint8_t CompletedEvent[3] = "OK";
const uint8_t MCUPowerModeEvent1[19] = "Deep Sleep1 Mode\r\n";
const uint8_t MCUPowerModeEvent2[19] = "Deep Sleep2 Mode\r\n";
const uint8_t MCUPowerModeEvent3[14] = "Normal Mode\r\n"; 

class BMC11T001
{
      public:
            BMC11T001(HardwareSerial *theSerial  = &Serial4);
            void begin(uint32_t baud = BMC11T001_BAUD);    
            void begin_ISO14443A();
            uint8_t getUID_ISO14443A(uint8_t buff[]);
            uint8_t readType2Tag_ISO14443A(uint8_t addr,uint8_t buff[]);
            uint8_t writeType2Tag_ISO14443A(uint8_t addr,long data);
            uint8_t readMifareBlock_ISO14443A(uint8_t keyType[],uint8_t key[],uint8_t addr,uint8_t buff[]);
            uint8_t writeMifareBlock_ISO14443A(uint8_t keyType[],uint8_t key[],uint8_t addr,uint8_t data[]); 

            void begin_ISO14443B(void);
            uint8_t getUID_ISO14443B(uint8_t buff[]);
            uint8_t initiate_ST25TB512_ISO14443B(uint8_t buff[]);
            uint8_t selectChipID_ST25TB512_ISO14443B(uint8_t random[],uint8_t buff[]);
            uint8_t getUID_ST25TB512_ISO14443B(uint8_t buff[]);
            uint8_t readBlock_ST25TB512_ISO14443B(uint8_t addr,uint8_t buff[]);
            void writeBlock_ST25TB512_ISO14443B(uint8_t addr,long data);

            void begin_ISO15693(void);
            uint8_t getSingleUID_ISO15693(uint8_t buff[]);   
            uint8_t getMultipleUID_ISO15693(uint8_t buff[]);
            uint8_t readBlock_ISO15693(uint8_t addr,uint8_t uid[],uint8_t buff[]);
            uint8_t writeBlock_ISO15693(uint8_t addr,long data,uint8_t uid[]);

            uint8_t getFWVer_ADVANCED(uint8_t buff[]);
            void turnoffRFField_ADVANCED(void);
            void resetRFField_ADVANCED(void);
            void turnOnBuzzer_ADVANCED(void);
            void turnOffBuzzer_ADVANCED(void);
            uint8_t getSingleCardDetectResult_ADVANCED(void);
            uint8_t getMultiCardDetectResult_ADVANCED(void);
            void setMCUPowerMode_ADVANCED(uint8_t powermode);
            void setMCUWakeupTime_ADVANCED(uint8_t wakeuptime);
            void setCDCalibration_ADVANCED(void);
            void startLowPowerCardDetection_ADVANCED(uint8_t cardType);
            uint8_t scaningLowPowerCardDetection_ADVANCED(uint8_t buff[]);
            void stopLowPowerCardDetection_ADVANCED(void);
  
      private:
            int getEvent(uint8_t command,uint8_t *event); 
            void writeBySerial(uint8_t *buff,uint16_t len);
            bool Is_same(uint8_t *saddr,const uint8_t *taddr,uint8_t len);
            bool ReceiveEvent_ErrorCheck(uint8_t *event, uint8_t len);
            void byteToChar(uint8_t *inputByte, uint8_t *outputChar1,uint8_t *outputChar2);
            
            uint16_t _rxPin;
            uint16_t _txPin;
            uint8_t  NFC_ReturnEvent[4096];
            uint8_t  n_count = 0 ;
            uint8_t  OK_count = 0;
            int  Event_Length = 0;
            int  Number_Cards = 0;    
            bool NFC_Error = false;    
            HardwareSerial *_serial = NULL;
            SoftwareSerial *_softSerial = NULL ;                                                         
};

/*******************************************************************************
* enum     Command TYPE
* brief    Command TYPE
*******************************************************************************/
enum
{
  NFCA_Initial = 0x10,
  NFCA_ScanUID,
  NFCA_Type2Read,
  NFCA_Type2Write,
  Mifare_ReadBlock,
  Mifare_WriteBlock,
  NFCB_Initial = 0x20,
  NFCB_ScanUID,
  NFCB_ST25TB512_Initiate,
  NFCB_ST25TB512_SelectChipID, 
  NFCB_ST25TB512_GetUID,  
  NFCB_ST25TB512_ReadBlock,
  NFCB_ST25TB512_WriteBlock,
  NFCV_Initial = 0x40,
  NFCV_ScanUID_TimeSlots_1,
  NFCV_ScanUID_TimeSlots_16,
  NFCV_ReadBlock = 0x43,
  NFCV_WriteBlock,
  Advanced_FWVersion,
  Advanced_Field_Off,
  Advanced_Field_Reset,
  Advanced_Buzzer_Turn_On,
  Advanced_Buzzer_Turn_Off,
  Advanced_Single_Card_Detect_Result,
  Advanced_Multi_Card_Detect_Result,
  Advanced_Read_Register,
  Advanced_Read_All_Register,
  Advanced_Write_Register,
  Advanced_Write_DefaultValue_Register,
  Advanced_MCU_Power_Mode,
  Advanced_MCU_Wakeup_Time,
  Advanced_CD_Calibration,
  Advanced_LOW_Power_Card_Detection_Start,
  Advanced_LOW_Power_Card_Detection_Scaning,
  Advanced_LOW_Power_Card_Detection_Stop,
};

/*******************************************************************************
* enum     MCU Power Mode TYPE
* brief    MCU Power Mode TYPE
*******************************************************************************/
enum
{
  SECTOR0,
  SECTOR1,
};

/*******************************************************************************
* enum     MCU Power Mode TYPE
* brief    MCU Power Mode TYPE
*******************************************************************************/
enum
{
  SLEEP = 0x00,
  DEEP_SLEEP,
  NORMAL
};

/*******************************************************************************
* enum     MCU Wake up time 
* brief    MCU Wake up time
*******************************************************************************/
enum
{
 PERIOD_100MS,
 PERIOD_200MS,
 PERIOD_500MS,
 PERIOD_1000MS
};

#endif
