# GaggiaPID
Open Source PID for Gaggia Classic and Classic Pro espresso machines

As seen on https://www.reddit.com/r/gaggiaclassic/comments/kmm09a/diy_pid_for_my_gaggia_classic/

The .ino file includes several libraries.  Please make sure you've downloaded them prior to uploading the code to your Arduino of choice.  This was built on an Adafruit Metro Mini, which is directly compatible with the Arduino Uno R3, but you can use any Arduino board as long as it has enough pins and it supports I2C.

The code is reasonably well commented internally and variables are named such that it should be easy to find values you may need to change for your specific implementation, such as which pins have buttons assigned to them or the output pin for the SSR.

This design is intended to be as simple as possible in terms of the installation, and requires minimal modification to the Gaggia machine.  You'll need the following parts, as well as basic soldering tools/skills and access to a 3D printer (if you choose to use the included .stl files)

* Arduino board of your choice
* MCP9600 thermocouple amp (https://www.adafruit.com/product/4101)
* M4 threaded K-type thermocouple (widely available from Amazon, etc as a replacement part for 3D printers)
* 128x64 I2C OLED screen (substitute any screen you prefer but you'll need to update the code for your specific unit)
* 3 buttons (I used these: https://www.aliexpress.com/item/32811149954.html?spm=a2g0s.9042311.0.0.49554c4dRzoluB)
* M2x8mm screws of your choise (I used these: https://www.amazon.com/gp/product/B00YBMRAH4/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
* Solid state relay (I used this: https://www.amazon.com/gp/product/B07Y33RZ8F/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
* Rare earth magnets (optional), attached to the back of the SSR to fasten it to the inside of the espresso machine.
* Braided cable sleeve (optional), I used https://www.amazon.com/gp/product/B07QJW9FKS/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

Installation steps:

* THIS ASSUMES YOUR COMPETENT WORKING WITH AC CIRCUITS.  AC CIRCUITS ARE DANGEROUS AND MISTAKES CAN CAUSE INJURY OR DEATH.  YOU ALONE ARE RESPONSIBLE FOR YOUR OWN SAFETY, PLEASE KNOW YOUR LIMITS.
* Remove the top of the machine and locate the boiler thermal switch.  This will be the one at the bottom of the boiler, closest to the group head.
* Remove this thermal switch and replace it with the M4 thermocouple, making sure to add thermal compound if necessary.  The thermocouple should be snug but not too tight as the threads are not super robust.
* Run the TC leads out through the back of the machine and into the PID, attaching to the MCP9600.
* Connect the two leads you removed from the original thermal switch to the switched AC side of the SSR.  You may need to extend the leads depending on where you decide to mount the SSR.  They are not polarized so you can connect either one to either terminal of the SSR.
* Connect the +/- leads from the PID to the SSR input terminals, paying attention to the polarity as these are DC.
* Mount the SSR as desired.
* Optionally, you can add some sleeving to the wires running between the Gaggia and the PID.  If you do this, secure a zip tie to the end of the sleeve inside the machine to keep it from pulling out.
* Test that everything is working before closing the machine back up.

There's a circuit diagram included in the repo.  You're welcome to wire your PID however you like but make sure to change any pin numbers in the code to reflect how you've assembeled the circuit.

The PID has been setup with values for Kp, Ki and Kd that work adequately but you may want to adjust them to suit your needs.  The PID values are not saved if you change them through the menu so if you find a tuning you like and want to change the defaults, you'll need to do that in the code and then re-upload it to the device.

