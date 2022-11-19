#include <ThrottleServer.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <SPIFFS.h>
#include <SD.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <Settings.h>

ThrottleServer::ThrottleServer() : AsyncWebServer(80) { }

void ThrottleServer::begin() {
  serveStatic("/", SPIFFS, "/www/")
    .setCacheControl("max-age=604800")
    .setDefaultFile("index.html");

  on("/cs", HTTP_HEAD, [](AsyncWebServerRequest *request) {
    request->send(Settings.CS.valid() ? 200 : 404);
  });

  on("/cs", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse(false, 1024);
    JsonVariant& cs = response->getRoot();

    cs["ssid"] = Settings.CS.SSID();
    cs["password"] = Settings.CS.password();
    cs["server"] = Settings.CS.server();
    cs["port"] = Settings.CS.port();

    response->setLength();
    request->send(response);
  });

  addHandler(new AsyncCallbackJsonWebHandler("/cs", [](AsyncWebServerRequest *request, JsonVariant &json) {
    Settings.CS.SSID(json["ssid"]);
    Settings.CS.password(json["password"]);
    Settings.CS.server(json["server"]);
    Settings.CS.port(json["port"]);

    if (Settings.CS.valid()) { // Save new settings as we're valid
      Settings.save();
      request->send(204);
      Settings.dispatchEvent(static_cast<uint8_t>(SettingsClass::Event::CS_CHANGE));
    } else { // New settings are invalid so reload
      Settings.load();
      request->send(404);
    }
  }));

  on("^(\\/(?:locos|fns|icons))$", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse(true, 4096);
    JsonVariant& list = response->getRoot();

    File listDir = SD.open(request->pathArg(0));
    File listFile;

    if (strcmp(listDir.name(), "icons") == 0) {
      while ((listFile = listDir.openNextFile())) {
        if (!listFile.isDirectory()) {
          list.add(String(listFile.path()));
        }
      }
    } else {
      StaticJsonDocument<16> filterDoc;
      filterDoc["name"] = true;
      StaticJsonDocument<48> doc;

      while ((listFile = listDir.openNextFile())) {
        if (!listFile.isDirectory()) {
          ReadBufferingStream bufferedFile(listFile, doc.capacity());
          deserializeJson(doc, bufferedFile, DeserializationOption::Filter(filterDoc));
          JsonObject item = list.createNestedObject();
          item["file"] = String(listFile.path());
          item["name"] = String(doc["name"].as<const char*>());
          listFile.close();
        }
      }
    }
    listDir.close();

    response->setLength();
    request->send(response);
  });

  on("^\\/locos\\/.+\\.json$", HTTP_HEAD, [](AsyncWebServerRequest *request) {
    request->send(SD.exists(request->url()) ? 204 : 404);
  });

  on("^\\/(?:(?:locos|fns|icons)\\/.+|groups)\\.(?:json|bmp)$", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->url().endsWith(".bmp")) { // Bit hacky but allows us to use the built in ETag and Cache-Control code
      AsyncStaticWebHandler handler("", SD, "", "max-age=604800");
      handler.canHandle(request);
      handler.handleRequest(request);
    } else {
      request->send(SD, request->url());
    }
  });

  on("^\\/(?:locos|fns|icons)\\/.+\\.(?:json|bmp)$", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    bool success = SD.remove(request->url());
    request->send(success ? 204 : 404);
  });

  on("^\\/(?:(?:locos|fns|icons)\\/.+|groups)\\.(?:json|bmp)$", HTTP_PUT, [](AsyncWebServerRequest *request) {

  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!request->_tempFile) {
      request->_tempFile = SD.open(request->url(), "w");
    }

    request->_tempFile.write(data, len);

    if (len + index == total) {
      request->_tempFile.close();
      request->send(204);
    }
  });

  AsyncWebServer::begin();
}
