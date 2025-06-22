#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

WebServer server(80);

const char* ssid = "Cassies_Arcade";
const char* password = "play1234";

String getContentType(const String& filename) {
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css")) return "text/css";
  if (filename.endsWith(".js")) return "application/javascript";
  if (filename.endsWith(".png")) return "image/png";
  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return "image/jpeg";
  if (filename.endsWith(".ico")) return "image/x-icon";
  if (filename.endsWith(".woff")) return "font/woff";
  if (filename.endsWith(".ttf")) return "font/ttf";
  if (filename.endsWith(".svg")) return "image/svg+xml";
  if (filename.endsWith(".json")) return "application/json";
  return "text/plain";
}

void handleFileRequest() {
  String path = server.uri();
  if (path == "/") path = "/data/index.html";  // only if that's where it is

  Serial.println("[HTTP] Requested path: " + path);  // 🔍 SEE what's requested

  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, getContentType(path));
    file.close();
    Serial.println("[SPIFFS] Served successfully: " + path);
  } else {
    Serial.println("[SPIFFS] File not found: " + path);  // ❗SHOW missing file
    server.send(404, "text/plain", "404: File Not Found");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.softAP(ssid, password);
  Serial.println("\n[WiFi] AP Started");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.softAPIP());

  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Mount Failed");
    return;
  }
  Serial.println("[SPIFFS] Mounted successfully");

  server.onNotFound(handleFileRequest);
  server.begin();
  Serial.println("[HTTP] Server Started");
}

void loop() {
  server.handleClient();
}
