#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 7); // RX, TX
char inChar;
const int temperaturePin = 0;

void setup()
{
 
  Serial.begin(9600);
  // Open serial communications and wait for port to open:
 
  Serial.println("Awaiting command from BBB...");
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  pinMode(13, OUTPUT);
  
}

void temp_read()
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
  }

float getVoltage(int pin)
{
    return (analogRead(pin) * 0.004882814);
}

void loop() // run over and over
{
  if (mySerial.available()){
    inChar = (char)mySerial.read();
    if(inChar=='1'){
      Serial.print("Received read and transmit instructions from BBB ");
      digitalWrite(13, HIGH);
      temp_read();
     }else if(inChar =='0'){
      Serial.print("Awaitng read and transmit instruction from BBB");    
      digitalWrite(13, LOW);
    }else{
      Serial.print("Unknown instructions ");    
   }
  }
}
