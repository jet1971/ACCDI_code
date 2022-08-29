


#include "TimerOne.h"    // used for rpm calc

//#include "SafeString.h" // faster dedugging
//createSafeString(msgStr, 5);

const int triggerPin = 2;
const int scrGatePin = 7;
const int ledPin =  13;
const int potPin = A0;

int sensorValue = 0;

int triggerState = 1;

int rpm = 0;
int flag = 0;

int count = 0;  // temp for debug only

unsigned long getTime = 0;
unsigned long getTimeAgain = 0;
unsigned long fireNow = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis;
unsigned long rotationTime = 0;
unsigned long previousTriggerTime = 0;
unsigned long timingDelay = 0;            // delay microseconds between trigger and firing (eventually)
//unsigned long previousMicros =0;          // used inside ISR !

const unsigned long setGateOnTime = 1000;
volatile unsigned long gateOnTime = 0;

int preState = 0;
volatile int rotationCount = 0;
volatile int  triggerRecievedFlag = 0;
volatile int  gateFlag = 0;


bool  useProcessor = false;

void setup() {
  Serial.begin(2000000);          // as fast as it goes to prevent blocking
  pinMode(triggerPin, INPUT);    //  pickup coil on bike
  pinMode(scrGatePin, OUTPUT);   // to SCR gate pin
  //pinMode(ledPin, OUTPUT);       // debug

  attachInterrupt(digitalPinToInterrupt(triggerPin), isr, RISING);
  Timer1.initialize(500000);         // initialize timer1, and set a 1 second period
  Timer1.attachInterrupt(TimerCallback);  // attaches callback() as a timer overflow interrupt

  //  SafeString::setOutput(Serial);

}

void isr()
{

  triggerRecievedFlag = 1;
  rotationCount ++;
  // Serial.println("Hello");

}


void TimerCallback()
{
  rpm = rotationCount * 120;
  rotationCount = 0;
}


void loop() {

  if (triggerRecievedFlag == 1) {

    calcTiming();
  }
  Serial.print("RPM = ");
  Serial.print(rpm);
  Serial.print("  ");
  Serial.print("Timing Delay = ");
  Serial.println(timingDelay);


}
void calcTiming() {

  if (rpm <= 1395) {
    useProcessor = false;
    triggerRecievedFlag = 0;
   

  } else {
    useProcessor = true;
    timingDelay = 0;      // no delay = maximum advance
  }

  getTime = millis();

  if (flag == 0) {
    fireNow = getTime + timingDelay;
  }



  

  if (getTimeAgain >= fireNow && useProcessor == true) {                 // timingDelay is time from trigger point to spark output.

    gateOn();

   

    flag = 0;
    triggerRecievedFlag = 0;

  } else {
    getTimeAgain = millis();

    flag = 1;
  }

}

void gateOn() {

  // digitalWrite(ledPin, HIGH);
  digitalWrite(scrGatePin, HIGH);



  delayMicroseconds(1000);                 // 1 milli second
  //Serial.println(" Going LOW");
  // digitalWrite(ledPin, LOW);
  digitalWrite(scrGatePin, LOW);
  gateFlag = 0;
  
}
