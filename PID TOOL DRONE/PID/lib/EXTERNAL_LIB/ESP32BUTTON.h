#ifndef ESP32BUTTON_H
#define ESP32BUTTON_H

#if !defined(ARDUINO_ARCH_ESP32)
#error
#endif

#include <Arduino.h>

typedef void (*BtnCallback)(void);

class ESP32Button {
public:
  explicit ESP32Button(int pin = -1, bool activeLow = true, bool pullupActive = true) {
    if (pin >= 0) setup(pin, pullupActive ? INPUT_PULLUP : INPUT, activeLow);
  }

  void setup(uint8_t pin, uint8_t mode = INPUT_PULLUP, bool activeLow = true) {
    _pin = pin;
    pinMode(_pin, mode);
    _pressedLevel = activeLow ? LOW : HIGH;
    _lastStable = !activeLow; // assume released at start
    _debounced = _lastStable;
    _lastDebounceMs = millis();
    _state = IDLE;
  }

  void setDebounceMs(uint16_t ms) { _debounceMs = ms; }   // default 30 ms
  void setClickMs(uint16_t ms)    { _clickMaxMs = ms; }   // default 400 ms (max press to count as click)
  void setPressMs(uint16_t ms)    { _longPressMs = ms; }  // default 700 ms

  void attachClick(BtnCallback fn)            { _onClick = fn; }
  void attachLongPressStart(BtnCallback fn)   { _onLongStart = fn; }
  void attachLongPressStop(BtnCallback fn)    { _onLongStop = fn; }


  void tick() {
    if (_pin < 0) return;

    const unsigned long now = millis();
    const bool level = (digitalRead(_pin) == _pressedLevel);


    if (level != _rawLast) {
      _rawLast = level;
      _lastDebounceMs = now;
    }
    if ((now - _lastDebounceMs) >= _debounceMs) {
      _debounced = _rawLast;
    } else {

      return;
    }

    // FSM
    switch (_state) {
      case IDLE:
        if (_debounced) {
          _state = PRESSED;
          _pressStart = now;
          _longFired = false;

        }
        break;

      case PRESSED: {
        const unsigned long held = now - _pressStart;
        if (_debounced) {
          if (!_longFired && held >= _longPressMs) {
            _longFired = true;
            if (_onLongStart) _onLongStart();
          }
        } else {

          const unsigned long dur = now - _pressStart;
          if (_longFired) {

            if (_onLongStop) _onLongStop();
          } else {

            if (dur >= _debounceMs && dur <= _clickMaxMs) {
              if (_onClick) _onClick();
            }
          }
          _state = IDLE;
        }
      } break;
    }
  }

  // Helpers
  bool isPressed() const { return (_state == PRESSED); }
  int pin() const { return _pin; }

private:
  enum State : uint8_t { IDLE = 0, PRESSED = 1 };

  // Config
  int _pin = -1;
  bool _pressedLevel = LOW;
  uint16_t _debounceMs = 30;
  uint16_t _clickMaxMs = 600;
  uint16_t _longPressMs = 800;

  // Debounce
  bool _rawLast = false;
  bool _debounced = false;
  bool _lastStable = false;     
  unsigned long _lastDebounceMs = 0;

  // State
  State _state = IDLE;
  unsigned long _pressStart = 0;
  bool _longFired = false;

  // Callbacks
  BtnCallback _onClick = nullptr;
  BtnCallback _onLongStart = nullptr;
  BtnCallback _onLongStop = nullptr;
};

#endif // ESP32BUTTON_H