
   
   #if defined(ARDUINO) && ARDUINO >= 100
     #include "Arduino.h"
   #else
     #include <WProgram.h>
   #endif
   
   #include <Servo.h> 
   #include <ros.h>
   //#include <std_msgs/UInt16.h>
   #include <std_msgs/Int32MultiArray.h>
   
   ros::NodeHandle  nh;
   
   Servo servoA;
   Servo servoB;
   Servo servoC;
   Servo servoD;
   
   void servo_cb( const std_msgs::Int32MultiArray& cmd_msg){
   
     servoA.writeMicroseconds(cmd_msg.data[0]);
     servoB.writeMicroseconds(cmd_msg.data[1]); 
     servoC.writeMicroseconds(cmd_msg.data[2]);
     servoD.writeMicroseconds(cmd_msg.data[3]);
   }
   
   
   ros::Subscriber<std_msgs::Int32MultiArray> sub("/thrust_val", servo_cb);
   
   void setup(){
  
   
     nh.initNode();
     nh.subscribe(sub);
     
     servoA.attach(4);
     servoB.attach(5);
     servoC.attach(6); 
     servoD.attach(7);
     
     servoA.writeMicroseconds(1500);
     servoB.writeMicroseconds(1500);
     servoC.writeMicroseconds(1500);
     servoD.writeMicroseconds(1500);
     
     delay(2000);
   }
   
   void loop(){
    
     nh.spinOnce();
     delay(1);
   }
