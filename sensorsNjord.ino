//No ROS Only Pyserial******Dont edit to ROS :/

#include <dht.h> 
dht DHT;  
#define outPin 3 

double adc_voltage1 = 0.0;
double adc_voltage2 = 0.0;
double adc_voltage3 = 0.0;
double in_voltage1 = 0.0;
double in_voltage2 = 0.0;
double in_voltage3 = 0.0;
 
double R1 = 30000.0;
double R2 = 7500.0; 

double ref_voltage = 5.0;

 
// Integer for ADC value
int adc_value1 = 0;
int adc_value2 = 0;
int adc_value3 = 0;

// **************************************************************//


double Vout1 = 0;
double Vout2 = 0;
double Vout3 = 0;

double Current1 = 0;
double Current2 = 0;
double Current3 = 0;

const double scale_factor = 0.1; // 20A
const double vRef = 5.00;
const double resConvert = 1024;
double resADC = vRef/resConvert;
double zeroPoint = vRef/2;



void setup() {
  Serial.begin(9600);

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);

}

void loop() {
   int readData = DHT.read11(outPin);

  float t = DHT.temperature;        // Read temperature
  float h = DHT.humidity; 
 
//   Serial.println("hello");

  for(int i = 0; i < 1000; i++) {
    Vout1 = (Vout1 + (resADC * analogRead(A3)));   
    delay(1);
  }
  Vout1 = Vout1 /1000;
  Current1 = (Vout1 - zeroPoint)/ scale_factor; 

 
  for(int i = 0; i < 1000; i++) {
    Vout2 = (Vout2 + (resADC * analogRead(A4)));   
    delay(1);
  }
  Vout2 = Vout2 /1000;
  Current2 = (Vout2 - zeroPoint)/ scale_factor; 



  for(int i = 0; i < 1000; i++) {
    Vout3 = (Vout3 + (resADC * analogRead(A5)));   
    delay(1);
  }
  Vout3 = Vout3 /1000;
  Current3 = (Vout3 - zeroPoint)/ scale_factor; 


  //**********************************************************//
  adc_value1 = analogRead(A0);
  adc_value2 = analogRead(A1);
  adc_value3 = analogRead(A2);
  adc_voltage1  = (adc_value1 * ref_voltage) / 1024.0; 
   adc_voltage2  = (adc_value2 * ref_voltage) / 1024.0; 
    adc_voltage3  = (adc_value3 * ref_voltage) / 1024.0; 
   
   // Calculate voltage at divider input
   in_voltage1 = adc_voltage1 / (R2/(R1+R2)); 
    in_voltage2 = adc_voltage2 / (R2/(R1+R2)); 
     in_voltage3 = adc_voltage3 / (R2/(R1+R2)); 
     

char buffer[80];
String tem1 = dtostrf(Current1, 5, 2, buffer);
String tem2 = dtostrf(Current2, 5, 2, buffer);
String tem3 = dtostrf(Current3, 5, 2, buffer);

String tem4 = dtostrf(in_voltage1, 5, 2, buffer);
String tem5 = dtostrf(in_voltage2, 5, 2, buffer);
String tem6 = dtostrf(in_voltage3, 5, 2, buffer);

String tem7 = dtostrf(t, 5, 2, buffer);

String temper = (tem4+" "+tem5+" "+tem6+" "+tem1+" "+tem2+" "+tem3 +" "+tem7);

  Serial.println(temper);

  delay(10); 
  
}
