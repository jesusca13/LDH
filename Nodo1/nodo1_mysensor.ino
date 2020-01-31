/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * Simple binary switch example 
 * Connect button or door/window reed switch between 
 * digitial I/O pin 3 (BUTTON_PIN below) and GND.
 * http://www.mysensors.org/build/binary
 */


// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_RF24

#define MY_RF24_CHANNEL 48
#define MY_NODE_ID 87

#include <MySensors.h>
#include <Bounce2.h>

#define CHILD_ID 6
#define CHILD_ID_2 7
#define CHILD_ID_RELE 5

#define BUTTON_PIN  6  // Arduino Digital I/O pin for button/reed switch
#define BUTTON_PIN_2 7

#define RELE_PIN 8
#define RELE_ON 1  // GPIO value to write to turn on attached relay
#define RELE_OFF 0 // GPIO value to write to turn off attached relay
#define NUMBER_OF_RELAYS 1

Bounce debouncer = Bounce(); 
Bounce debouncer2 = Bounce(); 

int oldValue=-1;
int oldValue2=-1;

// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(CHILD_ID,V_TRIPPED);
MyMessage msg_2(CHILD_ID_2,V_TRIPPED);
MyMessage msg_RELE(CHILD_ID_RELE,V_TRIPPED);

void setup()  
{  
  // Setup the button 1
  pinMode(BUTTON_PIN,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN,HIGH);

  // Setup the button 2
  pinMode(BUTTON_PIN_2,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN_2,HIGH);

  //Setup RELE
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);

  // After setting up the button, setup debouncer
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);

  debouncer2.attach(BUTTON_PIN_2);
  debouncer2.interval(5);

}

void presentation() {
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(CHILD_ID, S_DOOR);  //Especifica su configuracion
  present(CHILD_ID_2, S_DOOR);  //Especifica su configuracion
  present(CHILD_ID_RELE, S_LIGHT);
}

void before()
{
  for (int sensor=1, pin=RELE_PIN; sensor<=NUMBER_OF_RELAYS; sensor++, pin++) {
    // Then set relay pins in output mode
    pinMode(pin, OUTPUT);
    // Set relay to last known state (using eeprom storage)
    digitalWrite(pin, loadState(sensor)?RELE_ON:RELE_OFF);
  }
}

void loop() 
{
  button1();
  button2();
}

int button1()
{

  debouncer.update();
  // Get the update value
  int value = debouncer.read();

  if (value != oldValue) {
     // Send in the new value
     send(msg.set(value==HIGH ? 1 : 0));
     oldValue = value;
     digitalWrite(RELE_PIN, !digitalRead(RELE_PIN));
  }
  
  return value;
}

int button2()
{

  debouncer2.update();
  // Get the update value
  int value = debouncer2.read();

  if (value != oldValue2) {
     // Send in the new value
     send(msg_2.set(value==HIGH ? 1 : 0));
     oldValue2 = value;
     digitalWrite(RELE_PIN, !digitalRead(RELE_PIN));
  }
  return value;
}

void receive(const MyMessage &message)
{
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_STATUS) {
    // Change relay state
    digitalWrite(RELE_PIN, message.getBool()?RELE_ON:RELE_OFF);
    // Store state in eeprom
    saveState(RELE_PIN, message.getBool());
    // Write some debug info
    Serial.print("Incoming change for sensor:");
    Serial.print(message.sensor);
    Serial.print(", New status: ");
    Serial.println(message.getBool());
  }
}
