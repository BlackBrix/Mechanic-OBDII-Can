/*********************************************************************
 * Mechanic - Hacking your Car
 *
 * Copyright (C) 2013 Joerg Pleumann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * LICENSE file for more details.
 */

#include <Mechanic.h>

// Controls whether all messages are being shown
#define DEBUG true

// Controls whether stress tests are included
#define STRESS true

// Number of passed and failed tests
int pass = 0;
int fail = 0;

// Assertion facility, macros and helper functions
#define TEST __TEST__(__FUNCTION__);
#define PASS pass++; return;
#define FAIL fail++; return;
#define ASSERT(x, y) if(!__ASSERT__(x, y, __FUNCTION__)) { FAIL; }

void __TEST__(const char funcName[]) {
  Serial.println();
  Serial.print("***** ");
  Serial.print(funcName);
  Serial.println(" *****");
  Serial.println();
}

boolean __ASSERT__(int assertNo, boolean exprValue, const char funcName[]) {
  if (!(exprValue)) {
    Serial.print(F("Assertion "));
    Serial.print(assertNo);
    Serial.print(F(" in function "));
    Serial.print(funcName);
    Serial.println(F("() failed"));

    return false;
  }

  return true;
}

// One-time initializer actually does all the work
void setup() {
  Serial.begin(115200);

  while (!Serial);

  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print(F("Press <Return> to start Mechanic "));
  Serial.print(MECHANIC_VERSION);
  Serial.println(F(" test suite..."));
  while(true) {
    int c = Serial.read();
    if (c == 10 || c == 13) {
      break;
    }
  }

  testObdMessage();
  testClear();
  testPrintTo();
  testGetIntValue();
  testGetFloatValue();

  testObdInterface();

  testGetSetSlow();
  testGetSetExtended();
  testGetSetDebug();
  testGetSetLoopback();
  testGetSetNoFilter();

  testBeginEnd();
  testSendReceiveMessage();
  testExchangeMessage();

  if (STRESS) {
    testSendReceiveMessageStress1();
    testSendReceiveMessageStress2();
    testExchangeMessageStress();
  }

  // testIsPidSupported();
  // testGetPidAsInteger();
  // testGetPidAsFloat();
  // testGetPidAsString();
  
  testGetMultiframePid();
  
  Serial.println();
  Serial.print(F("Total tests  : ")); 
  Serial.println(pass + fail);
  Serial.print(F("Passed tests : ")); 
  Serial.println(pass);
  Serial.print(F("Failed tests : ")); 
  Serial.println(fail);
  Serial.println();
  Serial.println(fail == 0 ? ":-)" : ":-(");  
  Serial.flush();
}

// Loop stays empty
void loop() {
}

void testObdMessage() {
  TEST;

  ObdMessage msg;

  ASSERT(0, msg.address == 0x7df);
  ASSERT(1, msg.length == 2);
  ASSERT(2, msg.mode == 1);
  ASSERT(3, msg.pid == 0);

  PASS;
}

void testClear() {
  TEST;

  ObdMessage msg;

  msg.address = 0x666;
  msg.length = 7;
  msg.mode = 0x08;
  msg.pid = 0x15;

  for (int i = 0; i < 5; i++) {
    msg.values[i] = i;
  }

  msg.clear();

  ASSERT(0, msg.address == 0x7df);
  ASSERT(1, msg.length == 2);
  ASSERT(2, msg.mode == 1);
  ASSERT(3, msg.pid == 0);

  for (int i = 0; i < 5; i++) {
    ASSERT(4 + i, msg.values[i] == 0);
  }

  PASS;
}

void testPrintTo() {
  TEST;

  ObdMessage msg;

  int i = msg.printTo(Serial);
  Serial.println();

  ASSERT(0, i == 32);

  PASS;
}

void testGetIntValue() {
  TEST;

  ObdMessage msg;

  msg.length = 3;

  msg.values[0] = 0;
  msg.values[1] = 0;

  ASSERT(0, msg.getIntValue() == 0);

  msg.values[0] = 1;

  ASSERT(1, msg.getIntValue() == 1);

  msg.values[1] = 2;

  ASSERT(2, msg.getIntValue() == 1);

  msg.length = 4;

  ASSERT(3, msg.getIntValue() == 258);

  msg.values[0] = 0xff;
  msg.values[1] = 0xff;

  ASSERT(4, msg.getIntValue() == 0xffff);

  PASS;
}

