#include <stdio.h>
#include <STC15Fxx.h>
#include <delay.h>

#define LED_OFF     P30     /* Power off led (red) */
#define LED_ON      P31     /* Power on led (green) */
#define PS_DLY      P32     /* Delayed power off */
#define SCP         (!P33)  /* Immediate power off (short protection) */
#define RELAY       P55     /* Relay control (power to printer on/off switch) */

#define RESPONSE_DELAY  100 /* how fast the sofware responds to input changes */
#define START_DELAY_S   60  /* time at startup to ignore PS_DLY state */
#define STOP_DELAY_S    20  /* time to wait before power off after trigger by PS_DLY */
#define RECOVER_DELAY_S 5   /* time to wait before power recovery after PS_DLY gets restored */
#define BLINKER_MUL     3   /* used for blinking LEDs, multiple of RESPONSE_DELAY */      

#if (RECOVER_DELAY_S > (STOP_DELAY_S-1))
#error "RECOVER_DELAY_S must be at least 1 second smaller than STOP_DELAY_S"
#endif

#if (RESPONSE_DELAY > 500)
#error "RESPONSE_DELAY must be lower than 500ms"
#endif

#if (BLINKER_MUL > 250)
#error "BLINKER_MUL must be lower than 250"
#endif

#define PWUP_DLY_TICKS  (START_DELAY_S*1000UL/RESPONSE_DELAY*1UL)
#define PWDN_DLY_TICKS  (STOP_DELAY_S*1000UL/RESPONSE_DELAY*1UL)
#define PWRE_DLY_TICKS  (STOP_DELAY_S*1000UL/RESPONSE_DELAY*1UL)

/* stated of the device */
typedef enum
{
    powerup,    /* first startup/reset will ignore PS_DLY state */
    running,    /* normal running state */
    psdelay,    /* PS_DLY was triggered, but we wait for power off delay */
    tripped,    /* PS_DLY trigger occurred */
    tripped_sc, /* SCP trigger occurred */
}states_t;

void main(void)
{
    /* setup i/o */
    P3M1 = 0xB4;
    P3M0 = 0xB4;
    P5M0 = 0xEA;
    /* initial state of outputs */
    RELAY = 0;
    LED_OFF = 0;
    LED_ON = 0;
    /* vars */
    states_t state = powerup;
    uint32_t updelay = 0;
    uint8_t blink = 0;
    /* program loop */
    while(1) {
        /* wait response time */
        delay_ms(RESPONSE_DELAY);
        /* check SCP, state independent */
        if (SCP)
            state = tripped_sc;
        /* check states */
        switch (state)
        {
        case powerup:
            /* powering state */
            RELAY = 0;
            /* increase delay timers */
            updelay += 1;
            blink += 1;
            if (blink > BLINKER_MUL) {
                /* leds */
                LED_OFF = (!LED_OFF);
                LED_ON = 1;
                blink = 0;
            }
            /* check power up delay */
            if (updelay > PWUP_DLY_TICKS) {
                updelay = 0;
                /* check PS_DLY */
                if (PS_DLY)
                    state = running;
                else
                    state = tripped;
            }
            break;
        case running:
            /* powering state */
            RELAY = 0;
            /* leds */
            LED_OFF = 0;
            LED_ON = 1;
            /* reset timers */
            updelay = 0;
            blink = 0;
            /* check PS_DLY */
            if (PS_DLY)
                state = psdelay;
            break;        
        case psdelay:
            /* powering state */
            RELAY = 0;
            /* increase delay timers */
            updelay += 1;
            blink += 1;
            if (blink > BLINKER_MUL) {
                /* leds */
                LED_OFF = 0;
                LED_ON = (!LED_ON);
                blink = 0;
            }
            /* check power down delay */
            if (updelay > PWDN_DLY_TICKS) {
                /* reset timers */
                blink = 0;
                updelay = 0;
                state = tripped;
            }
            break;
        case tripped:
            /* tripped state */
            RELAY = 1;
            /* leds */
            LED_OFF = 1;
            LED_ON = 0;
            /* increase delay timers */
            updelay += 1;
            /* power recovery */
            if (updelay > PWRE_DLY_TICKS) {
                /* check PS_DLY pin */
                if (!PS_DLY) {
                    /* reset timers */
                    blink = 0;
                    updelay = 0;
                    state = running;
                }
                updelay = PWRE_DLY_TICKS;
            }
            break;   
        default:
            /* tripped state */
            RELAY = 1;
            /* increase delay timers */
            blink += 1;
            /* blinking leds alternatively, one it BLINKER_MUL on, then the other is BLINKER_MUL on */
            if (blink > (2*BLINKER_MUL))
                blink=0;
            if (blink > BLINKER_MUL) {
                /* leds */
                LED_OFF = 1;
                LED_ON = 0;
            } else {
                /* leds */
                LED_OFF = 0;
                LED_ON = 1;
            }
            /* reset timers */
            updelay = 0;
            break;
        }       
    }
}