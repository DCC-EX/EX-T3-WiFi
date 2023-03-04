#ifndef SETTINGS_H
#define SETTINGS_H

#include <Events.h>
#if __has_include(<ThrottleWiFiSettings.h>)
// TODO, deprecate
#include <ThrottleWiFiSettings.h>
#endif
#include <IPAddress.h>
#include <ArduinoJson.h>

class SettingsClass : public Events {
  public:
    struct Event {
      static constexpr uint8_t CS_CHANGE = 0;
      static constexpr uint8_t ROTATION_CHANGE = 1;
    };
    struct Rotation {
      static constexpr uint8_t STANDARD = 0;
      static constexpr uint8_t INVERTED = 1;
      static constexpr uint8_t ACCELEROMETER = 2;
    };

    uint8_t rotation = Rotation::STANDARD;
    uint32_t pin = 0;
    uint16_t emergencyStop = 2000;

    void load();
    void save();

    struct AP {
      friend class SettingsClass;

      String SSID;
      String password;

      private:
        void load(const JsonObject &obj);
        void save(const JsonObject &obj); 
    } AP;

    struct CS {
      friend class SettingsClass;
        
      bool valid();

      String SSID() const;
      bool SSID(const String &value);

      String password() const;
      bool password(const String &value);

      String server() const;
      bool server(const String &value);

      uint16_t port() const;
      void port(uint16_t value);
    
      CS() {
        _ssid.reserve(32);
        _password.reserve(63);
      }

      private:
        String _ssid;
        String _password;
        String _server = "dccex";
        uint16_t _port = 2560;

        void load(const JsonObject &obj);
        void save(const JsonObject &obj);
    } CS;

    struct LocoUI {
      friend class SettingsClass;

      struct SpeedStep {
        static constexpr uint8_t STEP_1 = 0;
        static constexpr uint8_t STEP_2 = 1;
        static constexpr uint8_t STEP_4 = 2;
      };

      uint8_t speedStep = SpeedStep::STEP_1;

      struct Swipe {
        friend class SettingsClass::LocoUI;

        struct Action {
          static constexpr uint8_t NONE = 0;
          static constexpr uint8_t KEYPAD = 1;
          static constexpr uint8_t NAME = 2;
          static constexpr uint8_t GROUP = 3;
          static constexpr uint8_t NEXT = 4;
          static constexpr uint8_t PREV = 5;
          static constexpr uint8_t RELEASE = 6;
        };

        uint8_t up = Action::RELEASE;
        uint8_t down = Action::KEYPAD;
        uint8_t left = Action::NEXT;
        uint8_t right = Action::PREV;

        uint8_t release = Action::KEYPAD;

        private:
          void load(const JsonObject &obj);
          void save(const JsonObject &obj);  
      } Swipe;

      private:
        void load(const JsonObject &obj);
        void save(const JsonObject &obj);
    } LocoUI;
};

extern SettingsClass Settings;

#endif
