# ESPHome Components
Collection of ESPHome components

## Monk Makes Plant Monitor

This component provides support for the Monk Makes Plant Monitor sensor, which measures soil moisture, temperature, and humidity.

### Hardware Setup

Connect the Monk Makes Plant Monitor to your ESP device using a UART connection:
- Connect the TX pin of the sensor to the RX pin of your ESP device
- Connect the RX pin of the sensor to the TX pin of your ESP device
- Connect power (VCC and GND)

### Configuration

```yaml
# Example configuration
esphome:
  name: plant_monitor
  includes:
    - components/monk_plant_monitor/monk_plant_monitor.h

external_components:
  - source: ./components
    components: [monk_plant_monitor]

uart:
  id: uart_bus
  tx_pin: GPIO17  # Connect to RX on the sensor
  rx_pin: GPIO16  # Connect to TX on the sensor
  baud_rate: 9600

monk_plant_monitor:
  soil_moisture:
    name: "Soil Moisture"
    unit_of_measurement: "%"
    accuracy_decimals: 0
  temperature:
    name: "Temperature"
    unit_of_measurement: "°C"
    accuracy_decimals: 1
  humidity:
    name: "Humidity"
    unit_of_measurement: "%"
    accuracy_decimals: 1
```

### How It Works

The component communicates with the Monk Makes Plant Monitor using serial commands:
- 'w' to request soil moisture (wetness)
- 't' to request temperature
- 'h' to request humidity
- 'j' to request all values at once

The sensor responds with values in the format `<command>=<value>`, for example:
- `w=75.5` for soil moisture
- `t=23.4` for temperature
- `h=45.6` for humidity

The component first tries to get all values at once using the 'j' command for efficiency, and falls back to individual requests if needed.
