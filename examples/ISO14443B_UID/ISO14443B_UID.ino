/***********************************************************
File:                ISO14443B_UID.ino
Description:         1.HardwareSerial4 interface (BAUDRATE 115200)is used to communicate with BMC11T001.
                     2.Hardware serial (BAUDRATE 9600) is used to communicate with serial port monitors. 
                       Obtain the UID of the ISO14443B card.And it is displayed on the serial port monitor
connection method:   Plug in BMduino UNO directly                      
***********************************************************/
#include "BMC11T001.h"
BMC11T001     BMC11(&Serial4);  //create an object, select Serial
 
int nlens;                              
uint8_t uid_buf[50] = {0};            
        
void setup() 
{
  delay(1000);         //Wait for the BMC11T001 to complete initialization   
  BMC11.begin(115200);  //Initialize the serial port settings and configure the serial port BaudRate fixed to 115200bps                        
  Serial.begin(115200); //Set the communication rate between the serial monitor and BMC11T001 to 115200bps
  BMC11.begin_ISO14443B();  //Initialize the ISO14443B      
}

void loop() 
{
  nlens = BMC11.getUID_ISO14443B(uid_buf); //Gets the UID
  Serial.write(uid_buf,nlens);     //The UID is displayed on the serial monitor                                                 //Output to serial port to view events
  Serial.println(" ");
  delay(2000);
}
