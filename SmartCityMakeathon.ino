#include "DHT.h"
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3);
DHT dht(4, DHT22);
int bty = 0;
float nowHum = 0;
int humPer = 0;
String myString = "";
int dry = 0;
int dryTmp = 0;
void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  dht.begin();
}

long readVcc() {
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long bty = (high<<8) | low;

  bty = 1125300L / bty; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return bty; // Vcc in millivolts
}

 void loop() {
   while(BTSerial.available()) {
    char myChar = (char)BTSerial.read();
    myString+=myChar;
    delay(5);
   }
   bty = readVcc();
   float hum = dht.readHumidity();
   float temp = dht.readTemperature();
   if(isnan(hum) || isnan(temp)) {
     Serial.println("Failed to read from DHT");
   } else {
    if(!myString.equals("")) {
      nowHum=hum;
      Serial.println("aaa");
      myString="";
     }
     Serial.print("Humidity: ");
     Serial.print(hum);
     Serial.println(" %");
     Serial.print("Temperature: ");
     Serial.print(temp);
     Serial.println(" *C");
     Serial.print(dry);
     Serial.println("%");
     Serial.println(dryTmp);
     Serial.print(bty/60);
     Serial.println(" %");

     BTSerial.print((int)hum);
     BTSerial.print(", ");
     BTSerial.print((int)temp);
     BTSerial.print(", ");
     BTSerial.print(dry);
     BTSerial.println(", ");
     BTSerial.print(bty/60);
     BTSerial.println();
   }
   delay(2000);
   dryTmp++;
   if(dryTmp == 180) {
    dry++;
    dryTmp = 0;
   }
 }

