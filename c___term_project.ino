#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "alarmlcd.h"
#include <Bounce2.h>
#define TICK 60000000
#define TEST_TIME 15
LiquidCrystal_PCF8574 lcd(0x27);

AlarmLCD myAlarm;
Bounce INC_SW_DEBOUNCE = Bounce();
Bounce DEC_SW_DEBOUNCE = Bounce();
boolean let_speaker = false;
boolean let_led = false;
boolean timerState = false;
uint32_t prevTime  = millis(), curTime, interval = 60000;

int light = 0;
void setup() {
  int error;
  INC_SW_DEBOUNCE.attach(INC_SW);
  INC_SW_DEBOUNCE.interval(10);
  DEC_SW_DEBOUNCE.attach(DEC_SW);
  DEC_SW_DEBOUNCE.interval(10);
   
  Serial.begin(9600);
  Serial.println("LCD...");
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");
    lcd.begin(16, 2);  
  } else {
    Serial.println(": LCD not found.");
  }  
}  

void incHour(){
  myAlarm.setHour((myAlarm.getHour() + 1) % 24);
}

void decHour(){
  if (myAlarm.getHour() > 0)
    myAlarm.setHour((myAlarm.getHour() - 1) % 24);
  else if(myAlarm.getHour() == 0)
    myAlarm.setHour(23);
}

void incMinute(){
  myAlarm.setMinute((myAlarm.getMinute() + 1) % 60);
  
}

void decMinute(){
  if (myAlarm.getMinute() > 0)
    myAlarm.setMinute((myAlarm.getMinute() - 1) % 60);
  else if(myAlarm.getMinute() == 0)
    myAlarm.setMinute(59);
}


void loop() {
  
  INC_SW_DEBOUNCE.update();
  DEC_SW_DEBOUNCE.update();
  int cursorPosition;
  cursorPosition = analogRead(CURSOR);
  cursorPosition = map(cursorPosition,0,1023,0,100);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.setBacklight(255);
  lcd.cursor();
  lcd.blink();
  if(myAlarm.getHour() >= 10)
    lcd.print(myAlarm.getHour());
  else {
    lcd.print("0");
    lcd.print(myAlarm.getHour());
  }
  lcd.print(":");
  if(myAlarm.getMinute() >= 10)
    lcd.print(myAlarm.getMinute());
  else {
    lcd.print("0");
    lcd.print(myAlarm.getMinute());
  }
  lcd.setCursor(10,1);
  lcd.print(myAlarm.getState());
  lcd.setCursor(1,0);
  analogWrite(LED, light);
  
  if (myAlarm.getState() == "OFF"){
    noTone(SPEAKER);
    light = 0;
    let_speaker = false;
    let_led = false;
    timerState = false;
    myAlarm.setHour(0);
    myAlarm.setMinute(0);
  }

  
  if (myAlarm.getState() == "ON") {
    if (timerState == false){
      timerState = true;
      prevTime = millis();
    }
  }
  if(timerState == true){
    curTime = millis();
    if (curTime - prevTime >= interval){
      prevTime = curTime;
      int hours = myAlarm.getHour();
      int minutes = myAlarm.getMinute();
      int total = hours * 60 + minutes;
      total = total - 1;
      myAlarm.setHour(total / 60);
      myAlarm.setMinute(total % 60);
      if (total == TEST_TIME){
        float adc=analogRead(CDS);
        Serial.println("start");
        if (adc <= 985) {
          let_led = true;
          light = 15;
          Serial.println("led");
          Serial.println(adc);}
        else {
          let_speaker = true;
          Serial.println(adc);}
      }
      else if (let_led){
        if (light != 255){
          light += 16;
          Serial.println(light);}
        else 
          total = 0;
      }
      else if (total == 0 && let_speaker){
        tone(SPEAKER,800); 
        total = 0;
      }
    }
  }
 
  if (cursorPosition<=HOUR_START && cursorPosition >= HOUR_END){
    lcd.setCursor(1,0);
    if (myAlarm.getState() == "READY"){
      if(INC_SW_DEBOUNCE.fell())
        incHour();
      else if(DEC_SW_DEBOUNCE.fell())
        decHour();    
    }
  }
  else if (cursorPosition<=MINUTE_START && cursorPosition >= MINUTE_END){
    lcd.setCursor(4,0);
    if (myAlarm.getState() == "READY"){
      if(INC_SW_DEBOUNCE.fell())
        incMinute();
      else if(DEC_SW_DEBOUNCE.fell())
        decMinute();  
    }
  }
  else if (cursorPosition<=MODE_START && cursorPosition >= MODE_END){
    lcd.setCursor(9,1);

      if(INC_SW_DEBOUNCE.fell() || DEC_SW_DEBOUNCE.fell()){
        myAlarm.setState();
      
    }
  }
  
  
  
  delay(100);
}  