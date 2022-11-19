#ifndef SETTINGS_H
#define SETTINGS_H

#include <Events.h>
#include <ThrottleWiFiSettings.h>
#include <IPAddress.h>
#include <ArduinoJson.h>

class SettingsClass : public Events {
  public:
    enum class Event : uint8_t {
      CS_CHANGE,
      ROTATION_CHANGE
    };
    enum class Rotation : uint8_t {
      STANDARD,
      INVERTED,
      #if defined(USE_ACCELEROMETER)
      ACCELEROMETER
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
        void port(const uint16_t &value);
      
        CS() {
          _ssid.reserve(32);
          _password.reserve(63);
          _server.reserve(32);
        }
    } CS;

    struct LocoUI {
      friend class SettingsClass;
      
      private:
        void load(const JsonObject &obj);
        void save(const JsonObject &obj);
      public:
        enum class SpeedStep : uint8_t {
          STEP_1,
          STEP_2,
          STEP_4
        };

        SpeedStep speedStep = SpeedStep::STEP_1;

        struct Swipe {
          friend class SettingsClass::LocoUI;

          private:
            void load(const JsonObject &obj);
            void save(const JsonObject &obj);
          public:
            enum class Action : uint8_t {
              KEYPAD,
              NAME,
              GROUP,
              NEXT,
              PREV,
              RELEASE,
            };

            Action up = Action::RELEASE;
            Action down = Action::KEYPAD;
            Action left = Action::NEXT;
            Action right = Action::PREV;

            Action release = Action::KEYPAD;
        } Swipe;
    } LocoUI;

    Rotation rotation = Rotation::STANDARD;
};

extern SettingsClass Settings;

#endif