void testGetFloatValue() {
  TEST;

  ObdMessage msg;

  msg.length = 3;

  msg.values[0] = 0;
  ASSERT(0, abs(-10.0f - msg.getFloatValue(-10.0f, 10.0f)) < 0.05f);

  msg.values[0] = 0x80;
  ASSERT(1, abs(0.0f - msg.getFloatValue(-10.0f, 10.0f)) < 0.05f);

  msg.values[0] = 0xff;
  ASSERT(2, abs(10.0f - msg.getFloatValue(-10.0f, 10.0f)) < 0.05f);

  msg.length = 4;

  msg.values[0] = 0;
  msg.values[0] = 0;
  ASSERT(3, abs(-10.0f - msg.getFloatValue(-10.0f, 10.0f)) < 0.05f);

  msg.values[0] = 0x80;
  ASSERT(4, abs(0.0f - msg.getFloatValue(-10.0f, 10.0f)) < 0.05f);

  msg.values[0] = 0xff;
  ASSERT(5, abs(9.9f - msg.getFloatValue(-10.0f, 10.0f)) < 0.05f);

  msg.values[1] = 0xff;
  ASSERT(6, abs(10.0f - msg.getFloatValue(-10.0f, 10.0f)) < 0.05f);

  PASS;
}

void testObdInterface() {
  TEST;

  ObdInterface obd;

  ASSERT(0, !obd.isSlow());
  ASSERT(1, !obd.isExtended());
  ASSERT(2, !obd.isDebug());
  ASSERT(3, !obd.isLoopback());
  ASSERT(4, !obd.isNoFilter());

  PASS;
}

void testGetSetSlow() {
  TEST;

  ObdInterface obd;
  obd.setSlow(true);
  ASSERT(0, obd.isSlow());
  obd.setSlow(false);
  ASSERT(1, !obd.isSlow());

  PASS;
}

void testGetSetExtended() {
  TEST;

  ObdInterface obd;
  obd.setExtended(true);
  ASSERT(0, obd.isExtended());
  obd.setExtended(false);
  ASSERT(1, !obd.isExtended());

  PASS;
}

void testGetSetDebug() {
  TEST;

  ObdInterface obd;
  obd.setDebug(true);
  ASSERT(0, obd.isDebug());
  obd.setDebug(false);
  ASSERT(1, !obd.isDebug());

  PASS;
}

void testGetSetLoopback() {
  TEST;

  ObdInterface obd;
  obd.setLoopback(true);
  ASSERT(0, obd.isLoopback());
  obd.setLoopback(false);
  ASSERT(1, !obd.isLoopback());

  PASS;
}

void testGetSetNoFilter() {
  TEST;

  ObdInterface obd;
  obd.setNoFilter(true);
  ASSERT(0, obd.isNoFilter());
  obd.setNoFilter(false);
  ASSERT(1, !obd.isNoFilter());

  PASS;
}

void testBeginEnd() {
  TEST;

  ObdInterface obd;

  // No observable effects
  obd.begin();
  obd.end();

  PASS;
}

