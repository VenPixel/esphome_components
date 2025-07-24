#pragma once

#include "esphome.h"

class MonkPlantMonitor : public PollingComponent, public UARTDevice {
 public:
  void set_soil_sensor(Sensor *s) { soil_moisture = s; }
  void set_temp_sensor(Sensor *s) { temperature = s; }
  void set_humidity_sensor(Sensor *s) { humidity = s; }


  explicit MonkPlantMonitor(UARTComponent *parent);

  void setup() override;
  void update() override;

 protected:
  void request_reading(char cmd);
  float read_float_response();
  float parse_float(const std::string &text);
  bool process_j_response(const std::string &text);
  Sensor *soil_moisture{nullptr};
  Sensor *temperature{nullptr};
  Sensor *humidity{nullptr};
};
