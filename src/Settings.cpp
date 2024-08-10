#include <Settings.h>
#include <SD.h>
#include <Functions.h>

void SettingsClass::load() {
  if (SD.cardType() == CARD_NONE) {
    return;
  }

  File json = SD.open("/settings.json");
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, json);

  json.close();

  if (error == DeserializationError::Ok) {
    version = doc["version"] | version;

    // Version in `settings.json` is older so check for upgrade paths and save any update changes
    if (version < T3_VERSION) {
      upgrade(doc);
      version = T3_VERSION;
      save();
    }

    rotation = doc["rotation"] | rotation;
    pin = doc["pin"] | pin;
    emergencyStop = doc["emergencyStop"] | emergencyStop;

    AP.load(doc["ap"]);
    CS.load(doc["cs"]);
    LocoUI.load(doc["locoui"]);
  } else {
    init();
  }
}

void SettingsClass::save() {
  StaticJsonDocument<2048> doc;

  doc["version"] = version;
  doc["rotation"] = rotation;
  doc["pin"] = pin;
  doc["emergencyStop"] = emergencyStop;

  AP.save(doc["ap"] | doc.createNestedObject("ap"));
  CS.save(doc["cs"] | doc.createNestedObject("cs"));
  LocoUI.save(doc["locoui"] | doc.createNestedObject("locoui"));
  
  File json = SD.open("/settings.json", FILE_WRITE);
  serializeJson(doc, json);
  json.close();
}

void SettingsClass::init() {
  version = T3_VERSION;

  // Generate ap ssid and password
  char buf[9] = { 0 };
  randomChars(buf, 4);
  AP.SSID = "DCCEx-T3-";
  AP.SSID += buf;

  randomChars(buf, 8);
  AP.password = buf;

  save();
}

void SettingsClass::upgrade(JsonDocument& doc) {
  // TODO, Bit of backwards compatibility, will be removed in future release
  #if (defined THROTTLE_AP_NAME && defined THROTTLE_AP_PWD)
  if (version == 0) {
    auto ap = doc.createNestedObject("ap");
    ap["ssid"] = THROTTLE_AP_NAME;
    ap["password"] = THROTTLE_AP_PWD;
  }
  #endif
  // if (version < VERSION_TO_INT32(0, 1, 0)) {

  // }
}

void SettingsClass::AP::load(const JsonObject& obj) {
  SSID = obj["ssid"].as<const char*>();
  password = obj["password"].as<const char*>();
}

void SettingsClass::AP::save(const JsonObject& obj) {
  obj["ssid"] = SSID;
  obj["password"] = password;
}

bool SettingsClass::CS::valid() {
  return !_ssid.isEmpty() && !_server.isEmpty() && _port != 0;
}

void SettingsClass::CS::load(const JsonObject& obj) {
  SSID(obj["ssid"]);
  password(obj["password"]);
  server(obj["server"]);
  port(obj["port"]);
}

void SettingsClass::CS::save(const JsonObject & obj) {
  obj["ssid"] = SSID();
  obj["password"] = password();
  obj["server"] = server();
  obj["port"] = port();
}

String SettingsClass::CS::SSID() const {
  return _ssid;
}

bool SettingsClass::CS::SSID(const String& value) {
  if (value.length() > 32) {
    return false; 
  }
  _ssid = value;
  return true;
}

String SettingsClass::CS::password() const {
  return _password;
}

bool SettingsClass::CS::password(const String& value) {
  if (value.length() > 63) {
    return false;
  }
  _password = value;
  return true;
}

String SettingsClass::CS::server() const {
  return _server;
}

bool SettingsClass::CS::server(const String& value) {
  if (value.length() > 32) {
    return false; 
  }
  _server = value;
  return true;
}

uint16_t SettingsClass::CS::port() const {
  return _port;
}

void SettingsClass::CS::port(uint16_t value) {
  _port = value;
}

void SettingsClass::LocoUI::load(const JsonObject& obj) {
  speedStep = obj["step"] | speedStep;
  Swipe.load(obj["swipe"]);
}

void SettingsClass::LocoUI::save(const JsonObject& obj) {
  obj["step"] = speedStep;
  Swipe.save(obj["swipe"] | obj.createNestedObject("swipe"));
}

void SettingsClass::LocoUI::Swipe::load(const JsonObject& obj) {
  up = obj["up"] | up;
  down = obj["down"] | down;
  left = obj["left"] | left;
  right = obj["right"] | right;

  release = obj["release"] | release;
}

void SettingsClass::LocoUI::Swipe::save(const JsonObject& obj) {
  obj["up"] = up;
  obj["down"] = down;
  obj["left"] = left;
  obj["right"] = right;

  obj["release"] = release;
}

SettingsClass Settings;
