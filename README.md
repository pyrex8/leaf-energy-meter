# leaf-energy-meter

The Nissan Leaf Energy Meter displays kWh remaining battery energy using data from the Nissan Leafs EV CAN bus.

Display installed in dash
<p align="center">
  <img src="images/installed_in_dash.jpg" alt="finished" width="400"/>

Display explaination
<p align="center">
  <img src="images/energy_display.jpg" alt="energy_display" width="400"/>

Total energy is calculated from remaining energy and state of cahrge CAN bus data:

```total energy capacity = remaining energy / state of charge```

Note: total energy capacity does fluctuate a small amount while driving.


Display fully enclosed in 2011-2012 Nissan Leaf Upper Speedometer Combination Meter Display 24810-3NA1A
<p align="center">
  <img src="images/display_mounted.jpg" alt="display_mounted" width="400"/>

Wires fed through back 
<p align="center">
  <img src="images/display_connector.jpg" alt="connector" width="400"/>

Black painters tape is used to cover the "christmas" tree efficiency indicator
<p align="center">
  <img src="images/display_bezel_black_tape.jpg" alt="mounting" width="400"/>

Display and bezel mounted with Kapton tape
<p align="center">
  <img src="images/display_mounting.jpg" alt="findisplay_mountingished" width="400"/>

Soldering wires to OLED display 
<p align="center">
  <img src="images/display_soldering.jpg" alt="findisplay_mountingished" width="400"/>

Meter PCB in case
<p align="center">
  <img src="images/meter_pcb_in_case.jpg" alt="meter_pcb_in_case" width="400"/>

Code is using arduino
https://www.arduino.cc

<p align="center">
  <img src="images/arduino_editor.png" alt="arduino" width="800"/>

The schematics and pcb layout were made using diptrace. Files in ```pcb``` folder
https://diptrace.com

<p align="center">
  <img src="images/schematics.png" alt="schematics" width="800"/>

### Parts list

| approx. cost each | description | image |
| :---:   | :---: | :---: |
| $5 | Pro Mini Atmega328P 5V 16MHz for Arduino| <img src="images/arduino.png" alt="arduino" width="100"/> |
| $2 | Mini360 MP1584EN 5V Regulator, DC 5-30V to 5V 1.8A Output Buck Converter Board | <img src="images/dc-dc.png" alt="dc-dc" width="100"/> |
| $4 | MCP2515 CAN Bus Module TJA1050 Receiver SPI Module for Arduino | <img src="images/can.png" alt="can" width="50"/> |
| $7 | SSD1306 0.96" 128X64 OLED Display I2C Font Color White | <img src="images/oled.png" alt="oled" width="100"/> |
| $0.5 | JST XH 2.54 4 Pin Connector Plug Male with 200mm Wire & Female Connector | <img src="images/jst.png" alt="jst" width="100"/> |
| $8 | OBDII Male Connector with enclosure | <img src="images/enclosure.png" alt="enclosure" width="100"/> |

The pcb is shared on OSH Park
https://oshpark.com/projects/Czkf2fi5/view_design

<p align="center">
  <img src="images/pcb_top.png" alt="pcb" width="400"/>


### Wiring to OBDII connector
- pin 4, chassis ground
- pin 8, +12V DC when vehicle is powered is on
- pin 12, EV-CAN low
- pin 13, EV-CAN high

<p align="center">
  <img src="images/obd2_male_backshell_wiring.png" alt="enclosure" width="200"/>


bezel is design using OpenSCAD. Files in ```bezel``` folder
https://openscad.org/

<p align="center">
  <img src="images/bezel_openscad.png" alt="bezel" width="800"/>

