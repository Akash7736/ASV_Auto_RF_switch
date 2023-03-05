////  Master Code

#include <Wire.h>
#include<IBusBM.h>
IBusBM ibus;


void setup() {
  Wire.begin();
   Serial.begin(115200);
    ibus.begin(Serial);
}

int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

void loop() {
  Wire.beginTransmission(8);

 
//  int pot = analogRead(A0);
//  int Ch5val = map(pot,0,1023,1100,1900);
int Ch5val= readChannel(4, 1200, 1800, 1500);
int Ch4val= readChannel(3, 1200, 1800, 1500);
int Ch3val= readChannel(2, 1200, 1800, 1500);
int Ch2val= readChannel(1, 1200, 1800, 1500);
int Ch1val= readChannel(0, 1200, 1800, 1500);
    
  int data[5] = {0,0,0,0,0};
       

  
  
    data[1] = Ch1val;
  data[2] = Ch2val;
   data[3] = Ch3val;
   data[4] = Ch4val;
  
  if(Ch5val>1500){
    data[0] = 1;}// integers to send Auto
  else if (Ch5val<=1500){
     data[0] = 0;}// integers to send RF
  


   for (int i = 0; i < 5; i++) {
    Wire.write(data[i] >> 8); // send the high byte
    Wire.write(data[i] & 0xFF); // send the low byte
    Serial.println("data transmitted");
  }
  Wire.endTransmission();

  delay(100);
}
