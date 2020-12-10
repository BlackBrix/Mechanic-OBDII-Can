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
  float speed = 0, rpm = 0, load = 0, temp = 0, fuel = 0;
  
  obd.getPidAsFloat(0x0d, 0.0f, 255.0f, speed);
  obd.getPidAsFloat(0x0c, 0.0f, 16383.75f, rpm);
  obd.getPidAsFloat(0x04, 0.0f, 100.0f, load);
  obd.getPidAsFloat(0x05, -40.0f, 215.0f, temp);
  obd.getPidAsFloat(0x2F, 0.0f, 100.0f, fuel);

  Serial.print(" Speed: ");
  Serial.print(speed);
  Serial.print("km/h RPM: ");
  Serial.print(rpm);
  Serial.print("min Load: ");
  Serial.print(load);
  Serial.print("% Temp: ");
  Serial.print(temp);
  Serial.print("°C Fuel: ");
  Serial.print(fuel);
  Serial.println("%");
  
  delay(1000);
}

