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

void listSPIFFSFiles() {
  Serial.println("[SPIFFS] Listing files:");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  if (!file) {
    Serial.println("[SPIFFS] No files found.");
  }

  while (file) {
    Serial.println("[SPIFFS] File: " + String(file.name()));
    file = root.openNextFile();
  }
}

void handleFileRequest() {
  String path = server.uri();
  if (path == "/") path = "/index.html";

  Serial.println("[HTTP] Requested path: " + path);

  String contentType = getContentType(path);

  if (SPIFFS.exists(path)) {
    Serial.println("[SPIFFS] File found: " + path);
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    Serial.println("[SPIFFS] File NOT found: " + path);
    listSPIFFSFiles(); // Show all files currently in SPIFFS
    server.send(404, "text/plain", "File Not Found: " + path);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.softAP(ssid, password);
  Serial.println("\n[WiFi] Access Point Started");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.softAPIP());

  Serial.println("[SPIFFS] Attempting to mount...");
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Mount FAILED");
    return;
  }
  Serial.println("[SPIFFS] Mount SUCCESSFUL");
  listSPIFFSFiles();

  server.onNotFound(handleFileRequest);
  server.begin();
  Serial.println("[HTTP] Server Started");
}

void loop() {
  server.handleClient();
}
