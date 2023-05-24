//For Arduino Mega only -- 2 Serial communication are used

#include <IBusBM.h>
#include <Servo.h>
char receivedChars[32];
boolean newData = false;

float la=0,ra=0,lf=0,rf=0;

byte servoPinA = 9;  //Thruster 1
byte servoPinB= 10;  // Thruster 2
byte servoPinC = 6;  //Thruster 3
byte servoPinD = 5;  //Thruster 4

Servo servoA;
Servo servoB;   
Servo servoC;
Servo servoD;

IBusBM ibus;  

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

void setup() {
  Serial2.begin(115200); 
    Serial1.begin(9600); 
  ibus.begin(Serial2,IBUSBM_NOTIMER); 
 
  servoA.attach(servoPinA);
    servoB.attach(servoPinB);
  servoC.attach(servoPinC);
  servoD.attach(servoPinD);
  pinMode(2,OUTPUT); // LED for RF
  pinMode(3,OUTPUT); // LED for Auto
  
   servoA.writeMicroseconds(1500);
   
   servoB.writeMicroseconds(1500);
   
   servoC.writeMicroseconds(1500);
   
   servoD.writeMicroseconds(1500);
    delay(2000); 
 
}

void loop() {
  
   ibus.loop();
   int Ch5val= readChannel(4, 1200, 1800, 1500);
 
  if(Ch5val>1500){  
    auto_control() ; 
 
      digitalWrite(2,HIGH);  // RF LED HIGH
      digitalWrite(3,LOW);   // Auto LED LOW
  }
  else{
    Rf_control();
      digitalWrite(2,LOW); // Auto  LED HIGH
      digitalWrite(3,HIGH); // RF LED LOW
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

  while (Serial1.available() > 0 && newData == false)
  {
    rc = Serial1.read();

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
