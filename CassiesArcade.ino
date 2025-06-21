#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

WebServer server(80);

const char* ssid = "Cassies_Arcade";
const char* password = "play1234"; // Leave blank ("") for open network

String getContentType(const String& filename) {
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css"))  return "text/css";
  if (filename.endsWith(".js"))   return "application/javascript";
  if (filename.endsWith(".png"))  return "image/png";
  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return "image/jpeg";
  if (filename.endsWith(".ico"))  return "image/x-icon";
  if (filename.endsWith(".woff")) return "font/woff";
  if (filename.endsWith(".ttf"))  return "font/ttf";
  return "text/plain";
}

void handleFileRequest() {
  String path = server.uri();
  if (path == "/") path = "/index.html";

  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, getContentType(path));
    file.close();
  } else {
    server.send(404, "text/plain", "404: File Not Found");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Start Access Point
  WiFi.softAP(ssid, password);
  Serial.println("\n[WiFi] AP Started");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Mount SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Mount Failed");
    return;
  }
  Serial.println("[SPIFFS] Mounted successfully");

  // Serve files on any request
  server.onNotFound(handleFileRequest);
  server.begin();
  Serial.println("[HTTP] Server Started");
}

void loop() {
  server.handleClient();
}
