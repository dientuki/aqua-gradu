/**
 * Author:Ab Kurk
 * version: 1.0
 * date: 4/02/2018
 * Description: 
 * This sketch is part of the guide to putting your Arduino to sleep
 * tutorial. We use the:
 * DS3231 RTC
 * Arduino Pro Mini
 * In this example we use the RTC to wake up the Arduino.
 * After the data has been logged the Arduino goes back to sleep and gets woken up 5 minutes later to 
 * start all over again
 * Link To Tutorial http://www.thearduinomakerman.info/blog/2018/1/24/guide-to-arduino-sleep-mode
 * Link To Project   http://www.thearduinomakerman.info/blog/2018/2/5/wakeup-rtc-datalogger
 */

#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#define interruptPin 2 //Pin we are going to use to wake up the Arduino
#include <DS3232RTC.h>  //RTC Library https://github.com/JChristensen/DS3232RTC
//RTC Module global variables
const int time_interval=1;// Sets the wakeup intervall in minutes

void setup() {
  Serial.begin(9600);//Start Serial Comunication
  pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indecate when Arduino is A sleep
  pinMode(interruptPin,INPUT_PULLUP);//Set pin d2 to input using the buildin pullup resistor
  digitalWrite(LED_BUILTIN,HIGH);//turning LED on
  
  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);  
  /*
  * Uncomment the block block to set the time on your RTC. Remember to comment it again 
  * otherwise you will set the time at everytime you upload the sketch
  * /
  /* Begin block
  tmElements_t tm;
  tm.Hour = 00;               // set the RTC to an arbitrary time
  tm.Minute = 00;
  tm.Second = 00;
  tm.Day = 4;
  tm.Month = 2;
  tm.Year = 2018 - 1970;      // tmElements_t.Year is the offset from 1970
  RTC.write(tm);              // set the RTC from the tm structure
  Block end * */
  time_t t; //create a temporary time variable so we can set the time and read the time from the RTC
  t=RTC.get();//Gets the current time of the RTC
  RTC.setAlarm(ALM1_MATCH_MINUTES , 0, minute(t)+time_interval, 0, 0);// Setting alarm 1 to go off 5 minutes from now
  // clear the alarm flag
  RTC.alarm(ALARM_1);
  // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
  RTC.squareWave(SQWAVE_NONE);
  // enable interrupt output for Alarm 1
  RTC.alarmInterrupt(ALARM_1, true);
}

void loop() {
 delay(5000);//wait 5 seconds before going to sleep. In real senairio keep this as small as posible
 Going_To_Sleep();
}

void Going_To_Sleep(){
    sleep_enable();//Enabling sleep mode
    attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    digitalWrite(LED_BUILTIN,LOW);//turning LED off
    time_t t;// creates temp time variable
    t=RTC.get(); //gets current time from rtc
    Serial.println("Sleep  Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//prints time stamp on serial monitor
    delay(1000); //wait a second to allow the led to be turned off before going to sleep
    sleep_cpu();//activating sleep mode
    Serial.println("just woke up!");//next line of code executed after the interrupt 
    digitalWrite(LED_BUILTIN,HIGH);//turning LED on
    //function that reads the temp and the humidity
    t=RTC.get();
    Serial.println("WakeUp Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//Prints time stamp 
    //Set New Alarm
    RTC.setAlarm(ALM1_MATCH_MINUTES , 0, minute(t)+time_interval, 0, 0);
  
  // clear the alarm flag
  RTC.alarm(ALARM_1);
  }

void wakeUp(){
  Serial.println("Interrrupt Fired");//Print message to serial monitor
   sleep_disable();//Disable sleep mode
  detachInterrupt(0); //Removes the interrupt from pin 2;
 
}
