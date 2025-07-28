#pragma once

#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace monk_plant_monitor {

class MonkPlantMonitor : public esphome::PollingComponent, public esphome::uart::UARTDevice {
 public:
  void set_soil_sensor(esphome::sensor::Sensor *s) { soil_moisture = s; }
  void set_temp_sensor(esphome::sensor::Sensor *s) { temperature = s; }
  void set_humidity_sensor(esphome::sensor::Sensor *s) { humidity = s; }


  MonkPlantMonitor(); // Default constructor
  explicit MonkPlantMonitor(esphome::uart::UARTComponent *parent);

  void setup() override;
  void update() override;

 protected:
  void request_reading(char cmd);
  float read_float_response();
  float parse_float(const std::string &text);
  bool process_j_response(const std::string &text);
  void setLed(bool enable);
  esphome::sensor::Sensor *soil_moisture{nullptr};
  esphome::sensor::Sensor *temperature{nullptr};
  esphome::sensor::Sensor *humidity{nullptr};
};

}  // namespace monk_plant_monitor
}  // namespace esphome
