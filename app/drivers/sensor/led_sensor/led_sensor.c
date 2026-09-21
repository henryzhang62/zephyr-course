#define DT_DRV_COMPAT henry_led_sensor

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#include "led_sensor.h"

struct led_sensor_config {
	struct gpio_dt_spec led;
};

struct led_sensor_data {
	uint32_t period_ms;
};

static int led_sensor_set_period_impl(const struct device *dev, uint32_t period_ms)
{
	struct led_sensor_data *data = dev->data;

	if (period_ms < 100 || period_ms > 2000) {
		return -EINVAL;
	}

	data->period_ms = period_ms;
	return 0;
}

static uint32_t led_sensor_get_period_impl(const struct device *dev)
{
	const struct led_sensor_data *data = dev->data;

	return data->period_ms;
}

static int led_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct led_sensor_config *cfg = dev->config;

	if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_LIGHT) {
		return -ENOTSUP;
	}

	return gpio_pin_set_dt(&cfg->led, 1);
}

static int led_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
				  struct sensor_value *value)
{
	const struct led_sensor_config *cfg = dev->config;
	int ret;

	if (chan != SENSOR_CHAN_LIGHT) {
		return -ENOTSUP;
	}
	if (value == NULL) {
		return -EINVAL;
	}

	ret = gpio_pin_set_dt(&cfg->led, 0);
	if (ret < 0) {
		return ret;
	}

	value->val1 = 0;
	value->val2 = 0;
	return 0;
}

static int led_sensor_init(const struct device *dev)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;

	if (!gpio_is_ready_dt(&cfg->led)) {
		return -ENODEV;
	}

	data->period_ms = 1000;
	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

static DEVICE_API(led_sensor, led_sensor_api) = {
	.sensor_api = {
		.sample_fetch = led_sensor_sample_fetch,
		.channel_get = led_sensor_channel_get,
	},
	.set_period_ms = led_sensor_set_period_impl,
	.get_period_ms = led_sensor_get_period_impl,
};

#define LED_SENSOR_DEFINE(inst) \
	static struct led_sensor_data led_sensor_data_##inst; \
	static const struct led_sensor_config led_sensor_config_##inst = { \
		.led = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(inst, led), gpios), \
	}; \
	SENSOR_DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL, &led_sensor_data_##inst, \
				     &led_sensor_config_##inst, POST_KERNEL, \
				     CONFIG_SENSOR_INIT_PRIORITY, &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)
