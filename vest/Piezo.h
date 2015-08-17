#ifndef __PEIZO_H__
#define __PEIZO_H__

#include <TimerOne.h>

class Piezo {
  int m_pin;
  int m_isOn;
public:
  Piezo(int pin) : 
  m_pin(pin), m_isOn(false) {
  }

  void start(long frequencyHz);

  void stop();
  
  void play(long frequency, long time){
    start(frequency);
    delay(time);
    stop();

  }
  bool isOn() {
    return m_isOn;
  }
};

#endif
