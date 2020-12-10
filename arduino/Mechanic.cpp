/*********************************************************************
 * Mechanic - Hacking your car
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

// #include <can.h>
#include "Mechanic.h"
// #include "canbus/canbus.c"
#include "can/mcp2515.h"
#include "can/mcp2515.c"

typedef tCAN can_t;

size_t printHex(Print &p, unsigned long hex, int digits) {
    size_t size = 0;

    String s = String(hex, HEX);

    for (int i = s.length(); i < digits; i++) {
        size += p.print("0");
    }

    size += p.print(s);

    return size;
}

#define SIZE 64

#define ulong unsigned long

can_t _buffer[SIZE];

volatile int posRead = 0;

volatile int posWrite = 0;

volatile boolean lastOpWasWrite = false;

void enqueue() {
    noInterrupts();
    
	if (posWrite == posRead && lastOpWasWrite) {
		// Serial.println("!?! Buffer full");
		interrupts();
		return;
	}

	if (mcp2515_check_message() && mcp2515_get_message(&_buffer[posWrite])) {
//	    if (_buffer[posWrite].id < 0x700) {
//	       interrupts();
//          return;
//	    }
		posWrite = (posWrite + 1) % SIZE;
	} else {
           interrupts();
           return;
		// Serial.println("!?! No message");
	}

	lastOpWasWrite = true;
    interrupts();
}

boolean dequeue(can_t *p) {
	noInterrupts();

	if (posWrite == posRead && !lastOpWasWrite) {
		interrupts();
		return false;
	}

	memcpy(p, &_buffer[posRead], sizeof(can_t));
/*
	p->id=_buffer[posRead].id;
	p->header.length=_buffer[posRead].length;

	for (int i = 0; i < p->header.length; i++) {
		p->data[i] = _buffer[posRead].data[i];
	}
*/
	//*p = _buffer[posRead];

	posRead = (posRead + 1) % SIZE;
	lastOpWasWrite = false;

	interrupts();

	return true;
}

// ===================================================================
// === ObdMessage ==================================================
// ===================================================================

ObdMessage::ObdMessage() {
    clear();
}

void ObdMessage::clear() {
	address = 0x7df;
	length = 2;
	mode = 1;
	pid = 0;
	for (int i = 0; i < 5; i++) {
		values[i] = 0x00;
	}
}

size_t ObdMessage::printTo(Print& p) const {
    size_t size = 0;

    size += printHex(p, address, 8);
    size += p.print(" ");
    size += printHex(p, length, 2);
    size += p.print(" ");
    size += printHex(p, mode, 2);
    size += p.print(" ");
    size += printHex(p, pid, 2);

    for (int i = 0; i < 5; i++) {
        size += p.print(" ");
        size += printHex(p, values[i], 2);
    }

    return size;
}

int ObdMessage::getIntValue() {
  return length < 4 ? values[0] : word(values[0], values[1]);
}

float ObdMessage::getFloatValue(float min, float max) {
    float value = values[0];
    boolean wide = length >= 4;
    
    if (wide) {
      value = value * 256.0f + (float)values[1];
    }
    
    value = min + value * (max - min) /
            (!wide ? 255.0f : 65535.0f);
            
    return value;
}


// ===================================================================
// === ObdInterface ===============================================
// ===================================================================

#define CANSPEED_125    7       // CAN speed at 125 kbps
#define CANSPEED_250    3       // CAN speed at 250 kbps
#define CANSPEED_500    1       // CAN speed at 500 kbps

ObdInterface::ObdInterface() {
    mSlow = false;
    mExtended = false;
    mDebug = false;
    mNoFilter = false;
    mLoopback = false;
}

ObdInterface::~ObdInterface() {
	end();
}

boolean ObdInterface::isSlow() {
	return mSlow;
}

void ObdInterface::setSlow(boolean slow) {
    mSlow = slow;
}

boolean ObdInterface::isExtended() {
    return mExtended;
}

