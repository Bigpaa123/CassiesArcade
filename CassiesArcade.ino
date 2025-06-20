#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

WebServer server(80);

const char* ssid = "Cassies_Arcade";
const char* password = "play1234";  // Leave empty for open network

// Determine MIME type
String getContentType(const String& filename) {
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css"))  return "text/css";
  if (filename.endsWith(".js"))   return "application/javascript";
  if (filename.endsWith(".png"))  return "image/png";
  if (filename.endsWith(".jpg"))  return "image/jpeg";
  if (filename.endsWith(".ico"))  return "image/x-icon";
  return "text/plain";
}

void handleNotFound() {
  String path = server.uri();
  if (path == "/") path = "/index.html";

  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, FILE_READ);
    server.streamFile(file, getContentType(path));
    file.close();
  } else {
    server.send(404, "text/plain", "404: File Not Found");
  }
}

void setup() {
  Serial.begin(115200);

  // Start Access Point
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Mount SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Setup catch-all route
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();
}

