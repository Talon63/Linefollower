
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
  Serial.println("To turn ON send: H");//print on serial monitor  
  Serial.println("To turn OFF send: L"); //print on serial monitor 
  pinMode(LEDpin, OUTPUT);
 
  SerialPort.begin(Baudrate);

  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("calibrate", onCalibrate);
  sCmd.addCommand("run", onRun);
  sCmd.addCommand("cmd", onCmd);
  sCmd.setDefaultHandler(onUnknownCommand);

  SerialPort.println("ready");


  for (int i = 0; i < 8; i++) {
    pinMode(analogPins[i], INPUT);
  }

  pinMode(MotorA2, OUTPUT);
  pinMode(MotorA1, OUTPUT);
  pinMode(MotorB1, OUTPUT);
  pinMode(MotorB2, OUTPUT);

}

void loop() 
{
 sCmd.readSerial();

 unsigned long current = micros();
 if (current - previous >= params.cycleTime)
 {
  previous = current;
  
  //Waardes normaliseren: 
  for (int i = 0; i < 8; i++) normalised[i] = map(analogRead(analogPins[i]), params.black[i], params.white[i], 0, 1000);

  //Kwadratische interpolatie:
  int index = 0;
  float position;
  for (int i=1; i < 8; i++) if (normalised[i] < normalised[index]) index = i;

  if (normalised[index] > 3000) run = false;

  if (index == 0) position = -20;
  else if (index == 7) position = 20;
  else
  {
    int sNul = normalised[index];
    int sMinEen = normalised[index-1];
    int sPlusEen = normalised[index+1];

    float b = sPlusEen - sMinEen;
    b = b / 2;
    float a = sPlusEen - b - sNul;
    position = -b /(2 * a);
    position += index;
    position -= 3.5;
    position *= 10;
  }
  debugposition = position;

  //PID
  float error = -position;
  float output = error * params.kp;
  iTerm += params.ki * error;
  iTerm = constrain(iTerm, -510, 510);
  output += iTerm;
  output += params.kd * (error - lastErr);
  lastErr = error;
  output = constrain(output, -510, 510);
  
  int powerLeft = 0;
  int powerRight = 0;

  if (run) if (output >= 0)
  {
    powerLeft = constrain(params.power + params.diff * output, -255, 255);
    powerRight = constrain(powerLeft - output, -255, 255);
    powerLeft = powerRight + output;
  }
  else
  {
    powerRight = constrain(params.power - params.diff * output, -255, 255);
    powerLeft = constrain(powerRight + output, -255, 255);
    powerRight = powerLeft - output;
  }
  
  
  analogWrite(MotorA1, powerLeft > 0 ? powerLeft : 0);
  analogWrite(MotorA2, powerLeft < 0 ? -powerLeft : 0);
  analogWrite(MotorB1, powerRight > 0 ? powerRight : 0);
  analogWrite(MotorB2, powerRight < 0 ? -powerRight : 0);

  Serial.println("Powerleft : ");
  Serial.println(powerLeft);
  Serial.println("right: ");
  Serial.println(powerRight);
  Serial.println("Output: ");
  Serial.println(output);

  //for (int i = 0; i < 8; i++) Serial.println(normalised[i]);
  //Serial.println("NEWNEWNEW");
  
 }
 unsigned long difference = micros() - current;
 if (difference > calculationTime) calculationTime = difference;
}

void onSet()
{
  char* parameter = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(parameter, "cycle") == 0)
  {
    long newCycleTime = atol(value);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);
    //params.ki *= ratio;
    params.kd /= ratio;
    params.cycleTime = newCycleTime;
  }
  else if (strcmp(parameter, "power") == 0) params.power = atol(value);
  else if (strcmp(parameter, "diff") == 0) params.diff = atof(value);
  else if (strcmp(parameter, "kp") == 0) params.kp = atof(value);
  else if (strcmp(parameter, "ki") ==0) params.ki = atof(value);
  else if (strcmp(parameter, "kd") ==0) params.kd = atof(value);
  else SerialBT.println("unknown paramameter");

}

void onDebug()
{
  SerialBT.print("cycle time: ");
  SerialBT.println(params.cycleTime);

  SerialBT.print("black: ");
  for (int i = 0; i < 8; i++)
  {
    SerialBT.print(params.black[i]);
    SerialBT.print(" ");    
  }
  SerialBT.println(" ");
  
  SerialBT.print("white: ");
  for (int i = 0; i < 8; i++)
  {
    SerialBT.print(params.white[i]);
    SerialBT.print(" ");    
  }
  SerialBT.println(" "); 

  SerialBT.print("normalised: ");
  for (int i = 0; i < 8; i++)
  {
    SerialBT.print(normalised[i]);
    SerialBT.print(" ");    
  }
  SerialBT.println(" ");

  SerialBT.print("position: ");
  SerialBT.println(debugposition);

  SerialBT.print("power: ");
  SerialBT.println(params.power);

  SerialBT.print("diff: ");
  SerialBT.println(params.diff);

  SerialBT.print("Kp: ");
  SerialBT.println(params.kp);

  //float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
  float ki = params.ki; // / cycleTimeInSec;
  SerialBT.print("Ki: ");
  SerialBT.println(ki, 5);
  
  float kd = params.kd; //* cycleTimeInSec;
  SerialBT.print("Kd: ");
  SerialBT.println(kd);

  SerialBT.print("calculation time: ");
  SerialBT.println(calculationTime);
  calculationTime = 0;
}

void onCalibrate()
{
  char* parameter = sCmd.next();
  if (strcmp(parameter, "black") == 0)
  {
    SerialBT.print("start calibrating black... ");
    for (int i = 0; i < 8; i++) params.black[i] = analogRead(analogPins[i]);
    SerialBT.print("done");
  }
  else if (strcmp(parameter, "white") == 0)
  {
    SerialBT.print("start calibrating white... ");
    for (int i = 0; i < 8; i++) params.white[i] = analogRead(analogPins[i]);
    SerialBT.print("done");
  }
}

void onRun()
{
  char* parameter = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(parameter, "on") == 0)
  {
   iTerm = 0;
   run = true;
  }
  else run = false;
}

void onCmd()
{
  SerialBT.println("-------Possible commands:-------");
  SerialBT.println("calibrate white");
  SerialBT.println("calibrate black");
  SerialBT.println("set cycle 'x' (<10000 µs");
  
  SerialBT.println("set power 'x' (0...255)");
  SerialBT.println("set diff 'x' (0...1)");
  SerialBT.println("set kp 'x'");
  SerialBT.println("set ki 'x'");
  SerialBT.println("set kd 'x'");
}

void onUnknownCommand(char* command)
{
  SerialBT.print("Unknown Command: \"");
  SerialBT.print(command);
  SerialBT.println("\"");
}
/*
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
*/