void ObdInterface::setExtended(boolean extended) {
    mExtended = extended;
}

boolean ObdInterface::isDebug() {
	return mDebug;
}

void ObdInterface::setDebug(boolean debug) {
    mDebug = debug;
}

boolean ObdInterface::isLoopback() {
	return mLoopback;
}

void ObdInterface::setLoopback(boolean loopback) {
    mLoopback = loopback;
}

boolean ObdInterface::isNoFilter() {
    return mNoFilter;
}

void ObdInterface::setNoFilter(boolean noFilter) {
    mNoFilter = noFilter;
}

void ObdInterface::begin() {
    // Even if we don't use the real SS pin
    // on all boards, it must be set to out,
    // otherwise SPI might switch to slave
    // and we just hang. Do not delete!
    pinMode(SS, OUTPUT);

	attachInterrupt(CAN_INT, enqueue, FALLING);

	if (!mcp2515_init(mSlow ? CANSPEED_250 : CANSPEED_500, mExtended, !mNoFilter, mLoopback)) {
	    if (mDebug) {
            Serial.println(F("!?! Init error"));
            Serial.println(F("!?! Emergency stop"));
		}
		for (;;);
	}

    delay(50);
}

// end - no interrupts

void ObdInterface::end() {
	detachInterrupt(CAN_INT);

	can_t t;

	boolean b = dequeue(&t);
	while (b) {
		b = dequeue(&t);
	}
}

boolean ObdInterface::sendMessage(ObdMessage &message) {
	can_t can;

	can.id = message.address;
	
	can.flags.extended = mExtended ? 1 : 0;
	can.flags.rtr = 0;
	can.length = 8;
	
    can.data[0] = message.length;
    can.data[1] = message.mode;
    can.data[2] = message.pid;
    
	for (int i = 0; i < 5; i++) {
		can.data[3 + i] = message.values[i];
	}
	
	if (mDebug) {
	    Serial.print("<== ");
	    Serial.println(message);
/*
            Serial.print("<<< ");
            Serial.print(can.id, HEX);
            Serial.print(" ");
            Serial.print(can.flags.rtr, HEX);
            Serial.print(" ");
           Serial.print(can.flags.extended, HEX);
            Serial.print(" ");
            Serial.print(can.length, HEX);
            Serial.print(" ");
            for (int i = 0; i < 8; i++) {
                Serial.print(can.data[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
*/
	}
	
	noInterrupts();
	// can_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
	boolean b = mcp2515_send_message(&can);
	interrupts();
	
	return b;
}

boolean ObdInterface::receiveMessage(ObdMessage &message) {
	can_t can;

	boolean result = dequeue(&can); // mcp2515_get_message(&can);

	if (result) {
		message.clear();
		message.address = can.id;
		message.length = can.data[0];
        message.mode = can.data[1];
        message.pid = can.data[2];
        
		for (int i = 0; i < 5; i++) {
			message.values[i] = can.data[3 + i];
		}

		if (mDebug) {
		    /*
            Serial.print(">>> ");
            Serial.print(can.id, HEX);
            Serial.print(" ");
            Serial.print(can.flags.rtr, HEX);
            Serial.print(" ");
            Serial.print(can.flags.extended, HEX);
            Serial.print(" ");
            Serial.print(can.length, HEX);
            Serial.print(" ");
            for (int i = 0; i < 8; i++) {
                Serial.print(can.data[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
            */
		    Serial.print("==> ");
		    Serial.println(message);
		    
		}
	}

	return result;
}

boolean ObdInterface::exchangeMessage(ObdMessage &out, ObdMessage &in, word timeout) {
    byte mode = out.mode;
	byte pid = out.pid;

	if (!sendMessage(out)) {
	    return false;
	}

	ulong time = millis();

	while (millis() < time + timeout) {
		in.clear();
		boolean result = receiveMessage(in);
		
		if (mLoopback) {
		  in.mode = in.mode + 0x40;
        }

		if (result && in.mode == mode + 0x40 && in.pid == pid) {
			return true;
		}
	}

	if (mDebug) {
		Serial.println(F("!?! Receive timeout"));
	}
	
	return false;
}

