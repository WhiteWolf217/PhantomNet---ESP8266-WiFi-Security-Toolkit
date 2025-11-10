# 👻 PhantomNet - ESP8266 WiFi Security Toolkit

> ⚠️ **EDUCATIONAL PURPOSE ONLY** - Unauthorized use is **ILLEGAL**. Only test on networks you own!

A powerful ESP8266-based WiFi security testing tool featuring beacon flooding and evil twin attacks with password capture capabilities.

---

## 🚀 Features

### 1. **Network Scanner**
- Scan all nearby WiFi networks
- Display SSID, Channel, Signal Strength (RSSI)
- Show MAC addresses (BSSID)

### 2. **Beacon Flood Jammer**
- Floods target channel with fake access points
- Creates network congestion
- Random SSID generation
- Packet counter with statistics

### 3. **Evil Twin Attack** 🎯
- Creates duplicate AP of target network
- **Captive Portal** - Auto-redirects all traffic
- **Password Capture** - Logs ANY password entered
- **Always Accepts** - User connects regardless of password
- Real-time device counter
- Professional fake login page
- Works on iOS, Android, Windows, Mac

### 4. **Professional Interface**
- Clean table-based menu
- Real-time status monitoring
- Live device count updates
- Formatted password capture display

---

## 📋 Prerequisites

### Hardware
- **ESP8266 board** (NodeMCU, Wemos D1 Mini, etc.)
- USB cable
- Computer with Arduino IDE

### Software
- Arduino IDE (1.8.x or 2.x)
- ESP8266 Board Package
- Required Libraries (auto-installed):
  - ESP8266WiFi
  - ESP8266WebServer
  - DNSServer

---

## 🔧 Installation Steps

### 1. Install Arduino IDE
Download from: https://www.arduino.cc/en/software

### 2. Add ESP8266 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to "Additional Boards Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "ESP8266"
6. Install **"ESP8266 by ESP8266 Community"**

### 3. Select Your Board
1. **Tools → Board → ESP8266 Boards**
2. Select your ESP8266 model (e.g., "NodeMCU 1.0")
3. **Tools → Port** - Select your COM port

### 4. Upload the Code
1. Open `wifijam_esp8266.ino`
2. Click **Upload** button (→)
3. Wait for compilation and upload
4. Open **Serial Monitor** (Ctrl+Shift+M)
5. Set baud rate to **115200**

---

## 📱 How to Use

### Menu Interface
```
╔════════════════════════════════════════════════════════╗
║                    WIFI JAMMER MENU                    ║
╠════════════════════════════════════════════════════════╣
║  1-Scan Networks    │  2-Set Target  │  3-Start Jammer ║
║  4-Stop Jammer      │  5-Evil Twin   │  6-Stop ET      ║
║  7-Show Menu        │                │                 ║
╠════════════════════════════════════════════════════════╣
║                    CURRENT STATUS                      ║
╠═══════════════╦═════════════╦══════════╦═══════════════╣
║ Target        │ Jammer      │ Evil Twin│ Devices       ║
╠═══════════════╬═════════════╬══════════╬═══════════════╣
║ MyWiFi        │ OFF         │ ON       │ 2             ║
╚═══════════════╩═════════════╩══════════╩═══════════════╝
```

### Basic Workflow

#### Option 1: Beacon Jammer
1. Press **1** - Scan networks
2. Press **2** - Enter target SSID
3. Press **3** - Start jammer
4. Watch packets flood the channel
5. Press **4** - Stop jammer

#### Option 2: Evil Twin Password Capture 🎯
1. Press **1** - Scan networks
2. Press **2** - Set target network name
3. Press **5** - Start Evil Twin
4. Wait for victim to connect
5. Victim enters password → **CAPTURED!**
6. Check Serial Monitor for password
7. Press **6** - Stop and view captured password

---

## 🎯 Evil Twin Attack Details

### How It Works
1. **Creates fake AP** with same SSID as target
2. **Captive portal** redirects all traffic to login page
3. User sees: *"Router firmware update required"*
4. User enters WiFi password
5. **Password is logged** to Serial Monitor
6. User sees: *"Connected Successfully!"*
7. User thinks they're connected (social engineering)

