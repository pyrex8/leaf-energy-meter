# leaf-energy-meter


<p float="center">
  <img src="images/installed_in_dash.jpg" alt="finished" width="400"/>



<p float="center">
  <img src="images/display_bezel_black_tape.jpg" alt="mounting" width="400"/>


<p float="center">
  <img src="images/display_connector.jpg" alt="connector" width="400"/>



<p float="center">
  <img src="images/meter_in_case.jpg" alt="meter" width="400"/>



Code is using arduino
https://www.arduino.cc

<p float="center">
  <img src="images/arduino_editor.png" alt="arduino" width="800"/>



The schematics and pcb layout were made using diptrace
https://diptrace.com

<p float="center">
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


<p float="center">
  <img src="images/pcb_top.png" alt="pcb" width="400"/>


### Wiring to OBDII connector
- pin 4, chassis ground
- pin 8, +12V DC when vehicle is powered is on
- pin 12, EV-CAN low
- pin 13, EV-CAN high

<p float="center">
  <img src="images/obd2_male_backshell_wiring.png" alt="enclosure" width="200"/>


bezel is design using OpenSCAD
https://openscad.org/

<p float="center">
  <img src="images/bezel_openscad.png" alt="bezel" width="800"/>

