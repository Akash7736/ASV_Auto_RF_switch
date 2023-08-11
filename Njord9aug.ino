
   
   #if defined(ARDUINO) && ARDUINO >= 100
     #include "Arduino.h"
   #else
     #include <WProgram.h>
   #endif
   
   #include <Servo.h> 
   #include <IBusBM.h>
   #include <ros.h>
   //#include <std_msgs/UInt16.h>
   #include <std_msgs/Int32MultiArray.h>
   #include <std_msgs/Int32.h>
   unsigned long myTime;
   ros::NodeHandle  nh;

     #define green 24
   #define yellow 30
   #define red 28
   #define fan 26

    #define fstime 10
   
   Servo servoA;
   Servo servoB;
   Servo servoC;
   Servo servoD;

byte servoPinA = 4;  //Thruster 1
byte servoPinB= 5;  // Thruster 2
byte servoPinC = 6;  //Thruster 3
byte servoPinD = 7;  //Thruster 4

int fsval[60];
int fsnxtval[60];
int lastime = 0;
int i =0;
int j = 0;
int k = 0;



IBusBM ibus;  

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}


      void warn_cb( const std_msgs::Int32& warn_msg){
   
   if(warn_msg.data==1){digitalWrite(yellow,LOW);}
   else{digitalWrite(yellow,HIGH);}
   }

   
   void servo_cb( const std_msgs::Int32MultiArray& cmd_msg){
   
     servoA.writeMicroseconds(cmd_msg.data[0]);
     servoB.writeMicroseconds(cmd_msg.data[1]); 
     servoC.writeMicroseconds(cmd_msg.data[2]);
     servoD.writeMicroseconds(cmd_msg.data[3]);
   }

   void failsafe(){
       servoA.writeMicroseconds(1500);
     servoB.writeMicroseconds(1500);
     servoC.writeMicroseconds(1500);
     servoD.writeMicroseconds(1500);
  }
   
   
   ros::Subscriber<std_msgs::Int32MultiArray> subt("/thrust_val", servo_cb);
   ros::Subscriber<std_msgs::Int32> subw("/warning", warn_cb);
   
   void setup(){
  
   
     nh.initNode();
     nh.subscribe(subt); //thruster subscriber
     nh.subscribe(subw);  //warning subscriber

 
     ibus.begin(Serial1,IBUSBM_NOTIMER); 
//     Serial.begin(9600);
     
     servoA.attach(servoPinA);
     servoB.attach(servoPinB);
     servoC.attach(servoPinC); 
     servoD.attach(servoPinD);
     
    pinMode(red,OUTPUT); // LED for Auto
    pinMode(green,OUTPUT); // LED for RF
       pinMode(yellow,OUTPUT); 
    pinMode(fan,OUTPUT);

       digitalWrite(red,HIGH);
   digitalWrite(green,HIGH);
   digitalWrite(yellow,HIGH);
   digitalWrite(fan,HIGH);
    
     servoA.writeMicroseconds(1500);
     servoB.writeMicroseconds(1500);
     servoC.writeMicroseconds(1500);
     servoD.writeMicroseconds(1500);
     
     delay(2000);
   }
   
   void loop(){
     ibus.loop();
     int Ch6val = readSwitch(5, false); // Fail Safe 
    
//    if(myTime<60){
//      if(Ch6val==1){gaincount+=1;}
//      if(Ch6val==0){losscount+=1;}
//    }

  myTime = millis()/1000;
      if(lastime!=myTime){
        if(i<fstime){
        fsval[i] = Ch6val;
        i++;}
        else{for(j=1;j<fstime;j++){fsnxtval[j-1]=fsval[j] ;
        fsnxtval[fstime-1]= Ch6val;
      
        }}
        }
        
for(k=0;k<fstime;k++){
  fsval[k] = fsnxtval[k];
  }
      
       
         lastime = myTime;
     if(myTime%59 ==0){digitalWrite(fan,LOW);}
     if(myTime%179==0){digitalWrite(fan,HIGH);}
  
   //int Ch5val= readChannel(4, 1200, 1800, 1500);
 int Ch7val = readSwitch(6, true);

   //int Ch7val= readChannel(6, 1200, 1800, 1500);

  int sum = 0;
   for(int i =0;i<fstime;i++){
    sum+= fsval[i];
//   Serial.println(fsval[i]);
}
   
   if(sum==fstime){failsafe();digitalWrite(yellow,LOW);}
   else if(sum==0){digitalWrite(yellow,HIGH);}

   
//   Serial.println("Done");
//   Serial.print("CH6val : ");
//     Serial.println(Ch6val);

//Serial.println(Ch6val); 
  if(Ch7val==1){  

     digitalWrite(green,LOW);  // Auto LED HIGH
      digitalWrite(red,HIGH);   // RF LED LOW
//      Serial.println("yesss auto");
     nh.spinOnce();
     delay(1);

 
     
  }
  else{
    digitalWrite(green,HIGH); // Auto  LED LOW
      digitalWrite(red,LOW); // RF LED HIGHed,
    Rf_control();
//    Serial.println(Ch7val);
      
    }
   }



void Rf_control(){
   int servoA_int = 0;
   int servoB_int = 0;
   int servoC_int = 0;
   int servoD_int = 0;

  int Ch1val= readChannel(0, 1300, 1700, 1500);
  int Ch2val= readChannel(1, 1300, 1700, 1500);
  int Ch3val= readChannel(2, 1300, 1700, 1500);
  int Ch4val= readChannel(3, 1300, 1700, 1500);
  int Ch5val= readChannel(4, 1300, 1700, 1500);
   // int Ch7val= readChannel(6, 1200, 1800, 1500);
   int Ch7val = readSwitch(6, false);
 
  delay(10);

if(Ch4val<=1450 || Ch4val >=1550 ){
   servoA_int += Ch4val; // Send signal to ESC. ---- yaw
  servoB_int +=  3000 - Ch4val;
  servoC_int +=  3000 - Ch4val;
  servoD_int +=  Ch4val;

  }
 
else  if((Ch1val>=1450 && Ch1val<=1550) && (Ch2val>1550 || Ch2val<1450)) {
 servoA_int +=  Ch2val; // Send signal to ESC. --- surge
  servoB_int +=  Ch2val;
  servoC_int +=  Ch2val ;
  servoD_int += Ch2val;

}

else if((Ch1val>1550 || Ch1val<1450 )&&(Ch2val>=1450 && Ch2val<=1550)){
    servoA_int += Ch1val; // Send signal to ESC. ---- sway
  servoB_int +=   3000 - Ch1val;
  servoC_int +=   Ch1val;
  servoD_int += 3000 - Ch1val;

  }
else{
      servoA_int += 1500 ; // Send signal to ESC. ---- zero condition
  servoB_int +=   1500;
  servoC_int +=  1500;
  servoD_int += 1500;
  };
      servoA.writeMicroseconds(servoA_int);
   servoB.writeMicroseconds(servoB_int); 
   servoC.writeMicroseconds(servoC_int); 
   servoD.writeMicroseconds(servoD_int);
}