// Tests sending and receiving messages
void testSendReceiveMessage() {
  TEST;

  ObdInterface obd;
  ObdMessage out;
  ObdMessage in;

  obd.setDebug(DEBUG);
  obd.setLoopback(true);
  obd.begin();

  out.address = 0x7e8;
  out.length = 3;
  out.mode = 1;
  out.pid = 2;

  for (int i = 0; i < 5; i++) {
    out.values[i] = i * i;
  }

  // No response without a sent message
  ASSERT(0, !obd.receiveMessage(in));

  // Single message is reflected
  ASSERT(1, obd.sendMessage(out));

  delay(20);

  ASSERT(2, obd.receiveMessage(in));
  ASSERT(3, in.address == out.address);
  ASSERT(4, in.length == out.length);
  ASSERT(5, in.mode == out.mode);
  ASSERT(6, in.pid == out.pid);

  for (int i = 0; i < 5; i++) {
    ASSERT(7 + i, in.values[i] == out.values[i]);
  }

  ASSERT(19, !obd.receiveMessage(in));

  // Filter
  out.address = 42; // Not an ECU, filter applies
  obd.sendMessage(out);
  delay(20);

  ASSERT(21, !obd.receiveMessage(in));

  obd.end();
  obd.setNoFilter(true);
  obd.begin();

  obd.sendMessage(out);
  delay(20);

  ASSERT(21, obd.receiveMessage(in));

  // Short and extended IDs
  out.address = 0x7ff; // Max std address
  obd.sendMessage(out);
  delay(20);
  obd.receiveMessage(in);

  ASSERT(22, in.address == out.address);

  out.address = 0x800; // Invalid std address, seen as zero
  obd.sendMessage(out);
  delay(20);
  obd.receiveMessage(in);

  ASSERT(23, in.address == 0);

  obd.end();
  obd.setExtended(true);
  obd.begin();

  obd.sendMessage(out);
  delay(20);
  obd.receiveMessage(in);

  ASSERT(24, in.address == out.address);

  out.address = 0x1fffffff; // Max ext address
  obd.sendMessage(out);
  delay(20);
  obd.receiveMessage(in);

  ASSERT(25, in.address == out.address);

  out.address = 0x20000000; // Invalid ext address, seen as zero
  obd.sendMessage(out);
  delay(20);
  obd.receiveMessage(in);

  ASSERT(26, in.address == 0);

  PASS;
}

// Tests exchanging messages
void testExchangeMessage() {
  TEST;

  ObdInterface obd;
  ObdMessage out;
  ObdMessage in;

  obd.setDebug(true);
  obd.setLoopback(true);
  obd.begin();

  ASSERT(0, !obd.receiveMessage(in));

  // Single message is reflected
  out.address = 0x7e8;
  out.length = 7;
  out.mode = 1;
  out.pid = 2;
  for (int i = 0; i < 5; i++) {
    out.values[i] = i;
  }

  ASSERT(1, obd.exchangeMessage(out, out, 1000));  
  ASSERT(2, out.address = 0x7e8);
  ASSERT(3, out.length == 7);
  ASSERT(4, out.mode == 0x41);
  ASSERT(5, out.pid == 2);

  for (int i = 0; i < 5; i++) {
    ASSERT(6 + i, out.values[i] == i);
  }

  ASSERT(19, !obd.receiveMessage(in));

  // When using separate 'in' message 'out' stays untouched
  out.mode = 1;

  ASSERT(1, obd.exchangeMessage(out, in, 1000));  
  ASSERT(2, in.address == 0x7e8);
  ASSERT(3, in.length == 7);
  ASSERT(4, in.mode == 0x41);
  ASSERT(5, in.pid == 2);

  for (int i = 0; i < 5; i++) {
    ASSERT(6 + i, in.values[i] == i);
  }

  ASSERT(18, out.mode == 1);

  ASSERT(19, !obd.receiveMessage(in));

  // Non-matching responses are consumed
  out.mode = 2;
  ASSERT(20, obd.sendMessage(out));

  out.mode = 1; 

  ASSERT(21, obd.exchangeMessage(out, in, 1000));  
  ASSERT(22, in.mode == 0x41);

  ASSERT(29, !obd.receiveMessage(in));

  PASS;
}

void testIsPidSupported() {
  TEST;

  ObdInterface obd;
  ObdMessage msg;

  obd.setDebug(true);
  obd.setLoopback(true);
  obd.setNoFilter(true);
  obd.begin();

  msg.values[0] = 0x80; // Set first bit in message

  boolean yes;

  obd.sendMessage(msg);
  obd.sendMessage(msg);
  
  ASSERT(0, obd.isPidSupported(1, yes));
  ASSERT(1, yes);
  
  ASSERT(2, obd.isPidSupported(2, yes));
  ASSERT(3, !yes);

  obd.end();
  obd.setLoopback(false);
  obd.begin();

  ASSERT(4, !obd.isPidSupported(3, yes));
  
  PASS;
}