### What Gets Captured
```
🎯 PASSWORD CAPTURED! 🎯
SSID: MyHomeWiFi
Password: MySecretPass123
====================
```

### Device Monitoring
- Shows live count of connected devices
- Updates every 2 seconds
- Displays in status table
- Example: `📱 Connected Devices: 3`

---

## ⚙️ Technical Details

### ESP8266 Capabilities
- **Packet Injection**: Send custom 802.11 frames
- **AP Mode**: Create access points
- **Monitor Mode**: Capture WiFi traffic
- **Channel Control**: Switch between WiFi channels

### Attack Mechanisms

**Beacon Flooding:**
- Sends fake beacon frames
- Random SSIDs and MACs
- Saturates WiFi scanner apps
- Creates network noise

**Evil Twin:**
- Open AP (no encryption needed)
- DNS hijacking (all domains → ESP8266)
- HTTP server with fake login
- Password logger

---

## 🛡️ Legal & Ethical Considerations

### ⚠️ **WARNING**

**This tool is for EDUCATIONAL and AUTHORIZED TESTING ONLY!**

**Illegal Activities:**
- ❌ Attacking networks you don't own
- ❌ Interfering with others' WiFi
- ❌ Capturing passwords without consent
- ❌ Using in public spaces

**Legal Uses:**
- ✅ Testing your own network security
- ✅ Educational/research purposes
- ✅ Authorized penetration testing
- ✅ Cybersecurity training

**Consequences:**
- Heavy fines
- Criminal charges
- Imprisonment
- FCC violations (USA)

**YOU ARE RESPONSIBLE** for how you use this tool!

---

## 🐛 Troubleshooting

### Upload Fails
- Check correct COM port selected
- Try holding FLASH button during upload
- Reset ESP8266 before upload
- Check USB cable (data cable, not charge-only)

### Can't See Serial Output
- Set baud rate to **115200**
- Try different line ending (Both NL & CR)
- Press reset button on ESP8266

### Evil Twin Not Working
- Make sure jammer is stopped first
- Check if DNS server started (check serial)
- Try restarting ESP8266
- Verify target SSID is set

### No Password Captured
- User must click login page
- Check if devices are connecting (device count)
- Some devices may not show captive portal immediately
- Try disconnecting/reconnecting to the fake AP

---

## 📊 Status Indicators

| Indicator | Meaning |
|-----------|---------|
| `Target: None` | No target network selected |
| `Jammer: ON` | Beacon flooding active |
| `ET: ON` | Evil Twin running |
| `Devices: 3` | 3 devices connected to Evil Twin |
| `🎯 PASSWORD CAPTURED!` | Password logged successfully |
| `📱 Connected Devices: X` | Device count updated |

---

## 🔄 Version History

**v2.0** (Current)
- ✨ Added Evil Twin with password capture
- ✨ Captive portal implementation
- ✨ Real-time device counter
- ✨ Professional table menu
- ✨ Enhanced status displays
- 🎨 Improved UI/UX

**v1.0**
- Basic beacon flooding
- Network scanner
- Simple menu system

---

## 📚 Learn More

### WiFi Security Resources
- [WiFi 802.11 Frame Types](https://en.wikipedia.org/wiki/802.11_Frame_Types_and_Formats)
- [Evil Twin Attack Explained](https://www.kaspersky.com/resource-center/threats/evil-twin-attacks)
- [Captive Portal Detection](https://developer.android.com/training/monitoring-device-state/connectivity-status-type#captive-portals)

### ESP8266 Documentation
- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)
- [ESP8266 WiFi Library](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html)

---

## 📄 License

This project is for **educational purposes only**. Use responsibly and legally.

**Disclaimer:** The author is not responsible for any misuse or damage caused by this program.

---

## Credits

Built with ESP8266 Arduino Core  
Inspired by WiFi security research community  
Created for educational cybersecurity training

---

**Remember:** With great power comes great responsibility. Use this knowledge to protect, not to harm! 🛡️
