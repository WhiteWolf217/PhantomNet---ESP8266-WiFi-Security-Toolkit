# Project Flowcharts

This file contains the Mermaid syntax for the flowcharts used in the project introduction. You can paste this code into any Mermaid-compatible viewer (like the Markdown preview in VS Code or online editors) to render the diagrams.

---

## System Flowchart

This chart illustrates the main operational loop of the device, driven by user commands from the serial interface.

```mermaid
graph TD
    A[Start / Power On] --> B{Initialize Serial & WiFi};
    B --> C[Display Main Menu];
    C --> D{Wait for User Input};

    D -- "1. Scan" --> E[Scan for WiFi Networks];
    E --> C;

    D -- "2. Set Target" --> F[Select Target SSID from Scan];
    F --> C;

    D -- "3. Start Jammer" --> G{Target Set?};
    G -- Yes --> H[Activate Deauth Flood Loop];
    G -- No --> I[Show Error Message];
    I --> C;
    H --> J{Jammer Loop};
    J -- "Input: 4. Stop" --> K[Deactivate Jammer];
    K --> C;

    D -- "5. Start Evil Twin" --> L{Target Set?};
    L -- Yes --> M[Activate AP Mode & Start Servers];
    L -- No --> I;
    M --> N{Evil Twin Loop};
    N -- "Input: 6. Stop" --> O[Stop Servers & Deactivate AP];
    O --> C;

    D -- "7. JAM ALL WiFi" --> P[Activate Broadcast Deauth & Channel Hopping];
    P --> J;

    D -- "0. Show Menu" --> C;
```

---

## Evil Twin Attack Flow Diagram

This sequence diagram shows the step-by-step interaction between a victim, the ESP8266 (acting as an Evil Twin), and the real access point during a credential harvesting attack.

```mermaid
sequenceDiagram
    participant V as Victim Device
    participant E as ESP8266 (Evil Twin)
    participant R as Real AP

    Note over R: Jammed by ESP8266
    V->>R: Connection Failed
    V->>E: Connects to Rogue AP (Same SSID)
    E-->>V: Connection Successful

    V->>E: DNS Query for any domain (e.g., google.com)
    E-->>V: Response: My IP Address (e.g., 192.168.4.1)

    V->>E: HTTP Request to ESP8266's IP
    E-->>V: Serve Fake Login Page

    V->>E: POST /submit (with password)
    Note over E: Password Captured & Logged to Serial!
    E-->>V: Serve "Success" Page & Redirect
```
