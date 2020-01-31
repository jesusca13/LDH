
// Enable debug prints to serial monitor
//#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

#define MY_RF24_CHANNEL 48
#define MY_NODE_ID 86

#include <SPI.h>

#include <MySensors.h>

int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point

uint32_t SLEEP_TIME = 5000; //900000  // sleep time between reads (seconds * 1000 milliseconds)
int oldBatteryPcnt = 0;

#define CHILD_ID 3
#define CHILD_ID_SENSOR 4

//SENSOR TEMPERATURA/HUMEDAD
// Incluimos librería
#include <DHT.h>
 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 3
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

MyMessage msg(CHILD_ID,V_VOLTAGE);
MyMessage msg_sensor(CHILD_ID_SENSOR, V_TEMP);

void setup(){
  // use the 1.1 V internal reference
  #if defined(__AVR_ATmega2560__)
      analogReference(INTERNAL1V1);
  #else
      analogReference(INTERNAL);
  #endif

  // Comenzamos el sensor DHT
  dht.begin();

  Serial.begin(9600);
  
}

void presentation(){
  
    present(CHILD_ID, S_MULTIMETER);
    present(CHILD_ID_SENSOR, S_TEMP);
    
}

void loop()
{

    float sensorValue = readVcc();

    float voltage = sensorValue / 1000;
    
    send(msg.set(voltage, 2));
    
    /*if (oldBatteryPcnt != batteryPcnt) {
        // Power up radio after sleep
        sendBatteryLevel(batteryPcnt);
        oldBatteryPcnt = batteryPcnt;
    }
*/

    // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  send(msg_sensor.set(t, 5));
    
  sleep(SLEEP_TIME);
}





long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
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

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
