//  Slave code

#include <Wire.h>
#include<Servo.h>

#define NUM_INTS 5 // number of integers to receive
Servo servo;
int receivedData[NUM_INTS]; // array to store received integers
char receivedChars[18];
boolean newData = false;
int data[5]={0,0,0,0,0};
float la=0,ra=0,lf=0,rf=0;

byte servoPinA = 9;
byte servoPinB= 10;
byte servoPinC = 6;
byte servoPinD = 5;


Servo servoA;
Servo servoB;
Servo servoC;
Servo servoD;


void setup() {
  Serial.begin(115200);
    servoA.attach(servoPinA);
    servoB.attach(servoPinB);
  servoC.attach(servoPinC);
  servoD.attach(servoPinD);


   servoA.writeMicroseconds(1500); // send "stop" signal to ESC.


  // Attach iBus object to serial port
  
    delay(2000);
 
}

void loop() {
  
  // do something with received data here
   Wire.begin(8);

    Wire.onReceive(receiveEvent);
}

//*********************************8

void receiveEvent(int bytesReceived) {
  Serial.println("data received");
 
  for (int i = 0; i < 5; i++) {
    data[i] = Wire.read() << 8 | Wire.read();
    Serial.println(data[i]);
  }
//  for (int i = 0; i < 5; i++) {
//    Serial.print(data[i]);
//    Serial.print(" ");
//  }
//  Serial.println();
  if(data[0]==0){Rf_control();digitalWrite(LED_BUILTIN,HIGH);}
  else if(data[0]==1){auto_control();digitalWrite(LED_BUILTIN,LOW);};
  Serial.println(data[1]);
  Serial.println(data[0]);
}

//******************************
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
 

  
  int Ch1val= data[1];
  int Ch2val= data[2];
  int Ch3val= data[3];
  int Ch4val= data[4];
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
