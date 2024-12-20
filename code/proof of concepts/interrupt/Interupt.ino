
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
const int LEDpin = 22;

String receivedString = "";


#include "SerialCommand.h"

#define SerialPort SerialBT
#define Baudrate 115200

SerialCommand sCmd(SerialPort);
bool debug;
bool A;
bool B;
bool C;
bool D;
bool run;

float position;
float debugposition;

int powerLeft = 0;
int powerRight = 0;

int normalised[8];

float iTerm, lastErr;

struct param_t
{
  unsigned long cycleTime;
  int black[8];
  int white[8];
  int power = 200;
  float diff = 0.15;
  float kp = 18;
  float ki = 0.002;
  float kd = 100;
  //andere parameters die in het geheugen moeten
} params;



unsigned long previous, calculationTime;

const int analogPins[8] = {A0, A3, A6, A7, A4, A5, A18, A19};

const int MotorA1 = 18;
const int MotorA2 = 19;
const int MotorB1 = 17;
const int MotorB2 = 16;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("PhoenixCar"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(LEDpin, OUTPUT);
 
  SerialPort.begin(Baudrate);

  SerialPort.println("ready");

}

void loop() 
{
}


void interrupt() {
   static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    run = !run;
  }
  last_interrupt_time = interrupt_time;
  }
