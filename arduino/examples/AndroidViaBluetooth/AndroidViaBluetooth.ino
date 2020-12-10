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
#include <SoftwareSerial.h>

ObdInterface obd;
ObdMessage msg;

SoftwareSerial blue(5, 4);

boolean hasSpeed = false, hasRpm = false, hasLoad = false, hasTemp = false, hasFuel = false; 

float speed = 0, rpm = 0, load = 0, temp = 0, fuel = 0;

char buffer[256];

boolean probe(boolean slow, boolean extended) {
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

  blue.begin(9600);

  if (!probe(false, false)) {
    if (!probe(false, true)) {
      if (!probe(true, false)) {
        if (!probe(true, true)) {
          for (;;); // Error
        }
      }
    }
  }

  obd.setDebug(false);
  obd.begin();
  
  obd.isPidSupported(0x0d, hasSpeed);
  obd.isPidSupported(0x0c, hasRpm);
  obd.isPidSupported(0x04, hasLoad);
  obd.isPidSupported(0x05, hasTemp);
  obd.isPidSupported(0x2f, hasFuel);
}

void receiveRequest() {
  int length = 0;
  boolean newline = false;

  while (!newline && length < sizeof(buffer)) {
    if (blue.available() > 0) {
      int c = blue.read();
      
      if (c == 10) {
        buffer[length++] = 0;
        newline = true;
      } else if (c == 13) {
        // Ignore
      } else {
        buffer[length++] = c;
      }
    }
  }
}

void loop() {
  if (blue.available() > 0) {
	receiveRequest();
	
	if (strcmp("vin", request)) {
      blue.print("vin,");
	  if (obd.getMultiframePid(0x09, 0x02, buffer, count)) {
		buffer[count] = 0;
        blue.print(buffer);
	  }
      blue.println();
	} else if (strcmp("dtc", request)) {
	  if (obd.getMultiframePid(0x03, -1, buffer, count)) {
		blue.print("dtc");
		for (int i = 0; i < count; i += 2) {
	      blue.print(",");
	      
		  byte first = (byte)buffer[i];
		  byte second = (byte)buffer[i + 1];
		  
		  switch(first >> 6) {
			case 0: blue.print("P"); break;
			case 1: blue.print("C"); break;
			case 2: blue.print("B"); break;
			case 3: blue.print("U"); break;
		  }
		 
		  blue.print((first >> 4) & 0x03, DEC);
		  
		  blue.print(first & 0x0f, HEX);
		  blue.print(second >> 4, HEX);
		  blue.print(second & 0x0f, HEX);
		}
		
		blue.println();
	  }
	}
  }
  
  if (hasSpeed) {
    obd.getPidAsFloat(0x0d, 0.0f, 255.0f, speed);
  }
  
  if (hasRpm) {
    obd.getPidAsFloat(0x0c, 0.0f, 16383.75f, rpm);
  }
  
  if (hasLoad) {
    obd.getPidAsFloat(0x04, 0.0f, 100.0f, load);
  }
  
  if (hasTemp) {
    obd.getPidAsFloat(0x05, -40.0f, 215.0f, temp);
  }
  
  if (hasFuel) {
	obd.getPidAsFloat(0x2F, 0.0f, 100.0f, fuel);
  }

  blue.print("mom,");
  blue.print(obd.isSlow() ? "true," : "false,");
  blue.print(obd.isExtended() ? "true," : "false,");
  blue.print(speed);
  blue.print(",");
  blue.print(rpm);
  blue.print(",");
  blue.print(load);
  blue.print(",");
  blue.print(temp);
  blue.print(",");
  blue.print(fuel);
  blue.write(13);
  blue.write(10);
  
  delay(100);
}