void testGetPidAsInteger() {
  TEST;

  ObdInterface obd;
  ObdMessage msg;

  obd.setDebug(true);
  obd.setLoopback(true);
  obd.setNoFilter(true);
  obd.begin();

  word value;
  
  msg.length = 3;
  msg.mode = 1;
  msg.pid = 1;
  msg.values[0] = 0x08;
  msg.values[1] = 0x15;

  obd.sendMessage(msg);
  msg.length = 4;
  obd.sendMessage(msg);
  
  ASSERT(0, obd.getPidAsInteger(1, value));
  ASSERT(1, value == 0x08);
  
  ASSERT(2, obd.getPidAsInteger(1, value));
  ASSERT(3, value == 0x0815);
  
  obd.end();
  obd.setLoopback(false);
  obd.begin();

  ASSERT(4, !obd.getPidAsInteger(1, value));
  
  PASS;
}

void testGetPidAsFloat() {
  TEST;

  ObdInterface obd;
  ObdMessage msg;

  obd.setDebug(true);
  obd.setLoopback(true);
  obd.setNoFilter(true);
  obd.begin();

  float value;
  
  msg.length = 3;
  msg.mode = 1;
  msg.pid = 1;
  msg.values[0] = 0x80;

  obd.sendMessage(msg);
  
  ASSERT(0, obd.getPidAsFloat(1, -10.0f, 10.0f, value));
  ASSERT(1, abs(0 - value) < 0.05f);
  
  obd.end();
  obd.setLoopback(false);
  obd.begin();

  ASSERT(4, !obd.getPidAsFloat(1, -10.0f, 10.0f, value));
  
  PASS;
}

void testGetPidAsString() {
  TEST;

  ObdInterface obd;
  ObdMessage msg;

  obd.setDebug(true);
  obd.setLoopback(true);
  obd.setNoFilter(true);
  obd.begin();

  char value[10];
  
  msg.length = 3;
  msg.mode = 1;
  msg.pid = 1;
  msg.values[0] = 0x80;

  obd.sendMessage(msg);
  
  ASSERT(0, obd.getPidAsString(1, 0.0f, 10.0f, "%1.0d green bottles", value));
  ASSERT(1, strcmp("5 green bottles", value) == 0);
  
  obd.end();
  obd.setLoopback(false);
  obd.begin();

  ASSERT(4, !obd.getPidAsString(1, 0.0f, 10.0f, "%f", value));
  
  PASS;
}

void testGetMultiframePid() {
  TEST;

  ObdInterface obd;
  ObdMessage msg;

  obd.setDebug(true);
  obd.setLoopback(true);
  obd.setNoFilter(true);
  obd.begin();

  char value[32];
  int length;
  
  msg.length = 0x07;   // Single frame, length 7 
  msg.mode = 0x47;     // Mode
  msg.pid = 0x11;      // PID
  
  msg.values[0] = 'H';
  msg.values[1] = 'e';
  msg.values[2] = 'l';
  msg.values[3] = 'l';
  msg.values[4] = 'o';

  obd.sendMessage(msg);
  obd.sendMessage(msg);
  
  ASSERT(0, obd.getMultiframePid(0x7, 0x11, NULL, length));
  ASSERT(1, length == 5);

  ASSERT(2, obd.getMultiframePid(0x7, 0x11, value, length));
  value[length] = 0;

  ASSERT(3, strcmp(value, "Hello") == 0);

  obd.end();
  obd.begin();

  msg.length = 0x10;    // First frame
  msg.mode = 13;        // Length 13
  msg.pid = 0x42;       // Mode
  msg.values[0] = 0x11; // PID
  msg.values[1] = 'H';
  msg.values[2] = 'e';
  msg.values[3] = 'l';
  msg.values[4] = 'l';

  obd.sendMessage(msg);

  msg.length = 0x21;    // Consecutive frame
  msg.mode = 'o';
  msg.pid = ' ';
  msg.values[0] = 'W';
  msg.values[1] = 'o';
  msg.values[2] = 'r';
  msg.values[3] = 'l';
  msg.values[4] = 'd';

  obd.sendMessage(msg);
  
  ASSERT(4, obd.getMultiframePid(0x7, 0x11, value, length));
  ASSERT(5, length == 11);
  value[length] = 0;
  ASSERT(6, strcmp(value, "Hello World") == 0);

  ASSERT(7, obd.receiveMessage(msg));   // COnsume our reflected request
  ASSERT(8, msg.length == 2);
  ASSERT(9, obd.receiveMessage(msg));   // Consume flow control frame
  ASSERT(10, msg.length == 0x30);
  ASSERT(11, !obd.receiveMessage(msg)); // No more messages
  
  PASS;
}

