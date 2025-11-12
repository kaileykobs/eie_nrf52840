#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

#define SLEEP_MS 100

// Button setup
#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);

// LED setup
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

int main(void) {
    // int ret;

    if (!gpio_is_ready_dt(&button0)) {
        printk("Error: button0 not ready\n");
        return 0;
    }
    if (!gpio_is_ready_dt(&led0)) {
        printk("Error: led0 not ready\n");
        return 0;
    }
    if (!gpio_is_ready_dt(&button1)) {
        printk("Error: button1 not ready\n");
        return 0;
    }
    if (!gpio_is_ready_dt(&led1)) {
        printk("Error: led1 not ready\n");
        return 0;
    }

    // Configure pins
    gpio_pin_configure_dt(&button0, GPIO_INPUT);
    gpio_pin_configure_dt(&button1, GPIO_INPUT);
    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);

    printk("Button+LED test started\n");

    while (1) {
        int val0 = gpio_pin_get_dt(&button0);
        int val1 = gpio_pin_get_dt(&button1);

        if (val0 > 0) {
            gpio_pin_set_dt(&led0, 1);
            k_msleep(100);
            gpio_pin_set_dt(&led0, 0);
            printk("BTN0 pressed!\n");
        }

        if (val1 > 0) {
            gpio_pin_set_dt(&led1, 1);
            k_msleep(100);
            gpio_pin_set_dt(&led1, 0);
            printk("BTN1 pressed!\n");
        }

        k_msleep(SLEEP_MS);
    }
}



  //pwm
  // uint8_t current_duty_cycle = 0;

  // LED_pwm(LED0, current_duty_cycle);


  // while(1) {
  //   if (BTN_check_clear_pressed(BTN0)) {
  //     current_duty_cycle = (current_duty_cycle >= 100) ? 0 : (current_duty_cycle + 10);
  //     printk("setting LED0 to %d%% brightness.\n", current_duty_cycle);
  //     LED_pwm(LED0, current_duty_cycle);
  //   }
  //   k_msleep(SLEEP_MS);
  // }
	// return 0;

