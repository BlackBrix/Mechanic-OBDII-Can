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

  Serial.println();
  Serial.println();
  Serial.println("   | 0 1 2 3 4 5 6 7 8 9 A B C D E F");
  Serial.println("---+--------------------------------");
  
  for (int i = 0; i <= 0xf0; i += 0x10) {
    Serial.print(i, HEX);
    Serial.print(i == 0 ? "0 |" : " |");
    for (int j = 0; j <= 0x0f; j++) {
      boolean supported;
      if (obd.isPidSupported(i + j, supported)) {
        Serial.print(supported ? " X" : "  ");
      } else {
        Serial.print(" ?");
      }
      
      delay(50);
    }
    Serial.println();
  }
}

void loop() {
}

