/**
 * @file my_state_machine.c
 */

#include <zephyr/smf.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "LED.h"
#include "BTN.h"
#include "my_state_machine.h"

 enum led_state_machine_states {
    S0_STATE,
    S1_STATE,
    S2_STATE,
    S3_STATE,
    S4_STATE
};


typedef struct {
    struct smf_ctx ctx;     // must be first
    bool entered;
    uint32_t timer;
} led_state_object_t;


static enum smf_state_result s0_run(void *o);
static void s0_exit(void *o);

static enum smf_state_result s1_run(void *o);
static void s1_exit(void *o);

static enum smf_state_result s2_run(void *o);
static void s2_exit(void *o);

static enum smf_state_result s3_run(void *o);
static void s3_exit(void *o);

static enum smf_state_result s4_run(void *o);
static void s4_exit(void *o);


static const struct smf_state states[] = {
    [S0_STATE] = SMF_CREATE_STATE(NULL, s0_run, s0_exit, NULL, NULL),
    [S1_STATE] = SMF_CREATE_STATE(NULL, s1_run, s1_exit, NULL, NULL),
    [S2_STATE] = SMF_CREATE_STATE(NULL, s2_run, s2_exit, NULL, NULL),
    [S3_STATE] = SMF_CREATE_STATE(NULL, s3_run, s3_exit, NULL, NULL),
    [S4_STATE] = SMF_CREATE_STATE(NULL, s4_run, s4_exit, NULL, NULL),
};

static led_state_object_t led_state_object;



void state_machine_init(void) {
    led_state_object.entered = false;
    led_state_object.timer = 0;
    printk("Starting state machine at S0\n");
    smf_set_initial(SMF_CTX(&led_state_object), &states[S0_STATE]);

}


int state_machine_run(void) {
    return smf_run_state(SMF_CTX(&led_state_object));
}


static enum smf_state_result s0_run(void *o) {
    if (!led_state_object.entered) {
        printk("Entering S0: All LEDs off\n");
        LED_set(LED1, LED_OFF);
        LED_set(LED2, LED_OFF);
        LED_set(LED3, LED_OFF);
        led_state_object.entered = true;
    }

    if (BTN_is_pressed(BTN1)) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S1_STATE]);
    } else if (BTN_is_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S4_STATE]);
    }

    return SMF_EVENT_HANDLED;
}

static void s0_exit(void *o) {
    printk("Exiting S0\n");
    led_state_object.entered = false;
}

static enum smf_state_result s1_run(void *o) {
    if (!led_state_object.entered) {
        printk("Entering S1: LED1 blinking at 4 Hz\n");
        LED_blink(LED1, 4); 
        led_state_object.entered = true;
    }

    if (BTN_is_pressed(BTN2)) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S2_STATE]);
    } else if (BTN_is_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S0_STATE]);
    }

    return SMF_EVENT_HANDLED;
}

static void s1_exit(void *o) {
    printk("Exiting S1\n");
    led_state_object.entered = false;
}

static enum smf_state_result s2_run(void *o) {
    if (!led_state_object.entered) {
        printk("Entering S2: LED1 & LED3 ON, LED2 & LED3 OFF\n");
        LED_set(LED1, LED_ON);
        LED_set(LED3, LED_ON);
        LED_set(LED2, LED_OFF);
        LED_set(LED3, LED_OFF);
        led_state_object.entered = true;
        led_state_object.timer = k_uptime_get();
    }


    

    if (k_uptime_get() - led_state_object.timer >= 1000) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S3_STATE]);
    } else if (BTN_is_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S0_STATE]);
    }

    return SMF_EVENT_HANDLED;
}

static void s2_exit(void *o) {
    printk("Exiting S2\n");
    led_state_object.entered = false;
}

static enum smf_state_result s3_run(void *o) {
    if (!led_state_object.entered) {
        printk("Entering S3: LED1 & LED3 OFF, LED2 & LED3 ON\n");
        LED_set(LED1, LED_OFF);
        LED_set(LED3, LED_OFF);
        LED_set(LED2, LED_ON);
        LED_set(LED3, LED_ON);
        led_state_object.entered = true;
        led_state_object.timer = k_uptime_get();
    }


    if (k_uptime_get() - led_state_object.timer >= 2000) {  
        smf_set_state(SMF_CTX(&led_state_object), &states[S2_STATE]);
    } else if (BTN_is_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S0_STATE]);
    }

    return SMF_EVENT_HANDLED;
}

static void s3_exit(void *o) {
    printk("Exiting S3\n");
    led_state_object.entered = false;
}

static enum smf_state_result s4_run(void *o) {
    if (!led_state_object.entered) {
        printk("Entering S4: All LEDs blinking at 16 Hz\n");
        LED_blink(LED1, 16);
        LED_blink(LED2, 16);
        LED_blink(LED3, 16);
        led_state_object.entered = true;
    }

    if (BTN_is_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &states[S0_STATE]);
    }

    return SMF_EVENT_HANDLED;
}

static void s4_exit(void *o) {
    printk("Exiting S4\n");
    led_state_object.entered = false;
}