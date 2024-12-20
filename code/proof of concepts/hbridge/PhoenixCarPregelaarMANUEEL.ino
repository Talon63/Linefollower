
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

struct param_t
{
  unsigned long cycleTime;
  int black[6];
  int white[6];
  int power = 10;
  float diff = 0.5;
  float kp = 10;
  float ki;
  float kd;
  //andere parameters die in het geheugen moeten
} params;



unsigned long previous;

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

void loop() {
  sCmd.readSerial();  
    
  unsigned long current = micros();
  if (current - previous >= 1000000)
  {
    previous = current;

    int index = 0;

    for (int i = 0; i < 8; i++) normalised[i] = map(analogRead(analogPins[i]), params.black[i], params.white[i], 0, 1000);

    for (int i=1; i < 8; i++) if (normalised[i] < normalised[index]) index = i;

    if (normalised[index] > 3000) run = false;

    if(index == 0) position = -50;
    else if (index == 7) position = 50;
    else{
      int sNul = normalised[index];
      int sMinEen = normalised[index-1];
      int sPlusEen = normalised[index+1];

      float b = sPlusEen - sMinEen;
      b = b / 2;
      float a = sPlusEen - b - sNul;
      position = -b /(2 * a);
      position += index;
      position -= 2.5;
      position *= 10;
    }

    if (debug) {
      Serial.println("loop still running");
      SerialBT.println("loop still running");
      SerialBT.print("Values: ");
      // Lees en print de waarden van de analoge pinnen
      for (int i = 0; i < 8; i++) {
        int analogValue = analogRead(analogPins[i]);
        //Serial.print(analogValue);
        SerialBT.print(analogValue);
        //Serial.print(" ");
        SerialBT.print(" ");
      }
      SerialBT.print("\n");
    }

    if (A) {
      Serial.println("Functie werkt ook");

      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 85);
      analogWrite(MotorB1, 85);
      analogWrite(MotorB2, 0);
      delay(1000);
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      A = false;
      
    }else{
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      delay(10);

    }

    if (B){
      Serial.println("Functie werkt ook");

      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 85);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 85);
      delay(1000);
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      B = false;
      
    }else{
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      delay(10);
    }

    if (C){
      Serial.println("Functie werkt ook");

      analogWrite(MotorA2, 85);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 85);
      analogWrite(MotorB2, 0);
      delay(1000);
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      C = false;
      
    }else{
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      delay(10);
    }

    if (D){
      Serial.println("Functie werkt ook");

      analogWrite(MotorA2, 255);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 255);
      delay(1000);
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      D = false;
      
    }else{
      analogWrite(MotorA2, 0);
      analogWrite(MotorA1, 0);
      analogWrite(MotorB1, 0);
      analogWrite(MotorB2, 0);
      delay(10);
    }




  //PID Regelaar begint hier
  debugposition = position;

  float error = -position; //error = setpoint - input
  float output = error * params.kp;
  output = constrain(output, -510, 510);



  if (run) if (output >= 0)
{
  Serial.println("ik ben hier geraakt");
  powerLeft = constrain(params.power + params.diff * output, -255, 255);
  powerRight = constrain(powerLeft - output, -255, 255);
  powerLeft = powerRight + output;
  Serial.println("powerLeft: ");
  Serial.println(powerLeft);
  Serial.println("powerRight: ");
  Serial.println(powerRight);
  Serial.println("power: ");
  Serial.println(params.power);
  Serial.println("diff: ");
  Serial.println(params.diff);
  Serial.println("kp: ");
  Serial.println(params.kp);
  Serial.println("output: ");
  Serial.println(output);



}
else
{
  //powerRight = constrain(params.power - params.diff * output, -255, 255);
  //powerLeft = constrain(powerRight + output, -255, 255);
  //powerRight = powerLeft - output;
}
  //analogWrite(MotorA1, powerLeft > 0 ? powerLeft : 0);
  //analogWrite(MotorA2, powerLeft < 0 ? -powerLeft : 0);
  //analogWrite(MotorB1, powerRight > 0 ? powerRight : 0);
  //analogWrite(MotorB2, powerRight < 0 ? -powerRight : 0);
  analogWrite(MotorA1, 0);
  analogWrite(MotorA2, 0);
  analogWrite(MotorB1, 0);
  analogWrite(MotorB2, 0);


  }
  Sensor();
  receivedChar =(char)SerialBT.read();

  //delay(20);

}

void Sensor(){

// Hier word de QTR-8A ingelezen

  //Serial.print("Values: ");
  //SerialBT.print("Values: ");
  // Lees en print de waarden van de analoge pinnen
  for (int i = 0; i < 8; i++) {
    int analogValue = analogRead(analogPins[i]);
    //Serial.print(analogValue);
    //SerialBT.print(analogValue);
    //Serial.print(" ");
    //SerialBT.print(" ");
  }
  //Serial.print("\n");
  //SerialBT.print("\n");


  // Wacht een seconde voor de volgende meting
  delay(10);

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
    if (strcmp(value, "on") == 0) {
      debug = true;
      Serial.println("Debug mode is ON");
      SerialBT.println("Debug mode is ON");      
    }
    else if (strcmp(value, "off") == 0) {
      debug = false;
      Serial.println("Debug mode is OFF");
      SerialBT.println("Debug mode is OFF");
    }
  }

    if (strcmp(param, "meter") == 0)
  {
    if (strcmp(value, "on") == 0) {
      A = true;
      Serial.println("Motors are ON");
      SerialBT.println("Motors are ON");      
    }
    else if (strcmp(value, "off") == 0) {
      A = false;
      Serial.println("Motors are OFF");
      SerialBT.println("Motors are OFF");
    }
  }

    if (strcmp(param, "rechts") == 0)
  {
    if (strcmp(value, "on") == 0) {
      B = true;
      Serial.println("Motors are ON");
      SerialBT.println("Motors are ON");      
    }
    else if (strcmp(value, "off") == 0) {
      B = false;
      Serial.println("Motors are OFF");
      SerialBT.println("Motors are OFF");
    }
  }

      if (strcmp(param, "links") == 0)
  {
    if (strcmp(value, "on") == 0) {
      C = true;
      Serial.println("Motors are ON");
      SerialBT.println("Motors are ON");      
    }
    else if (strcmp(value, "off") == 0) {
      C = false;
      Serial.println("Motors are OFF");
      SerialBT.println("Motors are OFF");
    }
  }

    if (strcmp(param, "achter") == 0)
  {
    if (strcmp(value, "on") == 0) {
      D = true;
      Serial.println("Motors are ON");
      SerialBT.println("Motors are ON");      
    }
    else if (strcmp(value, "off") == 0) {
      D = false;
      Serial.println("Motors are OFF");
      SerialBT.println("Motors are OFF");
    }
  }

    if (strcmp(param, "run") == 0)
  {
    if (strcmp(value, "on") == 0) {
      run = true;
      Serial.println("RUNNING");
      SerialBT.println("RUNNING");      
    }
    else if (strcmp(value, "off") == 0) {
      run = false;
      Serial.println("OFFLINE");
      SerialBT.println("OFFLINE");
    }
  }
}


