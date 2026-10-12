# Smart AC Hemat Energi

## Background
Smart AC Hemat Energi is an IoT starter project for controlling an air conditioner with an ESP32.  
The AC should only run when the room is occupied and the temperature is hot enough.  
> Placeholder: add business/problem context details.

## Features
- ESP32-based controller scaffold (PlatformIO + Arduino framework)
- DHT22 temperature/humidity sensor integration point (GPIO 4)
- PIR motion sensor integration point (GPIO 5)
- Relay actuator integration point (GPIO 18)
- WiFi and MQTT-ready project structure for dashboard communication

## Components
| Component | Model/Type | GPIO | Role |
|---|---|---:|---|
| Microcontroller | ESP32 Dev Board | - | Main controller |
| Temperature/Humidity Sensor | DHT22 | 4 | Read room temperature and humidity |
| Motion Sensor | PIR | 5 | Detect room occupancy |
| Actuator | Relay Module | 18 | Switch AC power/control line |
| MQTT Broker | broker.hivemq.com | - | Publish/subscribe messaging |

## How It Works
1. ESP32 reads occupancy and environmental conditions.  
2. ESP32 decides AC state based on occupancy + heat condition.  
3. ESP32 publishes status and receives control/config via MQTT.  
4. Dashboard displays telemetry and sends commands.  
> Placeholder: add final control-flow/state details.

## MQTT Topics
> Placeholder: finalize topic naming convention.

Suggested starter topics:
- `smartac/telemetry/temperature`
- `smartac/telemetry/humidity`
- `smartac/telemetry/occupancy`
- `smartac/state/relay`
- `smartac/cmd/relay`

## Setup
1. Install [PlatformIO](https://platformio.org/) in VS Code.
2. Clone this repository.
3. Open the project folder in VS Code.
4. Build/upload using PlatformIO.
5. Connect hardware according to the component GPIO mapping above.
> Placeholder: add SSID/MQTT credential setup details if needed.

## Demo Steps
1. Power the ESP32 and connect to WiFi.
2. Open your MQTT client/dashboard.
3. Observe telemetry topic updates.
4. Trigger occupancy/temperature conditions and observe relay state updates.
> Placeholder: add exact demo scenario and expected values.

## Limitations
- Initial scaffold only (no control logic implemented yet)
- Placeholder documentation sections still need project-specific details

## Future Work
- Implement sensor reading and filtering
- Implement occupancy + temperature decision logic
- Add robust MQTT reconnection and LWT handling
- Build dashboard interactions and visualization
- Add automated tests/simulation scenarios
