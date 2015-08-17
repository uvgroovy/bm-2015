#include "Piezo.h"

void Piezo::start(long frequencyHz) {
  long periodMicro = 1000000/frequencyHz;

  Timer1.setPeriod(periodMicro);
  if (!m_isOn) {
    Timer1.pwm(m_pin, 512);
    m_isOn = true;
  }
}

void Piezo::stop() {
  m_isOn = false;
  Timer1.disablePwm(m_pin);  
}
