# Foraging platform
This is the software and hardware designs to produce a low-power datalogger for remote monitoring fruit ripeness. Included in this repository is:

1. arduino code for the datalogger itself
2. schematics and PCB designs for custom, low-power arduino variants in  Eagle and Gerber format
3. Enclosure models in sketchup and .STL format for 3D printing
4. PDF of lids to fit the enclosures that can be laser-cut

###Hardware and software dependencies
The Real-Time Clock used in these files is here: [SparkFun DeadOn RTC Breakout - DS3234](https://www.sparkfun.com/products/10160)

The libraries that these sketches use are the SD card library included with the Arduino IDE, [ManiacBug's fork of RTClib](https://github.com/maniacbug/RTClib) and [Rodan's DS3234 Library](https://github.com/rodan/ds3234). For energy management, [Enerlib](http://playground.arduino.cc/Code/Enerlib) is used.