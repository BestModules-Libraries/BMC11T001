/***********************************************************
File:                ISO15693_Write_Read.ino
Description:         1.HardwareSerial4 interface (BAUDRATE 115200)is used to communicate with BMC11T001.
                     2.Hardware serial (BAUDRATE 9600) is used to communicate with serial port monitors. 
                       Obtain the UID of the ISO15693 card and read and write its EEPROM.
                       And it is displayed on the serial port monitor
connection method:   Plug in BMduino UNO directly                      
***********************************************************/
#include "BMC11T001.h"
BMC11T001     BMC11(&Serial4); //create an object, select Seria

int nlens;                              
uint8_t uid_buf[23] = {0};           
uint8_t data[18] = {0};         

void setup() 
{
  delay(1000);          //Wait for the BMC11T001 to complete initialization                                      
  BMC11.begin(115200);  //Initialize the serial port settings and configure the serial port BaudRate fixed to 115200bps                        
  Serial.begin(115200); //Set the communication rate between the serial monitor and BMC11T001 to 115200bps
  BMC11.begin_ISO15693();  //Initialize the ISO15693
}

void loop() 
{
  nlens = BMC11.getSingleUID_ISO15693(uid_buf); //Gets the UID
  Serial.write(uid_buf,nlens);   //The UID is displayed on the serial monitor                                                
  Serial.println(" "); 
 if(!BMC11.writeBlock_ISO15693(0x0A,0x22245678,uid_buf)) //Write data to the EEPROM of the Mifare card and determine whether it is successful.
  { 
   nlens = BMC11.readBlock_ISO15693(0x0A,uid_buf,data);//Read data written to EEPROM.
   Serial.write(data,nlens);   //The value read is displayed in the serial monitor                                                    
   Serial.println(" "); 
  }
  Serial.println("");
  delay(2000);
}
