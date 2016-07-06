#Foraging platform

![Installation](https://cloud.githubusercontent.com/assets/16662/16592091/fc493396-42ac-11e6-9078-3b756eb5492a.png)

This repository contains the software and hardware designs to produce a low-power datalogger for remote monitoring fruit ripeness. Included in this repository is:

1. Arduino code for the datalogger itself
2. schematics and PCB designs for custom, low-power arduino variants in  Eagle and Gerber format
3. Enclosure models in sketchup and .STL format for 3D printing
4. PDF of lids to fit the enclosures that can be laser-cut

###Goals
This platform is meant to be deployed autonomously on fruit trees while their fruit ripens. It offers a way for urban foragers to monitor remotely many trees, and lessen their reliance on volunteers or manually checking the fruit status on trees across large distances over long durations. It uses two different kinds of sensors to monitor the fruit’s ripeness. The first is a bend sensor attached to a branch that will bear fruit to represent the ripeness of the tree. In the second, a stretch sensor is placed between two branches. In each case, as the fruit grows, the branch bearing that fruit bends, providing a changing reading to our sensors.

###Current Iteration
Right now, the platform uses a real-time clock module to wake a custom Arduino board from a deep, energy-saving sleep mode and then logs the current bend data to a MicroSD card. In this configuration, the board has lasted over a month deployed in a tree during Summer 2015, reading every 12 hours. Decreasing the frequency of readings should enable much longer deployment times. Currently (Summer 2016) we are testing new sensors reading every 23 hours.

###Photos

![photo1](https://cloud.githubusercontent.com/assets/16662/16592089/fc46bf3a-42ac-11e6-8d8d-63cc52290c31.png)

![photo2](https://cloud.githubusercontent.com/assets/16662/16592088/fc459880-42ac-11e6-8457-b8b0ef2db25a.png)

![photo3](https://cloud.githubusercontent.com/assets/16662/16592090/fc486c5e-42ac-11e6-83a2-faa7677220a8.png)

###Technical Details
[Bill of Materials](https://docs.google.com/spreadsheets/d/1Vs3_GVGOHFVuYa3o1AHFbKYVgtYUHnmqFQ5do5olbm8/edit#gid=990807043) - includes notes for datalogging, WiFi and cellular versions

#####Hardware and software dependencies
The Real-Time Clock used in these files is here: [SparkFun DeadOn RTC Breakout - DS3234](https://www.sparkfun.com/products/10160)

The libraries that these sketches use are the SD card library included with the Arduino IDE, [ManiacBug's fork of RTClib](https://github.com/maniacbug/RTClib) and [Rodan's DS3234 Library](https://github.com/rodan/ds3234). For energy management, [Enerlib](http://playground.arduino.cc/Code/Enerlib) is used.

#####Potential future comm/sensor choices
Cell phone breakout board and pay-as-you-go SIM card options:

* https://www.adafruit.com/products/1963
* https://www.adafruit.com/products/1946

Inexpensive WiFi breakout board:

* https://www.sparkfun.com/products/13678

21 Sensors under $2:

* http://randomnerdtutorials.com/21-arduino-modules-you-can-buy-for-less-than-2/

###Problems and Issues
#####Accuracy Issues
There are a couple of obvious issues with this setup. The first is that the weight of fruit is the proxy for ripeness. This is not as much of a problem as it could be, because knowing when fruit gets heavy means that volunteer resources could be used to check on it for more detailed results. Additionally, we are experimenting with combinations of gas sensors to more properly detect ripeness itself.
#####Technical Issues
By far the most limiting issues we’ve come across have been technical. Detecting a bend is not very difficult, and building out a low-power sensor board as well as the industrial design planning to deploy the platform has been relatively straightforward. However, the initial vision of a wireless, long-term deployment has been difficult. Even sensing and sending bend every 24 hours doesn’t come very close to the long-term deployment envisioned.