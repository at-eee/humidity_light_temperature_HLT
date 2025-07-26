# HLT (Humidity, Light, Temperature) measuring prototype device

## Prerequisites

### Components and parts needed:

- Arduino Uno (or its clone),
- 3461BS LED display (4-digit, 7-segment (with dot) with common anode (CA)),
- SN74LS138N (3-to-8 demultiplexer of cat. number 138) demultiplexer,
- DHT11 (humidity & temperature) sensor
- 4 NPN transistors,
- 5 resistors 10k&Omega;,
- 5 resistors 1k&Omega;,
- 1 photoresistor,
- 1 button (preferably switch or "by default off" button),
- wires,
- any USB-A-USB-B cable.

### Software

- Arduino IDE (or similar) (to compile and upload the source code ("hlt.ino" file) to Arduino Uno),
- Downloaded libraries:
  - "DHT sensor library" by Adafruit,
  - "Adafruit Unified Sensor" (which is the prerequisite of the former).

#### Simple instruction for library download:

It's easy to download in Arduino IDE:

``` Select: Sketch -> Include Library -> Manage Libraries... ```

And then look for "DHT sensor library" and "Adafruit Unified Sensor" libraries and download them.

## Demos
### Example physical representation and functionality showcase
---
#### Quick overview:

![overview1-ezgif com-optimize](https://github.com/user-attachments/assets/5e63d370-fb26-48ec-bdc1-45ceec46c801)

Excuse the look (even when prototype I try to keep things clean/aesthetic) tried best to make it look clean as possible (see rubber bands) but sadly, back then we didn't have the short more aesthetic (pre-bent) jumper wires...

#### Better, more detailed look at some components from up-close

![overview2-ezgif com-optimize](https://github.com/user-attachments/assets/7db2e0d1-ee93-4653-bc91-11f0cd771ec3)

---

#### Temperature mode showcase

![temperature_mode-ezgif com-optimize](https://github.com/user-attachments/assets/b7e0d5ad-2531-4959-a19d-fd2e1c74dab4)

#### Humidity level mode showcase

![humidity_mode-ezgif com-optimize](https://github.com/user-attachments/assets/3af9655b-5c93-420a-824d-0e48d7f4872f)

(both temperature and humidity level showcase gifs were sped up by x5, so it gives clear signs that in our DHT humidity sensor adapts to changes faster than the temperatue sensor)

#### Light level mode showcase

![light_mode_newer-ezgif com-optimize](https://github.com/user-attachments/assets/9bd5ada8-16d5-4bcd-b6ae-0fb4a6f05509)

(last gif (this one - light level mode showcase) was sped up only x2) <br>
(Yes, in general light level mode is much more responsive (due to the structure and nature of DHT11 sensors module's sensors vs photoresistor sensor)

(Also yes; All the video speed ups were due to GitHub's 10MB limit to every media added to "readme.md") <br>
(If you want you can contact me for full materials/videos)

## This electronic circuit's/prototype's schematic

<img width="1270" height="1025" alt="Screenshot from 2025-02-05 18-52-37" src="https://github.com/user-attachments/assets/b2bbab66-bec2-41cb-96da-92fa782a8080" />

(The 4 transistors seen in the center of the image are makeshift "NOT" logic gates) <br>

<i>Made in falstad</i> (the file of electronic circuit is to be found in <code>/falstad/hlt_circuit.txt</code>) - Just be aware that the circuit in falstad works a little different (because there isn't a direct counterpart of 138 demux and), used demultiplexer from falstad returns <i>HIGH</i> on the selected pin wheras (our) 138 demux returns <i>LOW</i> on the selected pin.
