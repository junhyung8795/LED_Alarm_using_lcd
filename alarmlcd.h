#include <Arduino.h>
#define INC_SW 2
#define DEC_SW 3 
#define CURSOR A0
#define LED 5
#define HOUR_START 100
#define HOUR_END 66
#define MINUTE_START 65
#define MINUTE_END 34
#define MODE_START 33
#define MODE_END 0
#define CDS A5
#define SPEAKER 7
class AlarmLCD{
  int minute;
  int hour;
  String state;
  public:
    AlarmLCD(){
      minute = 0;
      hour = 0;
      state = "OFF";
      pinMode(CDS,INPUT);
      pinMode(INC_SW, INPUT_PULLUP);
      pinMode(DEC_SW, INPUT_PULLUP);
      pinMode(LED, OUTPUT);
      pinMode(SPEAKER,OUTPUT);
    }
    int getMinute()  {
      return minute;
    }
    int getHour()  {
      return hour;
    }
    String getState() {
      return state;
    }
    void setMinute(int minute_){
      minute = minute_;
    }
    void setHour(int hour_){
      hour = hour_;
    }
    void setState(){
      if (state == "OFF")
        state = "READY";
      else if (state == "READY")
        state = "ON";
      else if (state == "ON")
        state = "OFF";
    }
    
};