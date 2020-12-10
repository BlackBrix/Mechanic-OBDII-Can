# Mechanic

 This library allows you to query diagnostic values and other information<br>

 from your car using an<br>

 Arduino and a<br>

 CAN bus shield. It assumes your car<br>

 provides an<br>

 OBD-II interface that supports the CAN bus. This should be<br>

 true for most recent vehicles, since it has been a standard since 2008.<br>

 Even some older vehicles do have (possibly reduced) CAN bus support on the<br>

 ODB-II interface.<br>

<br>

 OBD-II, by the way, means "On-board Diagnostics, version II". The OBD-II<br>

 interface is usually found inside the car, within reach of the driver's<br>

 seat. The connector is large and almost rectangular. It looks a bit like<br>

 the SCART connector on older TVs.<br>

<br>

 Mechanic works fine with the following Arduino boards:<br>

<br>

- Diecimila (ATmega168 and 328P, some sketches too big for 168)<br>

- Duemilanove (ATmega168 and 328P, some sketches too big for 168)<br>

- Uno<br>

- Leonardo<br>

- Mega (ATmega1280 and 2560)

<!-- -->

 As a CAN bus shield I recommend the<br>

 CANdiy shield from Watterott.<br>

 This is<br>

 not only because I had a hand in it's making, but because it offers the<br>

 greatest flexibility for a really low price. In particular it will work<br>

 with several different Arduino boards. The Mechanic library does operate<br>

 fine on<br>

 Sparkfun's or<br>

 Seed Studio's CAN shields, too, but at least on the Sparkfun<br>

 Shield it is limited to the Arduino Diecimila, Duemilanove or Uno, because<br>

 this shield carries the SPI on pins 11-13 instead of the ICSP header.<br>

<br>

 Note that when using the CANdiy shield you **must** have the 2x3 pin ICSP<br>

 header soldered. Otherwise the Arduino cannot talk to the CAN controller.<br>

<br>

 Here are the cable pinouts (RJ45 is for CANdiy, Sub-D is for Sparkfun and<br>

 Seedstudio). I usually cut a CAT5/6 patch cable at one end and solder the<br>

 OBD-II connector there.<br>

<br>

**Signal** **OBD-II****RJ-45****Sub-D** CAN\_H 6 1 3 CAN\_L 14 2 5 GND 5 7 9 +12V 16 8 2 Installation is easy: Just get the latest release from the downloads page<br>

 and place its contents into a "Mechanic" directory under your Arduino<br>

 "libraries" directory. Then restart Arduino. You should now see a bunch of<br>

 new examples that teach you how to use Mechanic. The "SelfTest"<br>

 example is a good way of validating your setup (unless your Arduino has an<br>

 ATmega168, which doesn't have enough RAM for the sketch).<br>

<br>

 There are two parameters in the sample sketches that might need to be<br>

 adapted to your car: CAN bus speed can be 500 kbps or 250 kbps and<br>

 identifiers can be standard or extended. Most vehicles I have come across<br>

 seem to use the 500 kbps / standard IDs combination. The following table<br>

 lists the ones I have tested successfully myself or have been reported to<br>

 work by users. If your vehicle should be in this table please let me know.<br>

<br>

**Manufacturer****Model****Year****Speed****Identifiers** Audi A4 2012 500 kbps Standard Ford B-Max 2013 500 kbps Both Ford Fiesta 2005 500 kbps Standard Ford Fiesta 2012 500 kbps Standard Opel Zafira 2008 500 kbps Standard Skoda Yeti 2013 500 kbps Standard Volvo V70 2013 500 kbps Standard VW Caddy 2011 500 kbps Standard The repository also contains a small Android app that shows a live<br>

 dashboard with some values transmitted from Arduino via Bluetooth. I use<br>

 an inexpensive Bluetooth board from<br>

 MiB Instruments (around 10 US$, check<br>

 eBay), but, again, there is no reason why others shouldn't work just as<br>

 well assuming they provide a serial interface for communicating with the<br>

 main Arduino board.<br>

<br>

 For documentation on the functions I currently recommend to read the<br>

 comments in the "Mechanic.h" header file. There is also an article in<br>

 German<br>

 c't Hardware Hacks magazine 2/2013.<br>

<br>

 The library itself is made available under the GNU Lesser General Public<br>

 License (LGPL). See the LICENSE file for details. All examples except the<br>

 test suite are licensed under the Creative Commons Zero license, which<br>

 effectively makes them public domain. The test suite sketch is licensed<br>

 under the LGPL.<br>

<br>

<br>

 Mechanic uses parts of a<br>

 CAN library which is (c) 2007 Fabian Greif and published under a BSD-style license.<br>

<br>

 Have fun!<br>

<br>

# News

- 2013-10-28: Mechanic version 0.6 available for download. The library now works with more Arduino boards (see list to the left). The updated Android app is able to show the vehicle ID and trouble codes.<br>

- 2013-02-22: Mechanic featured in German c't Hardware Hacks magazine. This article should give a good introduction to the project.<br>

- 2013-05-21: Mechanic version 0.5 available for download. This is the first stable release of the library and the companion Android app. You can find some pictures here.
- 2013-05-05: Project goes online. This is actually a Railuino spin-off using the same hardware.

<!-- -->

# Hardware

- If you are looking for the CANdiy Shield, Watterott has it (and everything else you need for the project).<br>


<!-- -->

