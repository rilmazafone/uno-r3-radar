#include <Servo.h>
#include "SR04.h"

const int servoPin = 9;
const int triggerPin = 12;
const int echoPin = 11;
const int buzzerPin = 8;

Servo myservo;
SR04 mySensor = SR04(echoPin,triggerPin);

long distance;

int printDistance(int i,int distance){
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
}
void beepBuzzer(int beepInterval){
  digitalWrite(buzzerPin,HIGH);
  delay(beepInterval);
  digitalWrite(buzzerPin,LOW);
  delay(1);
}

void setup(){
  pinMode(buzzerPin,OUTPUT);
  myservo.attach(servoPin);
  Serial.begin(9600);
}

void loop(){
  for(int i=0;i<=180;i++){
    myservo.write(i);
    delay(20);
    distance=mySensor.Distance();
    printDistance(i,distance);
    if(distance < 40)
    {
      int beepInterval = map(distance,40,0,50,1);
      beepBuzzer(beepInterval);
    }
  }
  for(int i=180;i>=0;i--){
    myservo.write(i);
    delay(20);
    distance = mySensor.Distance();
    printDistance(i,distance);
    if(distance < 40)
    {
      int beepInterval = map(distance,40,0,50,1);
      beepBuzzer(beepInterval);
    }
  }
}