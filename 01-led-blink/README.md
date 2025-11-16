\# 01 – LED Blink (ESP32-S3, GPIO 4)



Basic ESP32-S3 LED blink example using ESP-IDF and FreeRTOS.



This project is meant as a minimal “Hello World” for the ESP32-S3: it blinks an external LED and prints log messages over the serial monitor.



---



\## Features



\- Blinks an external LED connected to \*\*GPIO 4\*\* every 500 ms.

\- Uses `vTaskDelay(pdMS\_TO\_TICKS(500))` instead of busy-wait loops.

\- Runs as a FreeRTOS task inside `app\_main`.

\- Prints `LED ON` / `LED OFF` messages on the serial monitor.



---



\## Hardware



\- ESP32-S3 dev board

\- 1× LED (red, yellow, etc.)

\- 1× resistor: 100–220 Ω

\- Breadboard and jumper wires



---



\## Wiring



\- `GPIO 4` → resistor (100–220 Ω) → LED anode (long leg)

\- LED cathode (short leg / flat side) → GND



If the LED is very dim, you can use a smaller resistor (for example 100 Ω) to increase current.  

Make sure the LED polarity is correct: long leg to GPIO (through the resistor), short leg to GND.



If you add a wiring photo or diagram, place it under a `docs` folder like this:



```text

01LEDBLINK/

├─ main/

│  └─ main.c

├─ docs/

│  └─ wiring.jpg

└─ README.md



