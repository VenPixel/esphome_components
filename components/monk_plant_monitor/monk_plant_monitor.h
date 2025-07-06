#pragma once

#include "esphome.h"

class MonkPlantMonitor : public PollingComponent, public UARTDevice {
 public:
  void set_soil_sensor(Sensor *s) { soil_sensor = s; }
  void set_temp_sensor(Sensor *s) { temp_sensor = s; }
  void set_humidity_sensor(Sensor *s) { humidity_sensor = s; }


  explicit MonkPlantMonitor(UARTComponent *parent);

  void setup() override;
  void update() override;

 protected:
  void request_reading(char cmd);
  float read_float_response();
  float parse_float(const std::string &text);
};
