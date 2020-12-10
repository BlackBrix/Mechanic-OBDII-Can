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

int pids[] = { 0x0d, 0x0c, 0x04, 0x05, 0x2f };

String names[] = { "Speed", "RPM", "Load", "Temp", "Fuel" };

String units[] = { "km/h", "1/m", "%", "°C", "%" };

boolean supported[] = { false, false, false, false, false };

float minimum[] = { 0, 0, 0, -40.0f, 0 };

float maximum[] = { 255.0f, 16383.75f, 100.0f, 215.0f, 100.0f };
  
void setup() {
  Serial.begin(115200);
  while (!Serial);

  obd.setSlow(false);
  obd.setExtended(false);
  obd.setDebug(false);
  obd.begin();
  
  for (int i = 0; i < sizeof(pids); i++) {
    obd.isPidSupported(pids[i], supported[i]);
  }
}

void loop() {
  for (int i = 0; i < sizeof(pids); i++) {
    float value = 0;
    if (supported[i] && obd.getPidAsFloat(pids[i], minimum[i], maximum[i], value)) {
      Serial.print(names[i]);
      Serial.print("=");
      Serial.print(value);
      Serial.print(units[i]);
      Serial.print(" ");
    }
  }
  Serial.println();

  delay(1000);
}

