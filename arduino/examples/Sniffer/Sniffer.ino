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
  
  Serial.println();
  Serial.println();
  Serial.println("  ADDR   LN MO PI D0 D1 D2 D3 D4");
  Serial.println("-------- -- -- -- -- -- -- -- --");
  
  obd.setSlow(false);
  obd.setExtended(false);
  obd.setDebug(false);
  obd.setNoFilter(true); // Show all messages, not just ECU answers
  obd.begin();
}

void loop() {
  if (obd.receiveMessage(msg)) {
    msg.printTo(Serial);
  }
  
  delay(20);
}

