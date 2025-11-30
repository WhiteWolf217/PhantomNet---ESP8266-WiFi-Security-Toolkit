# WiFi Jammer - Usage Instructions

## ESP8266 Version (RECOMMENDED)

### The Arduino code has been fixed with:
1. Proper deauth packet structure
2. Two attack types:
   - Packet 1: AP -> Client deauth
   - Packet 2: AP -> Broadcast deauth
3. **20,000 packets/second** rate
4. Added `yield()` to prevent watchdog resets

### Upload to ESP8266:
1. Open Arduino IDE
2. Select Board: "NodeMCU 1.0 (ESP-12E Module)" or your ESP8266 board
3. **IMPORTANT**: Use ESP8266 Core version 2.x (NOT 3.x)
   - Tools -> Board Manager -> ESP8266 -> Install 2.7.4
4. Upload the code
5. Open Serial Monitor (115200 baud)
6. Follow menu:
   - Option 1: Scan networks
   - Option 2: Set target (type exact SSID)
   - Option 3: Start jammer

### If STILL not working:
- Check ESP8266 core version (must be 2.x)
- Verify you're close to target (10-20 meters)
- Make sure Serial Monitor shows "Packets: X" increasing rapidly
- Some modern routers have 802.11w (PMF) protection - this won't work on those

---

## Python Version (Alternative)

### Requirements:
- **Linux or macOS** (Windows not supported)
- WiFi adapter with monitor mode support
- Root/sudo access

### Setup:
```bash
# Install requirements
sudo apt-get install aircrack-ng
pip3 install scapy

# Put adapter in monitor mode
sudo airmon-ng check kill
sudo airmon-ng start wlan0  # Replace wlan0 with your interface

# Run script
sudo python3 wifi_jammer.py
```

### Compatible WiFi Adapters:
- Alfa AWUS036NHA
- TP-Link TL-WN722N v1
- Any adapter with Atheros AR9271 chipset

---

## Legal Warning
⚠️ **ONLY USE ON NETWORKS YOU OWN**
- Unauthorized WiFi jamming is **ILLEGAL**
- Can result in fines and criminal charges
- For educational/testing purposes ONLY on your own network

---

## Troubleshooting ESP8266

### Code compiles but doesn't jam:
1. Verify ESP8266 Core 2.x: Arduino IDE -> Tools -> Board Manager -> Search "ESP8266" -> Should show 2.7.4
2. Check Serial Monitor output - should show increasing packet count
3. Distance matters - be within 10-20 meters
4. Some routers use PMF (Protected Management Frames) - cannot be jammed with this method

### How to check if it's working:
- Open your phone WiFi settings
- Should see the target network appear/disappear rapidly
- Connected devices should disconnect
- Serial Monitor should show: "💥 Packets: 20000", "💥 Packets: 40000", etc.

### If wifi_send_pkt_freedom error:
Your ESP8266 core is version 3.x which removed this function.
**FIX**: Downgrade to 2.7.4 in Board Manager
