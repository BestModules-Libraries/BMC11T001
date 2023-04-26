/***********************************************************
File:                Mifare_Write_Read.ino
Description:         1.HardwareSerial4 interface (BAUDRATE 115200)is used to communicate with BMC11T001.
                     2.Hardware serial (BAUDRATE 9600) is used to communicate with serial port monitors. 
                       Obtain the UID of the Mifare card and read and write its EEPROM.
                     And it is displayed on the serial port monitor
connection method:   Plug in BMduino UNO directly                      
***********************************************************/
#include "BMC11T001.h" 
BMC11T001     BMC11(&Serial4); //create an object, select Serial4 Interface Communication

int nlens;                            
uint8_t uid_buf[50] = {0}; 
uint8_t DATA[37] = {0};                                  
uint8_t KeyTypeA[] = "ka";                                          
uint8_t KeyTypeB[] = "kb";                                          
uint8_t Key[]="FFFFFFFFFFFF";                                       
uint8_t Mifare_Data[] = "11FFFFFFFFFFFF0780691122CCDDEE33";      

void setup() 
{
  delay(1000);          //Wait for the BMC11T001 to complete initialization                                      
  BMC11.begin(115200);  //Initialize the serial port settings and configure the serial port BaudRate fixed to 115200bps                        
  Serial.begin(115200); //Set the communication rate between the serial monitor and BMC11T001 to 115200bps
  BMC11.begin_ISO14443A();  //Initialize the ISO14443A                         
}
void loop()
{ 
  nlens = BMC11.getUID_ISO14443A(uid_buf); //Gets the UID
  Serial.write(uid_buf,nlens);  //The UID is displayed on the serial monitor                                                  
  Serial.println(" ");
  if(!BMC11.writeMifareBlock_ISO14443A(KeyTypeA,Key,0x15,Mifare_Data)) //Write data to the EEPROM of the Mifare card and determine whether it is successful.
  { 
   nlens = BMC11.readMifareBlock_ISO14443A(KeyTypeA,Key,0x15,DATA);//Read data written to EEPROM.
   Serial.write(DATA,nlens);   //The value read is displayed in the serial monitor                                                  
   Serial.println(" ");
  }
  Serial.println("");
  delay(2000);
}
