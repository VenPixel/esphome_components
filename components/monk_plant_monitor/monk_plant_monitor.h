#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace MonkPlantMonitor {

class MonkPlantMonitor : public sensor::Sensor public PollingComponent, public uart::UARTDevice {
 public:
  void set_soil_sensor(Sensor *s) { soil_sensor = s; }
  void set_temp_sensor(Sensor *s) { temp_sensor = s; }
  void set_humidity_sensor(Sensor *s) { humidity_sensor = s; }


  explicit MonkPlantMonitor(UARTComponent *parent);

  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;

 protected:
  void request_reading(char cmd);
  float read_float_response();
  float parse_float(const std::string &text);
};

}  // namespace MonkPlantMonitor
}  // namespace esphome