#ifdef STRESS
// Tests sending/receiving many messages one-by-one  
void testSendReceiveMessageStress1() {
  TEST;

  ObdInterface obd;
  ObdMessage out;
  ObdMessage in;

  obd.setDebug(true);
  obd.setNoFilter(true);
  obd.setLoopback(true); 
  obd.begin();

  // Bunch of send/receive calls 
  for (int i = 0; i < 1000; i++) {
    int a = 20 * i;

    out.clear();
    out.address = i;
    out.length = 7;
    out.mode = random(0x40);
    out.pid = random(256);

    for (int j = 0; j < 5; j++) {
      out.values[j] = random(256);
    }

    ASSERT(a + 0, !obd.receiveMessage(in));
    ASSERT(a + 1, obd.sendMessage(out));

    delay(20);

    ASSERT(a + 2, obd.receiveMessage(in));

    ASSERT(a + 3, in.address == out.address);
    ASSERT(a + 4, in.length == out.length);
    ASSERT(a + 5, in.mode == out.mode);
    ASSERT(a + 6, in.pid == out.pid);

    for (int k = 0; k < 5; k++) {
      ASSERT(a + 10 + k, in.values[k] == out.values[k]);
    }
  }

  PASS;
}

// Tests sending/receiving many messages in bulk
void testSendReceiveMessageStress2() {
  TEST;

  ObdInterface obd;
  ObdMessage out;
  ObdMessage in;

  obd.setDebug(true);
  obd.setNoFilter(true);
  obd.setLoopback(true);
  obd.begin();

  int a = 0;

  // Bunch of "fill buffer, then empty it"
  for (int i = 0; i < 32; i++) {
    ASSERT(a++, !obd.receiveMessage(in));

    for (int j = 0; j < 32; j++) {
      out.clear();
      out.address = j;
      out.length = 7;
      out.mode = j;
      out.pid = 2 * j;

      for (int k = 0; k < 5; k++) {
        out.values[k] = k;
      }

      ASSERT(a++, obd.sendMessage(out));

      delay(10);
    }

    for (int j = 0; j < 32; j++) {
      ASSERT(a++, obd.receiveMessage(in));

      ASSERT(a++, in.address == j);

      ASSERT(a++, in.length == 7);
      ASSERT(a++, in.mode == j);
      ASSERT(a++, in.pid == 2 * j);

      for (int k = 0; k < 5; k++) {
        ASSERT(a++, in.values[k] == k);
      }
    }
  }

  PASS;
}

// Tests exchanging many messages one-by-one 
void testExchangeMessageStress() {
  TEST;

  ObdInterface obd;
  ObdMessage out;
  ObdMessage in;

  obd.setDebug(true);
  obd.setLoopback(true);
  obd.setNoFilter(true);
  obd.begin();

  // Bunch of execute calls 
  for (int i = 0; i < 1000; i++) {
    int a = 20 * i;

    ASSERT(a + 0, !obd.receiveMessage(in));

    out.clear();
    out.address = random(256);
    out.mode = 0;
    out.length = 7;
    out.pid = random(256);

    for (int j = 0; j < 5; j++) {
      out.values[j] = random(256);
    }

    ASSERT(a + 1, obd.exchangeMessage(out, in, 1000));

    ASSERT(a + 2, in.address == out.address);

    ASSERT(a + 3, in.mode == 0x40);
    ASSERT(a + 4, in.pid == out.pid);
    ASSERT(a + 5, in.length == out.length);

    for (int k = 0; k < 5; k++) {
      ASSERT(a + 10 + k, in.values[k] == out.values[k]);
    }
  }

  PASS;
}
#endif