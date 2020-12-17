This project is copied / forked from here: [https://code.google.com/archive/p/mechanic](https://code.google.com/archive/p/mechanic/)  
The project was created on May 2, 2013 / (c) 2013 Joerg Pleumann

----  
  
# Mechanic  
This library allows you to query diagnostic values and other information from your car using an Arduino and a CAN bus shield. It assumes your car provides an OBD-II interface that supports the CAN bus. This should be true for most recent vehicles, since it has been a standard since 2008. Even some older vehicles do have (possibly reduced) CAN bus support on the ODB-II interface.  
OBD-II, by the way, means "On-board Diagnostics, version II". The OBD-II interface is usually found inside the car, within reach of the driver's seat. The connector is large and almost rectangular. It looks a bit like the SCART connector on older TVs.  
  
Mechanic works fine with the following Arduino boards:  
* Diecimila (ATmega168 and 328P, some sketches too big for 168) 
* Duemilanove (ATmega168 and 328P, some sketches too big for 168) 
* Uno 
* Leonardo 
* Mega (ATmega1280 and 2560)  
  
As a CAN bus shield I recommend the [CANdiy shield](https://github.com/BlackBrix/CANdiy-Shield) from Watterott. This is not only because I had a hand in it's making, but because it offers the greatest flexibility for a really low price. In particular it will work with several different Arduino boards. The Mechanic library does operate fine on Sparkfun's or Seed Studio's CAN shields, too, but at least on the Sparkfun Shield it is limited to the Arduino Diecimila, Duemilanove or Uno, because this shield carries the SPI on pins 11-13 instead of the ICSP header.  
Note that when using the CANdiy shield you **must** have the 2x3 pin ICSP header soldered. Otherwise the Arduino cannot talk to the CAN controller.  
Here are the cable pinouts (RJ45 is for CANdiy, Sub-D is for Sparkfun and Seedstudio). I usually cut a CAT5/6 patch cable at one end and solder the OBD-II connector there.  
  
<table><tbody>
<tr><td><strong>Signal</strong></td><td><strong>OBD-II</strong></td><td><strong>RJ45</strong></td><td><strong>Sub-D</strong></td></tr>
<tr><td>CAN_H</td><td>6</td><td>1</td><td>3</td></tr>
<tr><td>CAN_L</td><td>14</td><td>2</td><td>5</td></tr>
<tr><td>GND</td><td>5</td><td>7</td><td>9</td></tr>
<tr><td>+12V</td><td>16</td><td>8</td><td>2</td></tr>
</tbody></table>  
Installation is easy: Just get the latest release from the downloads page and place its contents into a "Mechanic" directory under your Arduino "libraries" directory. Then restart Arduino. You should now see a bunch of new examples that teach you how to use Mechanic. The "SelfTest" example is a good way of validating your setup (unless your Arduino has an ATmega168, which doesn't have enough RAM for the sketch).  
There are two parameters in the sample sketches that might need to be adapted to your car: CAN bus speed can be 500 kbps or 250 kbps and identifiers can be standard or extended. Most vehicles I have come across seem to use the 500 kbps / standard IDs combination. The following table lists the ones I have tested successfully myself or have been reported to work by users. If your vehicle should be in this table please let me know.  
  
<table><tbody>
<tr><td><strong>Manufacturer</strong></td><td><strong>Model</strong></td><td><strong>Year</strong></td><td><strong>Speed</strong></td><td><strong>Identifiers</strong></td></tr>
<tr><td>Audi</td><td>A4</td><td>2012</td><td>500 kbps</td><td>Standard</td></tr>
<tr><td>Ford</td><td>B-Max</td><td>2013</td><td>500 kbps</td><td>Both</td></tr>
<tr><td>Ford</td><td>Fiesta</td><td>2005</td><td>500 kbps</td><td>Standard</td></tr>
<tr><td>Ford</td><td>Fiesta</td><td>2012</td><td>500 kbps</td><td>Standard</td></tr>
<tr><td>Opel</td><td>Zafira</td><td>2008</td><td>500 kbps</td><td>Standard</td></tr>
<tr><td>Skoda</td><td>Yeti</td><td>2013</td><td>500 kbps</td><td>Standard</td></tr>
<tr><td>Volvo</td><td>V70</td><td>2013</td><td>500 kbps</td><td>Standard</td></tr>
<tr><td>VW</td><td>Caddy</td><td>2011</td><td>500 kbps</td><td>Standard</td></tr>
</tbody></table>   
The repository also contains a small Android app that shows a live dashboard with some values transmitted from Arduino via Bluetooth. I use an inexpensive Bluetooth board from MiB Instruments (around 10 US$, check eBay), but, again, there is no reason why others shouldn't work just as well assuming they provide a serial interface for communicating with the main Arduino board.  
For documentation on the functions I currently recommend to read the comments in the "Mechanic.h" header file. There is also an article in German c't Hardware Hacks magazine 2/2013.  
The library itself is made available under the GNU Lesser General Public License (LGPL). See the LICENSE file for details. All examples except the test suite are licensed under the Creative Commons Zero license, which effectively makes them public domain. The test suite sketch is licensed under the LGPL.  
  
Mechanic uses parts of a CAN library which is (c) 2007 Fabian Greif and published under a BSD-style license.  
Have fun!  
  
# News  
* 2013-10-28: Mechanic version 0.6 available for download. The library now works with more Arduino boards (see list to the left). The updated Android app is able to show the vehicle ID and trouble codes.  
* 2013-02-22: Mechanic featured in German c't Hardware Hacks magazine. This article should give a good introduction to the project.  
* 2013-05-21: Mechanic version 0.5 available for download. This is the first stable release of the library and the companion Android app. You can find some pictures here.  
* 2013-05-05: Project goes online. This is actually a Railuino spin-off using the same hardware.  
# Hardware  
* If you are looking for the [CANdiy Shield](https://github.com/BlackBrix/CANdiy-Shield), Watterott has it (and everything else you need for the project).  

