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

#ifndef obd__h
#define obd__h

#include <Arduino.h>
#include <Printable.h>

// ===================================================================
// === Common definitions ============================================
// ===================================================================

/**
 * Version of OBD library.
 */
#define MECHANIC_VERSION "0.6"

/**
 * Represents a message going through the OBD interface. More or
 * less a beautified version of the real CAN message. You normally
 * don't need to use this unless you want to experiment with the
 * protocol or extend the library. See the OBD spec for details.
 * The ObdMessage is a Printable, so it can be directly used in
 * Serial.println(), for instance.
 */
class ObdMessage : public Printable {

  public:

  /**
   * The address of the ECU we are talking to. Defaults to 0x7df,
   * which is the standard ECU broadcast address. See OBD spec for
   * details.
   */
  uint32_t address;
  
  /**
   * The length of the message including the two bytes for mode
   * and PID.
   */
  byte length;
  
  /**
   * The request/response mode. Defaults to 1, which means request
   * current values. See OBD spec for details.
   */
  byte mode;
  
  /**
   * The request/response PID (parameter ID). See OBD spec for
   * details.
   */
  byte pid;
  
  /**
   * The (up to five) bytes of value payload our message may
   * carry.
   */
  byte values[5];

  ObdMessage();
  
  /**
   * Clears the message, setting all values to defaults. Provides
   * for easy recycling of ObdMessage objects.
   */
  void clear();

  /**
   * Prints the message to the given Print object, which could be a
   * Serial object, for instance. The message format looks like this
   *
   * ADDR L MO PI V0 V1 V2 V3
   *
   * with all numbers being hexadecimals and the data bytes being
   * optional beyond what the message length specifies. Exactly one
   * whitespace is inserted between different fields as a separator.
   */
  virtual size_t printTo(Print &p) const;
  
  int getIntValue();

  float getFloatValue(float min, float max);

  // Common getters...

};

// ===================================================================
// === ObdInterface ==================================================
// ===================================================================

/**
 * Encapsulates the communication with the real OBD interface via the
 * CAN bus. While there are some low-level methods for dealing with
 * messages, you will normally want to use the high-level methods that
 * wrap most of the the nasty protocol details.
 */
class ObdInterface {

    private:

	/**
	 * Stores the speed used on the CAN interface. Must be one of the
	 * constants defined above, that is, 250 kbps and 500 kbps are
	 * allowed.
	 */
	boolean mSlow;

    /**
     * Stores the extended identifier flag. When this is true, all
     * outgoing messages use extended identifiers. Some cars seem to
     * expect this.
     */
    boolean mExtended;

	/**
	 * Stores the debug flag. When debugging is on, all outgoing and
	 * incoming messages are printed to the Serial console.
	 */
	boolean mDebug;
	
	/**
	 * Stores the loopback flag. When loopback is on, messages are
	 * reflected by the CAN controller. No external communication
	 * takes place. This is helpful for some test cases.
	 */
	boolean mLoopback;

    /**
     * Reflects whether the CAN filter is disabled. Default is
     * filter enabled, because there is so much traffic on the
     * bus that our little Arduino has trouble handling all
     * messages.
     */
    boolean mNoFilter;
    
    public:

	/**
	 * Creates a new ObdInterface with default values. This should
	 * be fine for most use cases. Further configuration can be done
	 * by using the init() method.
	 */
    ObdInterface();

    /**
     * Is called when an ObdInterface is being destroyed. Does the
     * necessary cleanup. No need to call this manually.
     */
    ~ObdInterface();

    /**
     * Queries the speed used by the ObdInterface. Default is 500 kbps,
     * slow means 250 kbps.
     */
    boolean isSlow();

    /**
     * Sets the speed used by the ObdInterface. Default is 500 kbps,
     * slow means 250 kbps.
     */
    void setSlow(boolean slowSpeed);
    
    /**
     * Reflects whether the ObdInterface uses extended identifiers
     * for outgoing messages.
     */
    boolean isExtended();
    
