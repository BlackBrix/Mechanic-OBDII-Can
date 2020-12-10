/*********************************************************************
 * Mechanic - Hacking your car
 *
 * Copyright (C) 2013 Joerg Pleumann
 * 
 * This example is free software; you can redistribute it and/or
 * modify it under the terms of the Creative Commons Zero License,
 * version 1.0, as published by the Creative Commons Organisation.
 * This effectively puts the file into the public domain.
 *
 * This example is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * LICENSE file for more details.
 */

#include <Mechanic.h>

ObdInterface obd;
ObdMessage msg;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  obd.setSlow(false);
  obd.setExtended(false);
  obd.setDebug(false);
  obd.begin();
}

void loop() {
  boolean supported;
    
  if (obd.isPidSupported(0x0c, supported)) {
    if (supported) {
      float value;
      
      if (obd.getPidAsFloat(0x0c, 0.0f, 16383.75f, value)) {
        Serial.print("Drehzahl ist: ");
        Serial.println(value);
      } else {
        Serial.print("Abfrage fehlgeschlagen");
      }
    } else {
    Serial.println("PID wird nicht unterstützt");
    }
  } else {
    Serial.println("PID-Check fehlgeschlagen");
  }
  
  delay(1000);
}

