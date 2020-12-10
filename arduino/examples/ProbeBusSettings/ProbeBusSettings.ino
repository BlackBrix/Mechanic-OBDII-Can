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

boolean probe(boolean slow, boolean extended) {
  ObdInterface obd;
  boolean dummy;
  
  obd.setSlow(slow);
  obd.setExtended(extended);
  
  obd.begin();
  boolean result = obd.isPidSupported(1, dummy);
  obd.end();
  
  delay(250);
  
  return result;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
}

void loop() {
  Serial.println();
  Serial.println();
  Serial.println("CAN bus being probed...");
  Serial.println();
  
  Serial.print("500 kbps, standard IDs: ");
  Serial.println(probe(false, false) ? "Yes" : "No");
  Serial.print("500 kbps, extended IDs: ");
  Serial.println(probe(false, true) ? "Yes" : "No");
  Serial.print("250 kbps, standard IDs: ");
  Serial.println(probe(true, false) ? "Yes" : "No");
  Serial.print("250 kbps, extended IDs: ");
  Serial.println(probe(true, true) ? "Yes" : "No");
  
  for (;;);  
}

