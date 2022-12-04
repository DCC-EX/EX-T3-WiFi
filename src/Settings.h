#ifndef SETTINGS_H
#define SETTINGS_H

#include <Events.h>
#include <ThrottleWiFiSettings.h>
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
      #if defined(USE_ACCELEROMETER)
      static constexpr uint8_t ACCELEROMETER = 2;
      #endif
    };
  public:
    void load();
    void save();

    struct CS {
      friend class SettingsClass;

      private:
        String _ssid;
        String _password;
        String _server = "dccex";
        uint16_t _port = 2560;

        void load(const JsonObject &obj);
        void save(const JsonObject &obj);
      public:
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
    } CS;

    struct LocoUI {
      friend class SettingsClass;
      
      private:
        void load(const JsonObject &obj);
        void save(const JsonObject &obj);
      public:
        struct SpeedStep {
          static constexpr uint8_t STEP_1 = 0;
          static constexpr uint8_t STEP_2 = 1;
          static constexpr uint8_t STEP_4 = 2;
        };

        uint8_t speedStep = SpeedStep::STEP_1;

        struct Swipe {
          friend class SettingsClass::LocoUI;

          private:
            void load(const JsonObject &obj);
            void save(const JsonObject &obj);
          public:
            struct Action {
              static constexpr uint8_t KEYPAD = 0;
              static constexpr uint8_t NAME = 1;
              static constexpr uint8_t GROUP = 2;
              static constexpr uint8_t NEXT = 3;
              static constexpr uint8_t PREV = 4;
              static constexpr uint8_t RELEASE = 5;
            };

            uint8_t up = Action::RELEASE;
            uint8_t down = Action::KEYPAD;
            uint8_t left = Action::NEXT;
            uint8_t right = Action::PREV;

            uint8_t release = Action::KEYPAD;
        } Swipe;
    } LocoUI;

    uint8_t rotation = Rotation::STANDARD;
};

extern SettingsClass Settings;

#endif
