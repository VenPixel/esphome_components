# text_sensor that reads lines for a uart. Also, a sensor that reads single values from the uart.

A configured uart is required.

Example:
```yaml
text_sensor:
  - platform: serial_pmon
    uart_id: my_uart   # optional
    name: My Plant Text

sensor:
  - platform: serial_pmon
    uart_id: my_uart   # optional
    name: My Plant Sensor
    sensors:
      - index: 0
        name: First value
      - index: 3
        name: Fourth value
```

Credit goes to https://github.com/ssieb/esphome_components/tree/2e82fc3a5acc3d1f4ca6b47cbe656f4217d382ac/components/serial for the original serial design and scaffolding.