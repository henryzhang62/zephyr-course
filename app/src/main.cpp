#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#define LED_PIN 2

static const struct device *const gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    if (!IS_ENABLED(CONFIG_LED_SUBSYSTEM)) return 0;

    bool led_state = false;
    int ret;

    if (!device_is_ready(gpio)) {
        printk("GPIO controller is not ready\n");
        return 0;
    }

    ret = gpio_pin_configure(gpio, LED_PIN, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("GPIO2 configuration failed: %d\n", ret);
        return 0;
    }

    while (1) {
        ret = gpio_pin_toggle(gpio, LED_PIN);
        if (ret < 0) {
            printk("GPIO2 toggle failed: %d\n", ret);
            return 0;
        }

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_LED_BLINK_SLEEP_MS);
    }
    return 0;
}
