/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 100
//button set up
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
//led set up
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);


int main(void) {
  int ret;

  if (!gpio_is_ready_dt(&button0)) {
    printk("Erorr: button not ready\n");
    return 0;
  }
  if (!gpio_is_ready_dt(&led0)) {
    printk("Error: led0 device not ready\n");
    return 0;
  }
  //config pins
  ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
  if (0 > ret) {
    printk("Error config btn0\n");
    return 0;
  }
    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
  if (ret < 0) {
    printk("Error config led0\n");
    return 0;
  }

  //delete later
  printk("Button+LED started\n");

  // if (0 > BTN_init()) {
  //   return 0;
  // }
  // if (0 > LED_init()) {
  //   return 0;
  // }

  while(1) {
    int val = gpio_pin_get_dt(&button0);

    if (val > 0) {
      //button pressed blinks once led0
      gpio_pin_set_dt(&led0, 1);
            k_msleep(100);     // LED on for 100 ms
            gpio_pin_set_dt(&led0, 0);
            k_msleep(100);     // LED off for 100 ms
            printk("BTN0 pressed!\n");
        } else {
            // Not pressed → keep LED off
            gpio_pin_set_dt(&led0, 0);
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

