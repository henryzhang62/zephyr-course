#define DT_DRV_COMPAT henry_led_sensor

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

struct led_sensor_config {
	struct gpio_dt_spec led;
};

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

	if (!gpio_is_ready_dt(&cfg->led)) {
		return -ENODEV;
	}

	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

static DEVICE_API(sensor, led_sensor_api) = {
	.sample_fetch = led_sensor_sample_fetch,
	.channel_get = led_sensor_channel_get,
};

#define LED_SENSOR_DEFINE(inst) \
	static const struct led_sensor_config led_sensor_config_##inst = { \
		.led = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(inst, led), gpios), \
	}; \
	SENSOR_DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL, NULL, \
				     &led_sensor_config_##inst, POST_KERNEL, \
				     CONFIG_SENSOR_INIT_PRIORITY, &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)
