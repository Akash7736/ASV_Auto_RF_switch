/*
  Arduino FS-I6X Demo
  fsi6x-arduino-mega-ibus.ino
  Read iBus output port from FS-IA6B receiver module
  Display values on Serial Monitor

  Channel functions by Ricardo Paiva - https://gist.github.com/werneckpaiva/

  DroneBot Workshop 2021
  https://dronebotworkshop.com
*/

// Include iBusBM Library
#include <IBusBM.h>
#include <Servo.h>
char receivedChars[18];
boolean newData = false;

float la=0,ra=0,lf=0,rf=0;

byte servoPinA = 9;
byte servoPinB= 10;
byte servoPinC = 6;
byte servoPinD = 5;


Servo servoA;
Servo servoB;
Servo servoC;
Servo servoD;



// Create iBus Object
IBusBM ibus;

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup() {
  // Start serial monitor
  Serial.begin(115200);
  servoA.attach(servoPinA);
    servoB.attach(servoPinB);
  servoC.attach(servoPinC);
  servoD.attach(servoPinD);



   servoA.writeMicroseconds(1500); // send "stop" signal to ESC.


  // Attach iBus object to serial port
  
    delay(2000);
 
    ibus.begin(Serial);
}

  
void loop() {
  
   int Ch5val= readChannel(4, 1200, 1800, 1500);
  if(Ch5val>1500){
  Rf_control();}
  else if(Ch5val<1500){
   auto_control() ; 
    }

}

void auto_control(){
    //Get commanded speed
  recvWithStartEndMarkers();
  if (newData == true)
  {
    parseData();
    newData = false;
  }

//  la = map(la,-10,10,1300,1700);
//   ra = map(ra,-10,10,1300,1700);
//    lf = map(lf,-10,10,1300,1700);
//     rf = map(rf,-10,10,1300,1700);
     
  //return cmd
  Serial.print("<");
  Serial.print(int(la));
  Serial.print(",");
  Serial.print(int(ra));
  Serial.print(",");
  Serial.print(int(lf));
    Serial.print(",");
  Serial.print(int(rf));
  Serial.print(">\n");

        servoA.writeMicroseconds(la);
   servoB.writeMicroseconds(ra); 
   servoC.writeMicroseconds(lf); 
   servoD.writeMicroseconds(rf);
  
  }


void Rf_control(){
   int servoA_int = 0;
   int servoB_int = 0;
   int servoC_int = 0;
   int servoD_int = 0;

  // Cycle through first 5 channels and determine values
  // Print values to serial monitor
  // Note IBusBM library labels channels starting with "0"
  

  for (byte i = 0; i < 5; i++) {
    int value = readChannel(i, 1400, 1600, 1500);
    Serial.print("Ch");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(value);
    Serial.print(" | ");
  }


  
  int Ch1val= readChannel(0, 1200, 1800, 1500);
  int Ch2val= readChannel(1, 1200, 1800, 1500);
  int Ch3val= readChannel(2, 1200, 1800, 1500);
  int Ch4val= readChannel(3, 1200, 1800, 1500);
  int Ch5val= readChannel(4, 1200, 1800, 1500);
  delay(10);

if(Ch4val!=1500){
   servoA_int += 3000 - Ch4val; // Send signal to ESC. ---- yaw
  servoB_int +=   Ch4val;
  servoC_int +=  3000 - Ch4val;
  servoD_int += Ch4val;

  }
 
else  if (Ch1val == 1500 && Ch2val != 1500 ) {
 servoA_int += 3000 - Ch2val; // Send signal to ESC. --- surge
  servoB_int +=  3000 - Ch2val;
  servoC_int +=  3000 - Ch2val ;
  servoD_int += 3000 - Ch2val;

}

else if(Ch1val!=1500 && Ch2val == 1500){
    servoA_int += 3000 - Ch1val; // Send signal to ESC. ---- sway
  servoB_int +=   Ch1val;
  servoC_int +=  Ch1val;
  servoD_int += 3000 - Ch1val;

  }
 else if (Ch1val ==1500 && Ch2val==1500 && Ch4val ==1500 ) {
      servoA_int += 1500 ; // Send signal to ESC. ---- zero condition
  servoB_int +=   1500;
  servoC_int +=  1500;
  servoD_int += 1500;
  }
else{
   servoA_int += 3000 - (Ch2val+ Ch1val)/2; // Send signal to ESC.  ---differential
  servoB_int +=   (Ch2val+ Ch1val)/2;
  servoC_int +=   (Ch2val + Ch1val)/2;
  servoD_int += 3000 - (Ch2val + Ch1val)/2;

  };
      servoA.writeMicroseconds(servoA_int);
   servoB.writeMicroseconds(servoB_int); 
   servoC.writeMicroseconds(servoC_int); 
   servoD.writeMicroseconds(servoD_int);
}


void recvWithStartEndMarkers()
{
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();

    if (recvInProgress == true)
    {
      if (rc != endMarker)
      {
        receivedChars[ndx] = rc;
        ndx++;
      }
      else
      {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker)
    {
      recvInProgress = true;
    }
  }
}


//============
void parseData()
// split the data into its parts
{
  char *strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(receivedChars, ","); // get the first part - left speed
  la = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ","); // get the second part - right speed
  ra = atoi(strtokIndx);
  
  strtokIndx = strtok(NULL, ","); 
  lf = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ","); 
  rf = atoi(strtokIndx);
}
