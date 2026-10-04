#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_string_conv.h>

#include "led_sensor.h"
 
static const struct device *const led_sensor = DEVICE_DT_GET(DT_NODELABEL(led_sensor));

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(led_sensor)) {
		shell_error(sh, "Sensor device is not ready");
		return -ENODEV;
	}

	ret = sensor_sample_fetch(led_sensor);
	if (ret < 0) {
		shell_error(sh, "sensor_sample_fetch failed: %d", ret);
		return ret;
	}

	shell_print(sh, "Sample fetched");
	return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	struct sensor_value value;
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(led_sensor)) {
		shell_error(sh, "Sensor device is not ready");
		return -ENODEV;
	}

	ret = sensor_channel_get(led_sensor, SENSOR_CHAN_LIGHT, &value);
	if (ret < 0) {
		shell_error(sh, "sensor_channel_get failed: %d", ret);
		return ret;
	}

	shell_print(sh, "Result: %d.%06d", value.val1, value.val2);
	return 0;
}

static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
	unsigned long period_ms;
	int err;
	int ret;

	if (argc != 2) {
		shell_error(sh, "Usage: sensorroot set <period_ms>");
		return -EINVAL;
	}

	period_ms = shell_strtoul(argv[1], 10, &err);
	if (err != 0 || period_ms < 100 || period_ms > 2000) {
		shell_error(sh, "Period must be an integer from 100 to 2000 ms");
		return -EINVAL;
	}

	if (!device_is_ready(led_sensor)) {
		shell_error(sh, "Sensor device is not ready");
		return -ENODEV;
	}

	ret = led_sensor_set_period_ms(led_sensor, (uint32_t)period_ms);
	if (ret < 0) {
		shell_error(sh, "Failed to set period: %d", ret);
		return ret;
	}

	shell_print(sh, "LED period set to %lu ms", period_ms);
	return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "Device: %s", led_sensor->name);
	shell_print(sh, "Ready: %s", device_is_ready(led_sensor) ? "yes" : "no");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensorroot_subcommands,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch()", cmd_sensor_fetch),
	SHELL_CMD(read, NULL, "Call sensor_channel_get() and print the result", cmd_sensor_read),
	SHELL_CMD(info, NULL, "Print the sensor device name and ready state", cmd_sensor_info),
	SHELL_CMD_ARG(set, NULL, "Set LED period in milliseconds", cmd_sensor_set, 2, 0),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensorroot, &sensorroot_subcommands, "LED sensor commands", NULL);
