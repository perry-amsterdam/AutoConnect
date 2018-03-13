#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

AutoConnect portal;

void handleRoot() {
  String page = PSTR(
"<html>"
"</head>"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
  "<style type=\"text/css\">"
    "body {"
    "-webkit-appearance:none;"
    "-moz-appearance:none;"
    "font-family:'Arial',sans-serif;"
    "text-align:center;"
    "}"
    ".menu {"
    "text-align:right;"
    "}"
    ".button {"
    "display:inline-block;"
    "border-radius:7px;"
    "background:#73ad21;"
    "margin:0 10px 0 10px;"
    "padding:10px 20px 10px 20px;"
    "text-decoration:none;"
    "color:#000000;"
    "}"
  "</style>"
"</head>"
"<body>"
  "<p class=\"menu\">" AUTOCONNECT_LINK(BAR_32) "</p>"
  "BUILT-IN LED<br>"
  "GPIO(");
  page += String(BUILTIN_LED);
  page += String(F(") : <span style=\"font-weight:bold;color:"));
  page += digitalRead(BUILTIN_LED) ? String("Tomato\">HIGH") : String("SlateBlue\">LOW");
  page += String(F("</span>"));
  page += String(F("<p><a class=\"button\" href=\"/io?v=low\">low</a><a class=\"button\" href=\"/io?v=high\">high</a></p>"));
  page += String(F("</body></html>"));
  portal.host().send(200, "text/html", page);
}

void handleGPIO() {
  ESP8266WebServer& server = portal.host();
  if (server.arg("v") == "low")
    digitalWrite(BUILTIN_LED, LOW);
  else if (server.arg("v") == "high")
    digitalWrite(BUILTIN_LED, HIGH);
  sendRedirect("/");
}

void sendRedirect(String uri) {
  ESP8266WebServer& server = portal.host();
  server.sendHeader("Location", uri, true);
  server.send(302, "text/plain", "");
  server.client().stop();
}

bool atDetect(IPAddress softapIP) {
  Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
  return true;
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  pinMode(BUILTIN_LED, OUTPUT);

  // Put the home location of the web site.
  // But in usually, setting the home uri is not needed cause default location is "/".
  //portal.home("/");   

  // Starts user web site included the AutoConnect portal.
  portal.onDetect(atDetect);
  if (portal.begin()) {
    ESP8266WebServer& server = portal.host();
    server.on("/", handleRoot);
    server.on("/io", handleGPIO);
    Serial.println("Started, IP:" + WiFi.localIP().toString());
  }
  else {
    Serial.println("Connection failed.");
    while (true) { yield(); }
  }
}

void loop() {
  portal.handleClient();
  if (WiFi.status() == WL_IDLE_STATUS) {
    ESP.reset();
    delay(1000);
  }
}