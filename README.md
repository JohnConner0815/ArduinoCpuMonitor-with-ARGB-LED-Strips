# Arduino-ARGB-CPU-Monitor
This CPU meter displays CPU usage using an ARGB LED strip. It has several color profiles stored, and you can adjust the brightness. The most recently used settings are saved in the Arduino and loaded the next time it is restarted or the USB cable is reconnected.

![](https://github.com/JohnConner0815/ArduinoGpuMonitor/blob/main/Chematic.png)
1. Open cpumonitor_LEDStrip.ino and adjust the number of LEDs in line 10 below “// --- HARDWARE CONFIGURATION ---” depending on how many you want to control. The scaling is done automatically. Save this to your file.
2. Upload cpumonitor_LEDStrip.ino to your board
3. Run ArduinoCpuMonitor.exe
4. Select the colour profile by pressing the Button which is connected to Pin 3
5. The following colour profiles are available: 
 > - 1 White
 > - 2 Blue
 > - 3 Cyan
 > - 4 thermometer style with green up to 50%, then yellow at 75% and red at 90% and higher
 > - 5 similar to 4 but all used LEDs lit up in the colour depending on the GPU-usage
 > - 6 all LEDs are off
6. Select your brightness by pressing the Button connected to Pin 4
7. Brightness adjustment is logarithmic; it is therefore more of a gamma correction, which is more easily perceived by the human eye.
