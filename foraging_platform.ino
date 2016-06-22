#include <SPI.h>
#include <SD.h>           //For SD
#include <Wire.h>
#include <RTClib.h>       //For RTC
#include <RTC_DS3234.h>
#include "Enerlib.h"      //For Power management

#define BUFF_MAX 256

const int chipSelect = 4;

const int timePin = 10;    // RTC select pin
RTC_DS3234 RTC(timePin); 

uint8_t sleep_hour = 23;   // the sleep interval in hours between 2 consecutive alarms
//uint8_t sleep_minute = ;
//uint8_t sleep_day = ;

Energy energy;
int time_interrupt=0;

//////////////////////////////////////////////////////
////FUNCTIONS FROM OTHER RTC LIBRARY TO CLEAN UP
// control register bits
#define DS3234_A1IE     0x1
#define DS3234_A2IE     0x2
#define DS3234_INTCN    0x4

// status register bits
#define DS3234_A1F      0x1
#define DS3234_A2F      0x2
#define DS3234_OSF      0x80

uint8_t dectobcd(const uint8_t val){
    return ((val / 10 * 16) + (val % 10));
}

void DS3234_set_addr(const uint8_t pin, const uint8_t addr, const uint8_t val){
    digitalWrite(pin, LOW);
    SPI.transfer(addr);
    SPI.transfer(val);
    digitalWrite(pin, HIGH);
}

uint8_t DS3234_get_addr(const uint8_t pin, const uint8_t addr){
    uint8_t rv;

    digitalWrite(pin, LOW);
    SPI.transfer(addr);
    rv = SPI.transfer(0x00);
    digitalWrite(pin, HIGH);
    return rv;
}

void DS3234_set_creg(const uint8_t pin, const uint8_t val){
    DS3234_set_addr(pin, 0x8E, val);
}

void DS3234_set_a2(const uint8_t pin, const uint8_t mi, const uint8_t h, const uint8_t d,
                   const uint8_t * flags){
    uint8_t t[3] = { mi, h, d };
    uint8_t i;

    for (i = 0; i <= 2; i++) {
        digitalWrite(pin, LOW);
        SPI.transfer(i + 0x8B);
        if (i == 2) {
            SPI.transfer(dectobcd(t[2]) | (flags[2] << 7) | (flags[3] << 6));
        } else
            SPI.transfer(dectobcd(t[i]) | (flags[i] << 7));
        digitalWrite(pin, HIGH);
    }
}

void DS3234_set_sreg(const uint8_t pin, const uint8_t sreg){
    DS3234_set_addr(pin, 0x8F, sreg);
}

uint8_t DS3234_get_sreg(const uint8_t pin){
    uint8_t rv;
    rv = DS3234_get_addr(pin, 0x0f);
    return rv;
}

void DS3234_clear_a2f(const uint8_t pin){
    uint8_t reg_val;

    reg_val = DS3234_get_sreg(pin) & ~DS3234_A2F;
    DS3234_set_sreg(pin, reg_val);
}

// END WEIRD FUNCTION DUMP
//////////////////////////////////////////////////////

void set_next_alarm(void){
    DateTime now = RTC.now();
    unsigned char wakeup_min;
    unsigned char wakeup_hour;
    
    // calculate the minute when the next alarm will be triggered
    wakeup_min = now.minute();

    //calculate the hour when the next alarm will be triggered
    wakeup_hour = now.hour() + sleep_hour;
    if (wakeup_hour > 23) {
        wakeup_hour -= 24;
    }

    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm
    // A2M2 (minutes) (0 to enable, 1 to disable)
    // A2M3 (hour)    (0 to enable, 1 to disable) 
    // A2M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    uint8_t flags[4] = { 0, 0, 1, 1 };

    // set Alarm2. only the minute is set since we ignore the hour and day component
    DS3234_set_a2(timePin, wakeup_min, wakeup_hour, 0, flags);

    // activate Alarm2
    DS3234_set_creg(timePin, DS3234_INTCN | DS3234_A2IE);  
}

void INT0_ISR(void){
  //detach interrupt and set time_interrupt=1
  //interrupt must be attached again
  detachInterrupt(0);
  time_interrupt=1;
}

void logSensorReading() {
    // make a string for assembling the data to log:
  String dataString = "";
  
  // get current date/time and append to string
  const int len = 32;
  static char buf[len];

  DateTime now = RTC.now();
  dataString += now.toString(buf,len);

  // read three sensors and append to the string:
  int analogPin = 0;
  int sensor = analogRead(analogPin);
  dataString += ", "+String(sensor);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

void setup(){
    Serial.begin(9600);
    Serial.println("Entering setup");

    SPI.begin();
    RTC.begin();
    
    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      return;
    } else {
      Serial.print("Setting time to... ");
      Serial.print(__DATE__);
      Serial.print(' ');
      Serial.println(__TIME__);
      // following line sets the RTC to the date & time this sketch was compiled
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }
    
    Serial.print("Initializing SD card...");
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      return;
    }
    Serial.println("card initialized.");

    pinMode(2, INPUT);
    delay(5000);

    DS3234_clear_a2f(timePin);
    set_next_alarm();
    attachInterrupt(0, INT0_ISR, LOW);
}

void loop(){
  if(time_interrupt==1){
    Serial.println(" time_interrupt==1");
    time_interrupt=0;
    
    // set next alarm
    set_next_alarm();
    // clear a2 alarm flag and let INT go into hi-z
    DS3234_clear_a2f(timePin);
    //Attach interrupt again
    attachInterrupt(0, INT0_ISR, LOW);
    
    if (energy.WasSleeping()){
        Serial.println("  Interrupt and was sleeping");
        logSensorReading();
    }
    else
    {
        /*
        The IRQ happened in awake state.
        This code is for the "normal" ISR.
        */
        Serial.println("  Interrupt and was NOT sleeping");
    }
  }
  Serial.println("Entering loop");
  delay(1000);

  Serial.println("Powering down");
  energy.PowerDown();
}
