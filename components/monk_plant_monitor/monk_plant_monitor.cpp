#include "monk_plant_monitor.h"

namespace esphome {
namespace monk_plant_monitor {

MonkPlantMonitor::MonkPlantMonitor()
    : PollingComponent(60 * 1000) {
    // Default constructor - UART not initialized yet
    uart_initialized_ = false;
    // Call setup in constructor to ensure LED is disabled when component is created
    setup();
} // Default update interval of 60 seconds

MonkPlantMonitor::MonkPlantMonitor(uart::UARTComponent *parent)
    : PollingComponent(60 * 1000), uart::UARTDevice(parent) {
    // UART is initialized in this constructor
    uart_initialized_ = true;
    // Call setup in constructor to ensure LED is disabled when component is created
    setup();
}

void MonkPlantMonitor::setup() {
    // Disable LED on boot up
//    ::delay(100);
    ESP_LOGD("MonkPlantMonitor", "Setting up MonkPlantMonitor device");
//    if (uart_initialized_) {
//        setLed(false);
//    } else {
        // UART not initialized yet, set flag to disable LED once it's available
//        ESP_LOGD("MonkPlantMonitor", "UART not initialized, will disable LED when available");
//        need_disable_led_ = true;
//    }
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
    
    // Only write if the UART device has been initialized
//    if (uart_initialized_) {
        write(cmd);
//    } else {
//        ESP_LOGW("MonkPlantMonitor", "UART device not initialized, cannot set LED state");
//    }
    need_disable_led_ = false;
    ESP_LOGD("MonkPlantMonitor", "LED Set");
}

void MonkPlantMonitor::update() {
  // Check if we need to disable the LED and UART is now initialized
  if (need_disable_led_) {
    ESP_LOGD("MonkPlantMonitor", "[UPDATE] disabling LED");
    setLed(false);
    need_disable_led_ = false;
  } else {
    ESP_LOGW("MonkPlantMonitor", "LED still not disabled");
    if(!uart_initialized_) {
        ESP_LOGW("MonkPlantMonitor", "UART still not initialized");
    }
    if(need_disable_led_) {
        ESP_LOGW("MonkPlantMonitor", "Still need to disable LED");
    }
  }

  // Try to get all values at once using 'j' command
//  ESP_LOGD("MonkPlantMonitor", "Requesting all values");
//  request_reading('j');
//  ::delay(100);
//
//  std::string result;
//  unsigned long start = ::millis();
//  bool success = false;
//
//  while (::millis() - start < 1000) {  // 1000ms timeout for all values
//    while (available()) {
//      char c = read();
//      if (c == '\n') {
//        success = process_j_response(result);
//        break;
//      } else {
//        result += c;
//      }
//    }
//
//    if (success) break;
//    ::delay(10);
//  }

  // If 'j' command failed, fall back to individual requests
//  if (!success) {
//    ESP_LOGW("MonkPlantMonitor", "Failed to get all values at once, falling back to individual requests");

    ESP_LOGD("MonkPlantMonitor", "Requesting moisture value");
    request_reading('w');  // Soil moisture
    ::delay(50);
    std::string result;
    unsigned long start = ::millis();
    
    while (::millis() - start < 500) {  // 500ms timeout
      while (available()) {
        char c = read();
        if (c == '\n') {
          ESP_LOGW("MonkPlantMonitor", "Raw soil moisture response: '%s'", result.c_str());
          int soil = parse_soil_moisture(result);
          if (soil >= 0) soil_moisture->publish_state(soil);
          break;
        } else {
          result += c;
        }
      }
      if (!result.empty() && result.find('\n') != std::string::npos) break;
      ::delay(10);
    }
    
    if (result.empty()) {
      ESP_LOGW("MonkPlantMonitor", "Timeout waiting for soil moisture response");
    }

    ESP_LOGD("MonkPlantMonitor", "Requesting temperature value");
    request_reading('t');  // Temperature
    ::delay(50);
    float temp = read_float_response();
    if (!isnan(temp)) temperature->publish_state(temp);

    ESP_LOGD("MonkPlantMonitor", "Requesting humidity value");
    request_reading('h');  // Humidity
    ::delay(50);
    float hum = read_float_response();
    if (!isnan(hum)) humidity->publish_state(hum);
//  }
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
        ESP_LOGW("MonkPlantMonitor", "Raw response: '%s'", result.c_str());
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

int MonkPlantMonitor::parse_soil_moisture(const std::string &text) {
  int value = -1;  // Default error value
  // Format is expected to be "w=<value>" (e.g., "w=75")
  size_t equals_pos = text.find('=');
  
  if (equals_pos != std::string::npos && equals_pos + 1 < text.length()) {
    std::string value_str = text.substr(equals_pos + 1);
    if (sscanf(value_str.c_str(), "%d", &value) == 1) {
      // Ensure value is within 0-255 range
      if (value < 0) value = 0;
      if (value > 255) value = 255;
      return value;
    }
  }
  
  ESP_LOGW("MonkPlantMonitor", "Invalid soil moisture response format: %s", text.c_str());
  return -1;  // Error value
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
    
    int soil = parse_soil_moisture(w_value_str);
    if (soil >= 0) {
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
