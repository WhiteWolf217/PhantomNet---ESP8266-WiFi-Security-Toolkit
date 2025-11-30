# ESP32 WiFi Deauther - Arduino Setup Guide

## ⚠️ LEGAL DISCLAIMER
This tool is for EDUCATIONAL and AUTHORIZED SECURITY TESTING ONLY!
- Only use on networks YOU OWN
- Unauthorized use is ILLEGAL and punishable by law
- This is for cybersecurity research and learning
- Always get written permission before testing

## What You Need
1. ESP32 Development Board
2. Arduino IDE installed
3. USB cable
4. Your own WiFi network for testing

## Setup Instructions

### Step 1: Install Arduino IDE
Download from: https://www.arduino.cc/en/software

### Step 2: Add ESP32 Board Support
1. Open Arduino IDE
2. Go to `File` → `Preferences`
3. In "Additional Board Manager URLs", add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to `Tools` → `Board` → `Boards Manager`
5. Search for "esp32"
6. Install "esp32 by Espressif Systems"

### Step 3: Select Your Board
1. Go to `Tools` → `Board` → `ESP32 Arduino`
2. Select your ESP32 model (usually "ESP32 Dev Module")
3. Go to `Tools` → `Port` and select your COM port

### Step 4: Upload the Code
1. Open `wifijam.ino` in Arduino IDE
2. Click the Upload button (→)
3. Wait for compilation and upload to complete

### Step 5: Open Serial Monitor
1. Go to `Tools` → `Serial Monitor`
2. Set baud rate to `115200`
3. You should see the menu

## How to Use

### Menu Options:
1. **Scan WiFi Networks** - Shows all nearby networks with MAC addresses
2. **Set Target** - Enter the SSID of YOUR network to test
3. **Start Deauth Attack** - Begins sending deauth packets
4. **Stop Attack** - Stops the attack
5. **Show Menu** - Displays menu again

### Example Workflow:
```
1. Type "1" and press Enter to scan
2. Type "2" and enter your network SSID
3. Type "3" to start attack (devices will disconnect)
4. Type "4" to stop
```

## How It Works

The code uses ESP32's WiFi capabilities to:
1. **Scan networks** - Identifies APs and their MAC addresses
2. **Craft deauth packets** - Creates 802.11 deauthentication frames
3. **Inject packets** - Uses `esp_wifi_80211_tx()` to send raw frames
4. **Broadcast attack** - Sends to FF:FF:FF:FF:FF:FF (all clients)

### Deauth Packet Structure:
```
Frame Control: 0xC0 (Deauthentication)
Destination: Broadcast (all clients)
Source: Target AP MAC
BSSID: Target AP MAC
Reason: Class 3 frame from non-associated STA
```

## Troubleshooting

**Error: "esp_wifi_80211_tx not declared"**
- Update your ESP32 board package to latest version
- Some ESP32 boards may have this function disabled

**Upload fails:**
- Hold the BOOT button on ESP32 while uploading
- Check correct COM port is selected
- Try lower baud rate in Tools → Upload Speed

**No networks in scan:**
- Check antenna connection (if external)
- Try in area with known WiFi networks

**Attack not working:**
- Verify you set the correct target SSID
- Modern routers may have deauth protection (802.11w)
- Some clients auto-reconnect quickly

## Advanced Usage

### For Pentesting Projects:
- Add beacon frame flooding
- Add probe request sniffing
- Capture handshakes for WPA analysis
- Add web interface for easier control

### Recommended Next Steps:
1. Study 802.11 frame structures
2. Learn about WiFi security protocols (WPA2, WPA3)
3. Research WiFi Protected Management Frames (802.11w)
4. Explore legitimate pentesting certifications (CEH, OSCP)

## Important Notes

- **Modern routers** with 802.11w (PMF) are protected against deauth
- **WPA3** networks are more resistant to these attacks
- **Legal use only** - get written permission before testing
- This is a **learning tool** for understanding WiFi vulnerabilities
- Professional pentesters use tools like Aircrack-ng suite

## References

- IEEE 802.11 Standard
- ESP32 WiFi Library Documentation
- Aircrack-ng Documentation
- WiFi Security Research Papers

---

Remember: With great power comes great responsibility!
Use this knowledge to PROTECT networks, not attack them.
