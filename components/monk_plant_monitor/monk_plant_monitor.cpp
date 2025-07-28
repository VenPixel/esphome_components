#include "monk_plant_monitor.h"

namespace esphome {
namespace monk_plant_monitor {

MonkPlantMonitor::MonkPlantMonitor()
    : PollingComponent(60 * 1000) {} // Default update interval of 60 seconds

MonkPlantMonitor::MonkPlantMonitor(uart::UARTComponent *parent)
    : PollingComponent(60 * 1000), uart::UARTDevice(parent) {}

void MonkPlantMonitor::setup() {
    // Disable LED on boot up
    ESP_LOGD("MonkPlantMonitor", "Setting up MonkPlantMonitor device");
    setLed(false);
    ESP_LOGD("MonkPlantMonitor", "MonkPlantMonitor device ready");
}

void MonkPlantMonitor::setLed(bool enable) {
    char cmd = 'l';
    ESP_LOGD("MonkPlantMonitor", "setLED");
    if(enable) {
        cmd = 'L';
        ESP_LOGD("MonkPlantMonitor", "Enabling LED");
    } else {
        ESP_LOGD("MonkPlantMonitor", "Disabling LED");
    }
    write(cmd);
}

void MonkPlantMonitor::update() {
  // Try to get all values at once using 'j' command
  ESP_LOGD("MonkPlantMonitor", "Requesting all values");
  request_reading('j');
  ::delay(100);
  
  std::string result;
  unsigned long start = ::millis();
  bool success = false;

  while (::millis() - start < 1000) {  // 1000ms timeout for all values
    while (available()) {
      char c = read();
      if (c == '\n') {
        success = process_j_response(result);
        break;
      } else {
        result += c;
      }
    }
    
    if (success) break;
    ::delay(10);
  }

  // If 'j' command failed, fall back to individual requests
  if (!success) {
    ESP_LOGW("MonkPlantMonitor", "Failed to get all values at once, falling back to individual requests");
    
    request_reading('w');  // Soil moisture
    ::delay(50);
    float soil = read_float_response();
    if (!isnan(soil)) soil_moisture->publish_state(soil);

    request_reading('t');  // Temperature
    ::delay(50);
    float temp = read_float_response();
    if (!isnan(temp)) temperature->publish_state(temp);

    request_reading('h');  // Humidity
    ::delay(50);
    float hum = read_float_response();
    if (!isnan(hum)) humidity->publish_state(hum);
  }
}

void MonkPlantMonitor::request_reading(char cmd) {
  write(cmd);
}

float MonkPlantMonitor::read_float_response() {
  std::string result;
  unsigned long start = ::millis();

  while (::millis() - start < 500) {  // 500ms timeout
    while (available()) {
      char c = read();
      if (c == '\n') {
        return parse_float(result);
      } else {
        result += c;
      }
    }
    ::delay(10);
  }

  ESP_LOGW("MonkPlantMonitor", "Timeout waiting for response");
  return NAN;
}

float MonkPlantMonitor::parse_float(const std::string &text) {
  float value = NAN;
  // Format is expected to be "<command>=<value>" (e.g., "w=75.5")
  size_t equals_pos = text.find('=');
  
  if (equals_pos != std::string::npos && equals_pos + 1 < text.length()) {
    std::string value_str = text.substr(equals_pos + 1);
    if (sscanf(value_str.c_str(), "%f", &value) == 1) {
      return value;
    }
  }
  
  ESP_LOGW("MonkPlantMonitor", "Invalid response format: %s", text.c_str());
  return NAN;
}

bool MonkPlantMonitor::process_j_response(const std::string &text) {
  // The 'j' command should return all values in a format like "w=75.5,t=23.4,h=45.6"
  // or possibly as separate lines
  
  ESP_LOGD("MonkPlantMonitor", "Processing 'j' response: %s", text.c_str());
  
  bool found_any = false;
  std::string remaining = text;
  
  // Look for soil moisture value (w=XX)
  size_t w_pos = remaining.find("w=");
  if (w_pos != std::string::npos) {
    // Extract the value after "w=" until the next comma or end of string
    size_t comma_pos = remaining.find(',', w_pos);
    std::string w_value_str;
    
    if (comma_pos != std::string::npos) {
      w_value_str = remaining.substr(w_pos, comma_pos - w_pos);
    } else {
      w_value_str = remaining.substr(w_pos);
    }
    
    float soil = parse_float(w_value_str);
    if (!isnan(soil)) {
      soil_moisture->publish_state(soil);
      found_any = true;
    }
  }
  
  // Look for temperature value (t=XX)
  size_t t_pos = remaining.find("t=");
  if (t_pos != std::string::npos) {
    // Extract the value after "t=" until the next comma or end of string
    size_t comma_pos = remaining.find(',', t_pos);
    std::string t_value_str;
    
    if (comma_pos != std::string::npos) {
      t_value_str = remaining.substr(t_pos, comma_pos - t_pos);
    } else {
      t_value_str = remaining.substr(t_pos);
    }
    
    float temp = parse_float(t_value_str);
    if (!isnan(temp)) {
      temperature->publish_state(temp);
      found_any = true;
    }
  }
  
  // Look for humidity value (h=XX)
  size_t h_pos = remaining.find("h=");
  if (h_pos != std::string::npos) {
    // Extract the value after "h=" until the next comma or end of string
    size_t comma_pos = remaining.find(',', h_pos);
    std::string h_value_str;
    
    if (comma_pos != std::string::npos) {
      h_value_str = remaining.substr(h_pos, comma_pos - h_pos);
    } else {
      h_value_str = remaining.substr(h_pos);
    }
    
    float hum = parse_float(h_value_str);
    if (!isnan(hum)) {
      humidity->publish_state(hum);
      found_any = true;
    }
  }
  
  return found_any;
}

}  // namespace monk_plant_monitor
}  // namespace esphome
