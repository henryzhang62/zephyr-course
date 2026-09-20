#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static const struct device *const led_sensor = DEVICE_DT_GET(DT_NODELABEL(led_sensor));

int main(void)
{
	struct sensor_value value;
	int ret;

	if (!IS_ENABLED(CONFIG_LED_SUBSYSTEM)) {
		return 0;
	}

	if (!device_is_ready(led_sensor)) {
		printk("LED sensor is not ready\n");
		return 0;
	}

	while (true) {
		ret = sensor_sample_fetch(led_sensor);
		if (ret < 0) {
			printk("LED on failed: %d\n", ret);
			return 0;
		}
		printk("LED on\n");
		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

		ret = sensor_channel_get(led_sensor, SENSOR_CHAN_LIGHT, &value);
		if (ret < 0) {
			printk("LED off failed: %d\n", ret);
			return 0;
		}
		printk("LED off\n");
		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
	}
}
