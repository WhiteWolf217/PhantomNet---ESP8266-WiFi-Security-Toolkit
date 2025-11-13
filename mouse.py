import sys
import serial
import pyautogui as pg

pg.FAILSAFE = False
pg.PAUSE = 0

def main():
    try:
        ser = serial.Serial("COM3", 9600, timeout=0.01)
    except Exception as e:
        print("Failed to open serial port:", e)
        sys.exit(1)

    ser.reset_input_buffer()
    
    while True:
        try:
            read = ser.readline().decode('ascii', errors='ignore').strip()
            if not read:
                continue
                
            readArray = read.split()
            if len(readArray) < 4:
                continue
                
            c, deltaX, deltaY, m = readArray[:4]
            
            try:
                dx = int(deltaX)
                dy = int(deltaY)
            except ValueError:
                continue
            
            if dx != 0 or dy != 0:
                print(f"dx={dx}, dy={dy}")
            
            if c == 'c':
                pg.click(button='left')

            if dx != 0 or dy != 0:
                pg.moveRel(dx, dy, duration=0, _pause=False)
        
        except Exception as e:
            pass


if __name__ == "__main__":
    main()