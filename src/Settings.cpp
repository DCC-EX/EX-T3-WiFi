#include <Settings.h>
#include <SD.h>

void SettingsClass::load() {
  File json = SD.open("/settings.json");
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error == DeserializationError::Ok) {
    CS.load(doc["cs"]);
    LocoUI.load(doc["locoui"]);
    rotation = static_cast<Rotation>(doc["rotation"].as<uint8_t>());
  }

  json.close();
}

void SettingsClass::save() {
  StaticJsonDocument<2048> doc;

  CS.save(doc["cs"] | doc.createNestedObject("cs"));
  LocoUI.save(doc["locoui"] | doc.createNestedObject("locoui"));
  doc["rotation"] = static_cast<uint8_t>(rotation);

  File json = SD.open("/settings.json", FILE_WRITE);
  serializeJson(doc, json);
  json.close();
}

bool SettingsClass::CS::valid() {
  return !_ssid.isEmpty() && !_server.isEmpty() && _port != 0;
}

void SettingsClass::CS::load(const JsonObject &obj) {
  SSID(obj["ssid"]);
  password(obj["password"]);
  server(obj["server"]);
  port(obj["port"]);
}

void SettingsClass::CS::save(const JsonObject &obj) {
  obj["ssid"] = SSID();
  obj["password"] = password();
  obj["server"] = server();
  obj["port"] = port();
}

String SettingsClass::CS::SSID() const {
  return _ssid;
}

bool SettingsClass::CS::SSID(const String &value) {
  if (value.length() > 32) {
    return false; 
  }
  _ssid = value;
  return true;
}

String SettingsClass::CS::password() const {
  return _password;
}

bool SettingsClass::CS::password(const String &value) {
  if (value.length() > 63) {
    return false;
  }
  _password = value;
  return true;
}

String SettingsClass::CS::server() const {
  return _server;
}

bool SettingsClass::CS::server(const String &value) {
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

void SettingsClass::LocoUI::load(const JsonObject &obj) {
  speedStep = static_cast<SpeedStep>(obj["step"].as<uint8_t>());
  Swipe.load(obj["swipe"]);
}

void SettingsClass::LocoUI::save(const JsonObject &obj) {
  obj["step"] = static_cast<uint8_t>(speedStep);
  Swipe.save(obj["swipe"] | obj.createNestedObject("swipe"));
}

void SettingsClass::LocoUI::Swipe::load(const JsonObject &obj) {
  up = static_cast<Action>(obj["up"].as<uint8_t>());
  down = static_cast<Action>(obj["down"].as<uint8_t>());
  left = static_cast<Action>(obj["left"].as<uint8_t>());
  right = static_cast<Action>(obj["right"].as<uint8_t>());

  release = static_cast<Action>(obj["release"].as<uint8_t>());
}

void SettingsClass::LocoUI::Swipe::save(const JsonObject &obj) {
  obj["up"] = static_cast<uint8_t>(up);
  obj["down"] = static_cast<uint8_t>(down);
  obj["left"] = static_cast<uint8_t>(left);
  obj["right"] = static_cast<uint8_t>(right);

  obj["release"] = static_cast<uint8_t>(release);
}

SettingsClass Settings;
