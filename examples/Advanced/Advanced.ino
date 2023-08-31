/***********************************************************
File:                Advanced.ino
Description:         1.HardwareSerial4 interface (BAUDRATE 115200)is used to communicate with BMC11T001.
                     2.Hardware serial (BAUDRATE 115200) is used to communicate with serial port monitors. 
                       Obtain the UID of  card .And it is displayed on the serial port monitor
                     3.Make sure that there are no tags or metal objects near the antenna when performing RF calibration.
connection method:   Plug in BMduino UNO directly                      
***********************************************************/
#include "BMC11T001.h"
BMC11T001     BMC11(&Serial4); //create an object, select Serial4 Interface Communication
int nlens;                              
uint8_t uid_buf[50] = {0};         

void setup() 
{
  delay(1000);                                    //Power on for the first time and wait for BMC11T001 to be ready       
  BMC11.begin(115200);  //Initialize the serial port settings and configure the serial port BaudRate fixed to 115200bps                        
  Serial.begin(115200); //Set the communication rate between the serial monitor and BMC11T001 to 115200bps                          
  BMC11.setMCUPowerMode_ADVANCED(SLEEP); // Set the working mode
  BMC11.setMCUWakeupTime_ADVANCED(PERIOD_100MS); // Set the interval between automatic wake-up in sleep mode
  BMC11.setCDCalibration_ADVANCED(); //RF calibration
  BMC11.startLowPowerCardDetection_ADVANCED(0x07); // Set the type of card that needs to be scanned
}

void loop() 
{
  nlens = BMC11.scaningLowPowerCardDetection_ADVANCED(uid_buf); ////Gets the UID&Working mode
  Serial.write(uid_buf,nlens); //The UID&Working mode is displayed on the serial monito                                                  
  Serial.println(" "); 
  delay(200);
}
