#include "esphome/core/log.h"
#include "monk_plant_monitor.h"

namespace esphome {
namespace MonkPlantMonitor {

static const char *TAG = "monk_plant_monitor.sensor";

void MonkPlantMonitor::setup() {
  // No special setup needed
}

void MonkPlantMonitor::update() {
  // request_reading('w');  // Soil moisture
  // delay(50);
  // float soil = read_float_response();
  // if (!isnan(soil)) soil_sensor->publish_state(soil);

  // request_reading('t');  // Temperature
  // delay(50);
  // float temp = read_float_response();
  // if (!isnan(temp)) temp_sensor->publish_state(temp);

  // request_reading('h');  // Humidity
  // delay(50);
  // float hum = read_float_response();
  // if (!isnan(hum)) humidity_sensor->publish_state(hum);
}

void MonkPlantMonitor::loop() {

}

void MonkPlantMonitor::dump_config(){
  ESP_LOGCONFIG(TAG, "Monk Plant Monitor");
  ESP_LOGCONFIG(TAG, "  soil = %i", soil_sensor.state);
}

// void MonkPlantMonitor::request_reading(char cmd) {
//   write(cmd);
// }

// float MonkPlantMonitor::read_float_response() {
//   std::string result;
//   unsigned long start = millis();

//   while (millis() - start < 500) {  // 500ms timeout
//     while (available()) {
//       char c = read();
//       if (c == '\n') {
//         return parse_float(result);
//       } else {
//         result += c;
//       }
//     }
//     delay(10);
//   }

//   ESP_LOGW("MonkPlantMonitor", "Timeout waiting for response");
//   return NAN;
// }

// float MonkPlantMonitor::parse_float(const std::string &text) {
//   float value = NAN;
//   if (sscanf(text.c_str(), "%f", &value) == 1) {
//     return value;
//   } else {
//     ESP_LOGW("MonkPlantMonitor", "Invalid float response: %s", text.c_str());
//     return NAN;
//   }
// }

}  // namespace MonkPlantMonitor
}  // namespace esphome
