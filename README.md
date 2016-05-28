# Analog HDD Persistence-of-Vision Clock

![Analog HDD POV Clock](/presentation/pictogram.png?raw=true "Analog HDD POV Clock")

## General description

An HDD Persistence-of-Vision Clock is a device that makes the fast spinning platter of an old hard drive look like the face of an analog clock by multiplexing RGB LEDs located under the platter!


## What you need to build one yourself

1. At least one old hard drive (capacity doesn't matter)
2. RGB-LED-Strip (not one of those programmable ones, they're too slow!)
3. Brushless motor driver (also called ESC, any type and amperage rating will do)
4. Arduino microcontroller (preferably a small type like the Nano)
5. Hall sensor (or an infrared light barrier alternatively)
6. Real time clock chip (like the DS3231)
7. (optional: 4 digit 7 segment display)

And also lots of wires, resistors, transistors and such things...


## Build steps
### Step 01: Taking hard drives apart and salvaging all parts needed

![Step 01: Salvaging Parts](/presentation/step_01.jpg?raw=true "Step 01: Salvaging Parts")


### Step 02: Cutting a slot into HDD disk and putting a magnet in place

![Step 02: HDD Disk](/presentation/step_02.jpg?raw=true "Step 02: HDD Disk")


### Step 03: Putting LED-Strip and Hall Sensor in place

![Step 03: LEDs and Hall Sensor](/presentation/step_03.jpg?raw=true "Step 03: LEDs and Hall Sensor")


### Step 04: Testing functionality of all components

![Step 04: Component Testing](/presentation/step_04.jpg?raw=true "Step 04: Component Testing")


### Step 05: Check how your 7 segment display works

![Step 05: 7 Segment Display](/presentation/step_05.jpg?raw=true "Step 05: 7 Segment Display")


### Step 06: Solder everything on a perfboard

![Step 06: Perfboard](/presentation/step_06.jpg?raw=true "Step 06: Perfboard")


### Step 07: Find the problems

![Step 07: Problems](/presentation/step_07.jpg?raw=true "Step 07: Problems")


### Step 08: Try displaying a still image for the first time

![Step 08: First Still Image](/presentation/step_08.jpg?raw=true "Step 08: First Still Image")


### Step 09: Get a copy of the software from this repo and adjust it to your own needs

### Step 10: Enjoy your own HDD POV Clock!!


## Library dependencies

* [TimerOne library](http://playground.arduino.cc/Code/Timer1) by [Paul Stoffregen](https://github.com/PaulStoffregen/TimerOne)
* [RTCLib](https://github.com/adafruit/RTClib) by [adafruit](https://www.adafruit.com/)  
 with temperature readout functionality obtained from [here](https://github.com/adafruit/RTClib/pull/62/commits/c3d2d11fd4d092d392c2875d3d5d4745274d3f8d)

