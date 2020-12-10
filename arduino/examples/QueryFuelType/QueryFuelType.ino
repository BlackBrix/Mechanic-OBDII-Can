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
  obd.setDebug(false);
  obd.begin();
}

String fuelName[] = {
  "Gasoline",
  "Methanol",
  "Ethanol",
  "Diesel",
  "LPG",
  "CNG",
  "Propane",
  "Electric",
  "Bifuel running Gasoline",
  "Bifuel running Methanol",
  "Bifuel running Ethanol",
  "Bifuel running LPG",
  "Bifuel running CNG",
  "Bifuel running Prop",
  "Bifuel running Electricity",
  "Bifuel mixed gas/electric",
  "Hybrid gasoline",
  "Hybrid Ethanol",
  "Hybrid Diesel",
  "Hybrid Electric",
  "Hybrid Mixed fuel",
  "Hybrid Regenerative"
};

void loop() {
  word fuelType;
  
  if (obd.getPidAsInteger(0x51, fuelType)) {
    Serial.print("Fuel type: ");
    Serial.print(fuelType, DEC);
    Serial.print(" (");
    if (fuelType > 0 && fuelType <= sizeof(fuelName)) {
      Serial.print(fuelName[fuelType - 1]);
    } else {
      Serial.print("Unknown");
    }
    Serial.println(")");
  } else {
    Serial.println("Unable to query fuel type.");
  }

  for(;;);
}

