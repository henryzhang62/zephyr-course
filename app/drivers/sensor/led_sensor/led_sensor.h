#ifndef HENRY_LED_SENSOR_H_
#define HENRY_LED_SENSOR_H_

#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

__subsystem struct led_sensor_driver_api {
	struct sensor_driver_api sensor_api;
	int (*set_period_ms)(const struct device *dev, uint32_t period_ms);
	uint32_t (*get_period_ms)(const struct device *dev);
};

DEVICE_API_EXTENDS(led_sensor, sensor, sensor_api);

static inline int led_sensor_set_period_ms(const struct device *dev, uint32_t period_ms)
{
	return DEVICE_API_GET(led_sensor, dev)->set_period_ms(dev, period_ms);
}

static inline uint32_t led_sensor_get_period_ms(const struct device *dev)
{
	return DEVICE_API_GET(led_sensor, dev)->get_period_ms(dev);
}

#endif
