import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ID, CONF_NAME, UNIT_PERCENT, UNIT_CELSIUS,
    ICON_WATER_PERCENT, ICON_THERMOMETER, ICON_WATER,
    DEVICE_CLASS_HUMIDITY, DEVICE_CLASS_TEMPERATURE
)

CODEOWNERS = ["@VenPixel"]

monk_ns = cg.esphome_ns.namespace("monk_plant_monitor")
MonkPlantMonitor = monk_ns.class_("MonkPlantMonitor", cg.PollingComponent, uart.UARTDevice)

CONF_SOIL = "soil_moisture"
CONF_TEMP = "temperature"
CONF_HUM = "humidity"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MonkPlantMonitor),
    cv.Required(CONF_SOIL): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        icon=ICON_WATER_PERCENT,
        accuracy_decimals=0
    ),
    cv.Required(CONF_TEMP): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE
    ),
    cv.Required(CONF_HUM): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        icon=ICON_WATER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_HUMIDITY
    )
}).extend(cv.polling_component_schema("60s")).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    soil = await sensor.new_sensor(config[CONF_SOIL])
    temp = await sensor.new_sensor(config[CONF_TEMP])
    hum = await sensor.new_sensor(config[CONF_HUM])

    cg.add(var.set_soil_sensor(soil))
    cg.add(var.set_temp_sensor(temp))
    cg.add(var.set_humidity_sensor(hum))
