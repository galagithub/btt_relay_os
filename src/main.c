#include <stdio.h>
#include <STC15Fxx.h>
#include <delay.h>
#include <reset.h>

/* define this to check if PS_ON has signal before going to blanking state */
#define IGNORE_FALSE_TRIPPING

#define LED_OFF     P30     /* Power off led (red) */
#define LED_ON      P31     /* Power on led (green) */
#define PS_DLY      P32     /* Delayed power off */
#define SCP         (!P33)  /* Immediate power off (short protection) */
#define RELAY       P55     /* Relay control (power to printer on/off switch) */

#define RESPONSE_DELAY  100 /* how fast the sofware responds to input changes */
#define START_DELAY_S   60  /* time at startup to ignore PS_DLY state */
#define STOP_DELAY_S    20  /* time to wait before power off after trigger by PS_DLY */
#define RECOVER_DELAY_S 30  /* time to wait before power recovery after PS_DLY gets restored */
#define BLINKER_MUL     3   /* used for blinking LEDs, multiple of RESPONSE_DELAY */      

#if (RESPONSE_DELAY > 500)
#error "RESPONSE_DELAY must be lower than 500ms"
#endif

#if (BLINKER_MUL > 250)
#error "BLINKER_MUL must be lower than 250"
#endif

#define PWUP_DLY_TICKS  (START_DELAY_S*1000UL/RESPONSE_DELAY*1UL)
#define PWDN_DLY_TICKS  (STOP_DELAY_S*1000UL/RESPONSE_DELAY*1UL)
#define PWRE_DLY_TICKS  (STOP_DELAY_S*1000UL/RESPONSE_DELAY*1UL)

/* states of the device */
typedef enum {
    powerup,    /* first startup/reset will ignore PS_DLY state */
    running,    /* normal running state */
    psdelay,    /* PS_DLY was triggered, but we wait for power off delay */
    blanking,   /* PS_DLY was triggered, power was cut, we wait recovery delay*/
    tripped,    /* PS_DLY trigger occurred */
    tripped_sc, /* SCP trigger occurred */
}states_t;

/* vars */
states_t state = powerup;
uint32_t updelay = 0;
uint8_t blink = 0;

/* things to always do when switching to new state */
void goto_state(states_t new) {
    /* reset leds */
    LED_ON = 0;
    LED_OFF = 0;
    /* if short, start with red led on */
    if (new == tripped_sc)
        LED_OFF = 1;
    /* reset timers */
    updelay = 0;
    blink = 0;
    /* set new state */
    state = new;
}

/* setup and init i/o */
void init_io(void) {
    /* setup i/o */
    P3M1 = 0xB4;
    P3M0 = 0xB4;
    P5M0 = 0xEA;
    /* initial state of outputs */
    RELAY = 0;
    LED_OFF = 0;
    LED_ON = 0;
}

void main(void) {
    /* setup i/o */
    init_io();
    /* program loop */
    while(1) {
        /* wait response time */
        delay_ms(RESPONSE_DELAY);
        /* check SCP, state independent */
        if (SCP)
            state = tripped_sc;
        /* check states */
        switch (state) {
        case powerup:
            /* powering state */
            RELAY = 0;
            /* led state */
            LED_ON = 1;
            /* increase delay timers */
            updelay += 1;
            blink += 1;
            /* blink led */
            if (blink > BLINKER_MUL) {
                /* leds */
                LED_OFF = (!LED_OFF);
                blink = 0;
            }
            /* check power up delay */
            if (updelay > PWUP_DLY_TICKS) {
                /* check PS_DLY */
                if (PS_DLY)
                    goto_state(blanking);
                /* PS_ON must have signal to continue ! */
                else
                    goto_state(running);
            }
            break;
        case running:
            /* powering state */
            RELAY = 0;
            /* led state */
            LED_OFF = 0;
            LED_ON = 1;
            /* reset timers */
            updelay = 0;
            blink = 0;
            /* check PS_DLY */
            if (PS_DLY)
                goto_state(psdelay);
            break;        
        case psdelay:
            /* powering state */
            RELAY = 0;
            /* led state */
            LED_OFF = 0;
            /* increase delay timers */
            updelay += 1;
            blink += 1;
            /* blink led */
            if (blink > BLINKER_MUL) {
                /* leds */
                LED_ON = (!LED_ON);
                blink = 0;
            }
            /* check power down delay */
            if (updelay > PWDN_DLY_TICKS)
#ifdef IGNORE_FALSE_TRIPPING
                if (!PS_DLY)
                    goto_state(running);
                else
#endif
                goto_state(blanking);
            break;        
        case blanking:
            /* tripped state */
            RELAY = 1;
            /* increase delay timers */
            updelay += 1;
            blink += 1;
            /* blink both leds */
            if (blink > BLINKER_MUL) {
                /* leds */
                LED_OFF = (!LED_OFF);
                LED_ON = (!LED_ON);
                blink = 0;
            }
            /* check power down delay */
            if (updelay > PWRE_DLY_TICKS) {
                /* check PS_DLY, if 1 then go to tripped */
                if (PS_DLY)
                    goto_state(tripped);
                else
                    reset();
            }
            break;
        case tripped:
            /* tripped state */
            RELAY = 1;
            /* leds */
            LED_OFF = 1;
            LED_ON = 0;
            /* power recovery (self-reset) */
            if (!PS_DLY)
                reset();
            break;   
        default:
            /* tripped state */
            RELAY = 1;
            /* increase delay timers */
            blink += 1;
            /* blinking leds alternating them */
            if (blink > BLINKER_MUL) {
                /* leds */
                if(LED_ON) {
                    LED_OFF = 1;
                    LED_ON = 0;
                } else {
                    LED_OFF = 0;
                    LED_ON = 1;
                }
                /* reset blink timer */
                blink = 0;
            }
            /* reset other timers */
            updelay = 0;
            break;
        }
    }
}