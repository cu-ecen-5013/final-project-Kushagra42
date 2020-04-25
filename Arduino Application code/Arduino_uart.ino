#include <SoftwareSerial.h>
#include <stdlib.h>

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
  // Open serial communications and wait for port to open:
 while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  //Serial.println("Awaiting command from BBB...");
  // set the data rate for the SoftwareSerial port
  mySerial.begin(115200);
  pinMode(13, OUTPUT);
  
}

float temp_read()
{
  float voltage, degreesC, degreesF;
  voltage = getVoltage(temperaturePin);
  degreesC = (voltage - 0.5) * 100.0;  
  degreesF = degreesC * (9.0/5.0) + 32.0;
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(degreesC);
  Serial.print("  deg F: ");
  Serial.println(degreesF);
  return  degreesF;
  }

float getVoltage(int pin)
{
    return (analogRead(pin) * 0.004882814);
}

void loop() // run over and over
{
  //Serial.print(temp_read());
  if (mySerial.available())
    {
       // Serial.write(mySerial.read());
       delay(10);
       inChar = mySerial.read();
  
      if(inChar== '1')
        {
             Serial.print("Received read and transmit instructions from BBB ");
             digitalWrite(13, HIGH);

             float f_temp = temp_read();
             send_temp = (f_temp * 100);
             itoa(send_temp,conv,10);

             for(int i = 0; i<3; i++)
              {
                 mySerial.write(conv[i]); 
              }
      
              Serial.print(send_temp);
      
              delay(20);
        }
      else if(value == '0')
        {
             Serial.print("Awaitng read and transmit instruction from BBB");    
              digitalWrite(13, LOW);   
        }
    }
}
