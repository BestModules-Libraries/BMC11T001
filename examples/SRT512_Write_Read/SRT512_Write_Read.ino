/***********************************************************
File:                SRT512_Write_Read.ino
Description:         1. HardwareSerial4 interface (BAUDRATE 115200)is used to communicate with BMC11T001.
                     2. Hardware serial (BAUDRATE 9600) is used to communicate with serial port monitors.
                     Obtain the UID of the SRT512 card and read and write its EEPROM. 
                     And it is displayed on the serial port monitor
connection method:   Plug in BMduino UNO directly         
***********************************************************/
#include "BMC11T001.h"
BMC11T001     BMC11(&Serial4);
 
int nlens;                              
uint8_t uid_buf[26] = {0};
uint8_t Random[12] = {0};
uint8_t Random1[12] ={0};
uint8_t data[21] = {0};
void setup() 
{
  delay(1000);          //Wait for the BMC11T001 to complete initialization                                      
  BMC11.begin(115200);  //Initialize the serial port settings and configure the serial port BaudRate fixed to 115200bps                        
  Serial.begin(115200); //Set the communication rate between the serial monitor and BMC11T001 to 115200bps
  BMC11.begin_ISO14443B();  ////Initialize the ISO14443B 
  BMC11.initiate_ST25TB512_ISO14443B(Random); //Initialize the ST25TB512 Tag
  BMC11.selectChipID_ST25TB512_ISO14443B(Random,Random1); //Select the ST25TB512 Tag Chip ID
  
}

void loop() 
{   
  nlens = BMC11.getUID_ST25TB512_ISO14443B(uid_buf); //Gets the UID
  Serial.write(uid_buf,nlens); //The UID is displayed on the serial monitor                                                  
  Serial.println(" ");
  BMC11.writeBlock_ST25TB512_ISO14443B(0x07,0x11852369);//Write data to the EEPROM of the ST25TB512 card 
  nlens = BMC11.readBlock_ST25TB512_ISO14443B(0x07,data); //Read data written to EEPROM
  Serial.write(data,nlens);       //The value read is displayed in the serial monitor                                             
  Serial.println(" ");
  Serial.println("");
  delay(2000);
}
