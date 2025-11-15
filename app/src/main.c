#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include "LED.h"
#include "BTN.h"

#define SLEEP_MS 50     
#define HOLD_TIME_MS 3000 

// Button setup
#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)

static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(SW3_NODE, gpios);

// LED setup
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

// FSM states
typedef enum {
    CHAR_ENTRY,
    STR_ENTRY,
    STR_SAVED,
    TRANSIMIT,
    STANDBY
} state;

state saved_state_before_standby = CHAR_ENTRY;
bool standby_printed = false;


int main(void) {
    state current_state = CHAR_ENTRY;
    uint64_t last_blink = k_uptime_get();
    bool led3_state = false;

    uint8_t char_buffer = 0;
    uint8_t bit_count = 0;
    uint64_t hold_start = 0;

    // GPIO ready checks
    if (!gpio_is_ready_dt(&button0) || !gpio_is_ready_dt(&button1) ||
        !gpio_is_ready_dt(&button2) || !gpio_is_ready_dt(&button3) ||
        !gpio_is_ready_dt(&led0) || !gpio_is_ready_dt(&led1) ||
        !gpio_is_ready_dt(&led2) || !gpio_is_ready_dt(&led3)) {
        printk("Error: GPIO not ready\n");
        return 0;
    }

    // Configure pins
    gpio_pin_configure_dt(&button0, GPIO_INPUT);
    gpio_pin_configure_dt(&button1, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);
    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

    printk("Program started\n");

    while (1) {
        // Read buttons
        bool btn0 = gpio_pin_get_dt(&button0);
        bool btn1 = gpio_pin_get_dt(&button1);
        bool btn2 = gpio_pin_get_dt(&button2);
        bool btn3 = gpio_pin_get_dt(&button3);

        // Blink LED3 at 1 Hz in CHAR_ENTRY
        if (current_state == CHAR_ENTRY && k_uptime_get() - last_blink >= 500) {
            led3_state = !led3_state;
            gpio_pin_set_dt(&led3, led3_state);
            last_blink = k_uptime_get();
        }

        // CHAR_ENTRY actions
        if (current_state == CHAR_ENTRY) {

            //when btn0 is pressed blink led0
            if (btn0 && !btn1 && !btn2 && !btn3) {
                if (bit_count < 8) {
                    char_buffer <<= 1;
                    char_buffer |= 0;
                    bit_count++;
                    gpio_pin_set_dt(&led0, 1);
                    k_msleep(100);
                    gpio_pin_set_dt(&led0, 0);
                    printk("BTN0 pressed\n");
                }
                k_msleep(200);
            }

            // btn1 pressed led1 blinks
            if (btn1 && !btn0 && !btn2 && !btn3) {
                if (bit_count < 8) {
                    char_buffer <<= 1;
                    char_buffer |= 1;
                    bit_count++;
                    gpio_pin_set_dt(&led1, 1);
                    k_msleep(100);
                    gpio_pin_set_dt(&led1, 0);
                    printk("BTN1 pressed\n");
                }
                k_msleep(200);
            }

            // BTN2 pressed resets
            if (btn2) {
                char_buffer = 0;
                bit_count = 0;
                printk("BTN2 pressed, reset entry\n");
                k_msleep(200);
            }

            // btn3 saves chr then goes to next state (STR_ENTRY)
            if (btn3) {
                printk("BTN3 pressed, saving chr: 0x%02X\n", char_buffer);
                current_state = STR_ENTRY;
                char_buffer = 0;
                bit_count = 0;
                k_msleep(200);
            }

            // Hold BTN0+BTN1 for standby
            if (btn0 && btn1) {
                if (hold_start == 0) hold_start = k_uptime_get();
                if (k_uptime_get() - hold_start >= HOLD_TIME_MS) {
                    printk("Entering Standby Mode\n");
                    saved_state_before_standby = current_state;
                    current_state = STANDBY;
                    standby_printed = false;
                    hold_start = 0;
                }
            } else {
                hold_start = 0;
            }
        }

        // STANDBY state
        if (current_state == STANDBY) {
            if (!standby_printed) {
                printk("In Standby mode\n");
                standby_printed = true;
            }

                k_msleep(200);
            }
        }

        k_msleep(SLEEP_MS);
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

