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

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  obd.setSlow(false);
  obd.setExtended(false);
  obd.setDebug(true);
  obd.begin();
}

void loop() {
  char buffer[64];
  int count = 0;
  
  if (obd.getMultiframePid(0x03, -1, buffer, count)) {
    Serial.print("Number of DTCs: ");
    Serial.println(count / 2, DEC);
    Serial.println();
    
    if (count > sizeof(buffer)) {
      Serial.println("Oops, too many. Please increase buffer!");
      for (;;);
    }
  
    /*
     * Iterate through the buffer and decode all DTCs
     * according to what Wikipedia tells us:
     *
     * http://en.wikipedia.org/wiki/OBD-II_PIDs#Bitwise_encoded_PIDs
     */  
    for (int i = 0; i < count; i += 2) {
      byte first = (byte)buffer[i];
      byte second = (byte)buffer[i + 1];
      
      /*
       * The two highest bits encode the subsystem.
       */
      switch(first >> 6) {
        case 0: Serial.print("P"); break;
        case 1: Serial.print("C"); break;
        case 2: Serial.print("B"); break;
        case 3: Serial.print("U"); break;
      }
     
      /*
       * Next two bits encode the first digit (0-3).
       */
      Serial.print((first >> 4) & 0x03, DEC);
      
      /*
       * Remaining 12 bits are simply three hex digits.
       */
      Serial.print(first & 0x0f, HEX);
      Serial.print(second >> 4, HEX);
      Serial.print(second & 0x0f, HEX);
      
      Serial.println();
    }
  }

  for(;;);
}

