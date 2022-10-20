# README

## Installation

- Download and install the Arduino IDE [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
- Set up Arduino IDE for the Wio Terminal [https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/#software](https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/#software)
- Install the following libraries in the Arduino IDE
  - `seeed rpcwifi`
  - `seeed rpcunified`
  - `seeed mbedtls`
  - `seeed fs`
  - `seeed sfud`
  - `LIS3DHTR`
  - [https://github.com/knolleary/pubsubclient](https://github.com/knolleary/pubsubclient)
- Update the firmware of the Realtek RTL8720 Wireless core [https://wiki.seeedstudio.com/Wio-Terminal-Network-Overview/#update-the-wireless-core-firmware](https://wiki.seeedstudio.com/Wio-Terminal-Network-Overview/#update-the-wireless-core-firmware)

## MQTT

### Subscribe

| Topic                 | Description             | Example                            |
| --------------------- | ----------------------- | ---------------------------------- |
| `msg/0`               | Broadcast message topic | `"Hallo Welt"`                     |
| `msg/<wio-id>`        | Wio specific message    | `"Hallo Welt"`                     |

### Publish

| Topic                 | Description             | Example                            |
| --------------------- | ----------------------- | ---------------------------------- |
| `tele/<wio-id>/acc`   | Acceleration data       | `{"x": 0.04,"y": 0.04,"z": -0.95}` |
| `tele/<wio-id>/light` | Light data              | `904`                              |
