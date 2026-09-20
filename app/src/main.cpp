#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(app_led), gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    if (!IS_ENABLED(CONFIG_LED_SUBSYSTEM)) return 0;

    bool led_state = false;
    int ret;

    if (!gpio_is_ready_dt(&led)) {
        printk("Heartbeat LED GPIO controller is not ready\n");
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("Heartbeat LED configuration failed: %d\n", ret);
        return 0;
    }

    while (1) {
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0) {
            printk("Heartbeat LED toggle failed: %d\n", ret);
            return 0;
        }

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