boolean ObdInterface::isPidSupported(int pid, boolean &value) {
  if (pid == 0) {
    value = true;
    return true;
  }
  
  pid--;
  
  ObdMessage msg;
  msg.pid = pid & 0xe0;
  pid = pid & 0x1f;
  
  if (exchangeMessage(msg, msg, 2000)) {
    value = bitRead(msg.values[pid / 8], 7 - (pid & 0x07)); 
    return true;
  } else {
    return false;
  }
}

boolean ObdInterface::getPidAsInteger(int pid, word &value) {
  ObdMessage msg;
  msg.pid = pid;
  
  if (exchangeMessage(msg, msg, 2000)) {
    value = msg.getIntValue();
    return true;
  } else {
    return false;
  }
}

boolean ObdInterface::getPidAsFloat(int pid, float min, float max, float &value) {
  ObdMessage msg;
  msg.pid = pid;
  
  if (exchangeMessage(msg, msg, 2000)) {
    value = msg.getFloatValue(min, max);
    return true;
  } else {
    return false;
  }
}

boolean ObdInterface::getPidAsString(int pid, float min, float max, char *format, char *buffer) {
  float value;
  if (getPidAsFloat(pid, min, max, value)) {
    int i = (int)value;
    sprintf(buffer, format, i);
    return true;
  } else {
    strcpy(buffer, "n/a");
    return false;
  }
}

boolean ObdInterface::getMultiframePid(int mode, int pid, char *buffer, int &count) {
  count = 0;
  
  ObdMessage msg;
  msg.mode = mode;
  if (pid == -1) {
    msg.length = 1;
  } else {
    msg.pid = pid;
  }
  
  if (!sendMessage(msg)) {
    return false;
  }

  while(!receiveMessage(msg)) {
    delay(20);
  }
    
  int length = msg.length & 0x0f;
  int type = msg.length >> 4;
      
  if(type == 0) {
    if (msg.mode == mode + 0x40 && (pid == -1 || msg.pid == pid)) {
      if (buffer == NULL) {
        count = length - 2;
        // TODO Cancel transmission here ???
        return true;
      }
    
      for (int i = 0; i < length - 2; i++) {
        buffer[count++] = msg.values[i];
      }
        
      return true;
    }
    // Else what?
  } else if (type == 1) {
    int total = (length << 8) | msg.mode;

    // Check mode, pid
    
    //Serial.print("Total size: ");
    //Serial.println(total, DEC);

    if (buffer == NULL) {
      count = total - 2;
      // TODO Cancel transmission here ???
      return true;
    }

    for (int i = 1; i < 5; i++) {
      buffer[count++] = msg.values[i];
    }
          
    while (count < total - 3) {
      //Serial.print("Count: ");
      //Serial.println(count, DEC);
    
      msg.address = 0x7df;
      msg.length = 0x30;
      msg.mode = 1;
      msg.pid = 0;

      for (int i = 0; i < 5; i++) {
        msg.values[i] = 0;
      }
              
      sendMessage(msg);

      while(!receiveMessage(msg)) {
        delay(20);
      }

      int index = msg.length & 0x0f;
      type = msg.length >> 4;
      //Serial.print("Index: ");
      //Serial.println(index, DEC);
      
      if (type == 2) {
        // TODO check sequence number here 
        buffer[count++] = msg.mode;
        if (count < total) {
          buffer[count++] = msg.pid;

          int remain = total - count;
          if (remain > 5) {
            remain = 5;
          }

          for (int i = 0; i < remain; i++) {
            buffer[count++] = msg.values[i];
          }
        }
      }
    }
    
    return true;
  }
  
  // TODO bail
  
  return false;
}
