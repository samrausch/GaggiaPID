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

# 3D printing

I printed everything using Inland PLA+ filament.  Pretty much any material should work fine as the outside of the Gaggia never gets hot enough to melt most thermoplastics.  The included STL files assume you're using the exact same parts I am.  If you use anything different you'll need to make adjustments to the 3D printed parts.

There are no holes in the printed parts for the wiring between the PID and espresso machine, you'll need to decide where you want these holes and drill them yourself.

In it's current revision, the Arduino board is still powered via USB so there needs to be an opening to plug in the USB cable.  This opening is also not included in the STL files as it's easier to locate the hole during the assembly.

# Assembly

Follow the included circuit diagram.  For the wiring inside the PID I just used 32AWG single strand hookup wire and for any wiring inside the Gaggia I used silicone insulated wire (26AWG for the wiring from PID to SSR, 12AWG to extend the thermoswitch leads to the SSR as referenced in the Installation Steps below).  The boiler in the Gaggia can get hot enough to melt non-silicone insulation so if you don't have silicone wiring, make sure to keep any wiring well clear of the boiler.

The Arduino board is secured using a small amount of hot glue.  The MCP9600 board is fastened with a screw to the small ledge inside the 3D printed box.  The OLED and buttons can be secured with your favorite adhesive.  The lid of the enclosure is held by four screws.  I drilled pilot holes for all the screws because my 3D printer has problems printing holes that small.  You will probably want to drill pilot holes as well to prevent cracking the plastic.

My PID is just held on by the clip on the side of the 3D printed box.  Gravity seems to be working as expected in my case but you may decide to attach a magnet to the side of the box that touches the Gaggia for a more secure mount.  If you do this, consider counter-sinking the magnet so that it is flush with the outisde of the box.  This will keep things looking tidy.

# Installation steps:

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

