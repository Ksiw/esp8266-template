#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "defines.h"

static ESP8266WebServer http_server(OTA_PORT);
static ESP8266HTTPUpdateServer http_updater;

//------------------------------------------------------------------------------
static void handle_404()
{
  http_server.send(404, "text/plain; charset=utf-8", "404: Прювет");
}
//------------------------------------------------------------------------------

void firmware_update_init()
{
  http_server.on("/", HTTP_GET, []() {
    if (!http_server.authenticate(OTA_USER, OTA_PASSWORD)) {
      return http_server.requestAuthentication();
    }
    http_server.send(200, "text/html; charset=utf-8",
      "<h2>Обновление прошивки</h2>"
      "<form method='POST' action='/update' enctype='multipart/form-data'>"
      "<input type='file' name='update'><br>"
      "<input type='submit' value='Обновить'>"
      "</form>"
    );
  });


  http_updater.setup(&http_server, OTA_PATH, OTA_USER, OTA_PASSWORD);

  http_server.onNotFound(handle_404);
  http_server.begin();
  WRITE_INFO("HTTP OTA запущен на порту ", OTA_PORT, "\n");
}
//------------------------------------------------------------------------------

void firmware_update_process()
{
  http_server.handleClient();
}
