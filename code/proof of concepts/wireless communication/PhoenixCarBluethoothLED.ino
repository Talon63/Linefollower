
 //* Turn LED ON or OFF from your phone 
 //* Using ESP32's Bluetooth and Mobile app
 //* turn ON and OFF LED


//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
int received;// received value will be stored in this variable
char receivedChar;// received value will be stored as CHAR in this variable

const char turnON ='H';
const char turnOFF ='L';
const int LEDpin = 2;


#include "SerialCommand.h"

#define SerialPort Serial
#define Baudrate 115200

SerialCommand sCmd(SerialPort);
bool debug;
unsigned long previous;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("PhoenixCar"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println("To turn ON send: H");//print on serial monitor  
  Serial.println("To turn OFF send: L"); //print on serial monitor 
  pinMode(LEDpin, OUTPUT);
 
  SerialPort.begin(Baudrate);

  sCmd.addCommand("set", onSet);
  sCmd.setDefaultHandler(onUnknownCommand);

  SerialPort.println("ready");



}

void loop() {
    receivedChar =(char)SerialBT.read();

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  
  }
  if (SerialBT.available()) {
    
    SerialBT.print("Received:");// write on BT app
    SerialBT.println(receivedChar);// write on BT app      
    Serial.print ("Received:");//print on serial monitor
    Serial.println(receivedChar);//print on serial monitor    
    //SerialBT.println(receivedChar);//print on the app    
    //SerialBT.write(receivedChar); //print on serial monitor
    if(receivedChar == turnON)
    {
     SerialBT.println("LED ON:");// write on BT app
     Serial.println("LED ON:");//write on serial monitor
     digitalWrite(LEDpin, HIGH);// turn the LED ON
       
    }
    if(receivedChar == turnOFF)
    {
     SerialBT.println("LED OFF:");// write on BT app
     Serial.println("LED OFF:");//write on serial monitor
      digitalWrite(LEDpin, LOW);// turn the LED off 
    }    
     
  

  }
  delay(20);

  sCmd.readSerial(); 
  unsigned long current = micros();
  if (current - previous >= 1000000)
  {
    previous = current;
    if (debug) Serial.println("loop still running");
  }


}



void onUnknownCommand(char *command)
{
  SerialPort.print("unknown command: \"");
  SerialPort.print(command);
  SerialPort.println("\"");
}

void onSet()
{  
  char* param = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(param, "debug") == 0)
  {
    if (strcmp(value, "on") == 0) debug = true;
    else if (strcmp(value, "off") == 0) debug = false;
  }
}



