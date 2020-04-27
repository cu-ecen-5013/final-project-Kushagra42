#include <SoftwareSerial.h>
#include <stdlib.h>

#define LOAD_MINIMUM_TIME_MS      10
#define LOAD_MAXIMUM_TIME_MS      250
#define SYNC_TIME_MS              100

#define DUMMY_MS()                       random(LOAD_MINIMUM_TIME_MS, LOAD_MAXIMUM_TIME_MS)

SoftwareSerial mySerial(2, 3); // RX, TX
char inChar;
const int temperaturePin = 0;
float result;
//String send_temp;
int value;
int send_temp;
char conv[20]= {0};

void setup()
{
  Serial.begin(9600);
  while(!Serial); 
  mySerial.begin(115200);
}

float temp_read()
{
  float voltage, degreesC, degreesF;
  voltage = getVoltage(temperaturePin);
  degreesC = (voltage - 0.5) * 100.0;  
  degreesF = degreesC * (9.0/5.0) + 32.0;
  return  degreesF;
}

float getVoltage(int pin)
{
  return (analogRead(pin) * 0.004882814);
}

void sendTempSerially(float f_temp)
{
  send_temp = (f_temp * 100);
  itoa(send_temp,conv,10);
  for(int i = 0; i<4; i++)  mySerial.write(conv[i]); 
}

void loop() // run over and over
{
  //Serial.print(temp_read());
  if (mySerial.available())
  {
    // Serial.write(mySerial.read());
    inChar = mySerial.read();
    
    if(inChar== '1')
    {
      //delay(DUMMY_MS());
      float f_temp = temp_read();
      delay(SYNC_TIME_MS);
      //sendTempSerially(70.59);
      sendTempSerially(f_temp);
    }
    //else if(inChar == '0')  Serial.print("Awaitng read and transmit instruction from BBB");  
  }
}
