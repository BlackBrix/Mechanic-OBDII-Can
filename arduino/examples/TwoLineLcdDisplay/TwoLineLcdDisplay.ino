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

SoftwareSerial lcd(4, 5);

/*
 * Clear screen on Sparkfun serial LCD.
 */
void clearScreen() {
  lcd.write(0xfe);
  lcd.write(0x01);
}

/*
 * Set cursor on Sparkfun serial LCD.
 */
void gotoXY(int line, int column) {
  lcd.write(0xfe);
  lcd.write(0x80 + (64 * line + column));
}

ObdInterface obd;
ObdMessage msg;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  obd.setSlow(false);
  obd.setExtended(false);
  obd.setDebug(false);
  obd.begin();

  lcd.begin(9600);
}

void loop() {
  char buffer[16];

  clearScreen();

  obd.getPidAsString(0x04, 0.0f, 100.0f, "%d %%", buffer);
  gotoXY(0, 0);
  lcd.print(buffer);
  
  obd.getPidAsString(0x05, -40.0f, 215.0f, "%d", buffer);
  gotoXY(0, 8);
  lcd.print(buffer);
  lcd.write(0xDF); // Degree sign on Sparkfun serial LCD
  lcd.print("C");
  
  obd.getPidAsString(0x0c, 0.0f, 16383.75f, "%d rpm", buffer);
  gotoXY(1, 0);
  lcd.print(buffer);

  obd.getPidAsString(0x0d, 0.0f, 255.0f, "%d km/h", buffer);
  gotoXY(1, 8);
  lcd.print(buffer);
  
  delay(1000);
}