    /**
     * Sets the type of CAN identifiers used. Default is 11 bits
     * (CAN 2.0a), extended means 29 bits (CAN 2.0b).
     */
    void setExtended(boolean extended);
    
    /**
     * Reflects whether the ObdInterface is in debug mode,
     * where all messages are dumped to the Serial console.
     */
    boolean isDebug();
    
    /**
     * Controls whether the ObdInterface is in debug mode,
     * where all messages are dumped to the Serial console.
     */
    void setDebug(boolean debug);

    /**
     * Returns true if the ObdInterface is in loopback mode,
     * where all messages are reflected by the CAN controller.
     */
    boolean isLoopback();
    
    /**
     * Controls loopback mode of the CAN controller.
     */
    void setLoopback(boolean loopback);

    /**
     * Returns true if CAN filtering is disabled.
     */    
    boolean isNoFilter();
    
    /**
     * Controls CAN filtering. The default is false, so filtering
     * is normally enabled.
     */
    void setNoFilter(boolean noFilter);

    /**
     * Sends a message and reports true on success. Internal method.
     * Normally you don't want to use this, but the more convenient
     * methods below instead.
     */
    boolean sendMessage(ObdMessage &message);

    /**
     * Receives an arbitrary message, if available, and reports true
     * on success. Does not block. Internal method. Normally you
     * don't want to use this, but the more convenient methods below
     * instead.
     */
    boolean receiveMessage(ObdMessage &message);

    /**
     * Sends a message and waits for the corresponding response,
     * returning true on success. Blocks until either a message with
     * the correct response mode (i.e. request mode + 0x40) arrives or
     * the timeout (in ms) expires. All non-matching messages are
     * discareded. Internal method. Normally you don't want to use this,
     * but the more convenient methods below instead. 'out' and 'in'
     * may be the same object.
     */
    boolean exchangeMessage(ObdMessage &out, ObdMessage &in, word timeout);

    /**
     * Initializes the CAN hardware and starts receiving CAN
     * messages. CAN messages are put into an internal buffer of
     * limited size, so they don't get lost, but you have to take
     * care of them in time. Otherwise the buffer might overflow.
     */
    void begin();

    /**
     * Stops receiving messages from the CAN hardware. Clears the
     * internal buffer.
     */
    void end();

    /**
     * Queries whether the given mode 0x01 PID is supported. Returns
     * true if the query is successful and false otherwise. In case
     * of a successful query the desired result is stored in the
     * value variable.
     */
    boolean isPidSupported(int pid, boolean &value);
    
    /**
     * Queries the raw integer value of the given mode 0x01 PID.
     * Returns true if the query is successful and false otherwise.
     * In case of a successful query the desired result is stored
     * in the value variable. The method can handle 8 and 16 bit
     * answers automatically (based on the length of the response). 
     */
    boolean getPidAsInteger(int pid, word &value);
    
    /**
     * Queries the floating point value of the given mode 0x01 PID.
     * Returns true if the query is successful and false otherwise.
     * In case of a successful query the desired result is scaled
     * based upon the given range and the actual length of the
     * response (8 or 16 bits). The result is stored in the value
     * variable.
     */
    boolean getPidAsFloat(int pid, float min, float max, float &value);
    
    /**
     * Queries the floating point value of the given mode 0x01 PID
     * and pretty-prints it. Returns true if the query is successful
     * and false otherwise. In case of a successful query the result
     * value is scaled based upon the given range and printed into
     * the given buffer, using the given format (using sprintf).
     */
    boolean getPidAsString(int pid, float min, float max, char *format, char *buffer);

    /**
     * Queries a multiframe PID based on ISO 15765-2, also known as
     * ISO-TP. Returns true on success and false otherwise. In case of
     * a successful call the result is written into the given buffer,
     * and count holds the number of bytes. The caller has to ensure
     * that the buffer is large enough. If the buffer is null the method
     * will only return the required number of bytes, but not make any
     * attempts to store the actual data. This can be used for finding
     * out the required buffer size in advance. Pass a PID value of -1
     * to query a service without PID, such as the DTCs.
     */
    boolean getMultiframePid(int mode, int pid, char *buffer, int &count);

};

#endif
