/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include "BTN.h"
#include "LED.h"


// #define SLEEP_MS 1
// #define SLEEP_TIME_MS 1000
#define PASSWORD_LENGTH 3
// #define SW0_NODE DT_ALIAS(sw0)
// static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

// static struct gpio_callback button_isr_data;

// void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
//   printk("Button 0 pressed!\n");
// }
const int password[PASSWORD_LENGTH] = {BTN0, BTN1, BTN2};
int input[PASSWORD_LENGTH];
int input_index = 0;

bool locked = true;


int main(void) {
  int ret;

  ret = LED_init();
  if (ret < 0) {
    printk("LED failed\n");
    return 0;
  }

  ret = BTN_init();
  if (ret < 0) {
    printk("Button failed\n");
    return 0;
  }

  LED_set(LED0, LED_ON);
  // uint8_t counter = 0;

  // int ret;

  // if(!gpio_is_ready_dt(&button)) {
  //   return 0;
  // }

  // ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
  // if (0 > ret) {
  //   return 0;
  // }

  // ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
  // if (0 > ret) {
  //   return 0;
  // }

  // gpio_init_callback(&button_isr_data, button_isr, BIT(button.pin));
  // gpio_add_callback(button.port, &button_isr_data);

  while(1) {
    if (locked) {
      if (BTN_check_clear_pressed(BTN0) && input_index < PASSWORD_LENGTH) {
        input[input_index++] = BTN0;
        printk("BTN0 pressed, stored at indx %d\n", input_index - 1);
      }
      if (BTN_check_clear_pressed(BTN1) && input_index < PASSWORD_LENGTH) {
        input[input_index++] = BTN1;
        printk("BTN1 pressed, stored at the index %d\n", input_index - 1);
      }
      if (BTN_check_clear_pressed(BTN2) && input_index < PASSWORD_LENGTH) {
        input[input_index++] = BTN2;
        printk("BTN2 pressed, stored at the index %d\n", input_index - 1);
      }
      //password submit when BTN3 is pressed
      if (BTN_check_clear_pressed(BTN3)) {
        bool correct = true;
        if (input_index != PASSWORD_LENGTH) correct = false;
        for (int i = 0; i < PASSWORD_LENGTH; i++) {
          if (input[i] != password[i]) {
            correct = false;
            break;
          }
        }

        if (correct) {
          printk("correct\n");
        } else {
          printk("Incorrect\n");
        }
        //enter waiting mode
        locked = false;
        LED_set(LED0, LED_OFF);
        input_index = 0;
      }
    }

    else {
      if (BTN_check_clear_pressed(BTN0) ||
        BTN_check_clear_pressed(BTN1) ||
        BTN_check_clear_pressed(BTN2) ||
        BTN_check_clear_pressed(BTN3)) {

          locked = true;
          LED_set(LED0, LED_ON);
          printk("waiting state\n");
        }
    }

    k_msleep(50);


    // if (BTN_check_clear_pressed(BTN0)) {
    //   counter++;

    //   if (counter >= 17) {
    //     counter = 0;
    //   }
    //   LED_set(LED0, (counter & 0b0010) ? LED_ON : LED_OFF);
    //   LED_set(LED1, (counter & 0b0001) ? LED_ON : LED_OFF);
    //   LED_set(LED2, (counter & 0b0100) ? LED_ON : LED_OFF);
    //   LED_set(LED3, (counter & 0b1000) ? LED_ON : LED_OFF);

    //   printk("Counter: %d\n", counter);
    }
    // if (BTN_check_clear_pressed(BTN0)) {
    //   LED_toggle(LED0);
    //   printk("Button 0 pressed!\n");
    // }
    // ret = gpio_pin_get_dt(&button);
    // if (0 < ret) {
    //   printk("Pressed!\n");
    // }
    // k_msleep(SLEEP_MS);
    // k_msleep(SLEEP_TIME_MS);
    return 0;
  }


