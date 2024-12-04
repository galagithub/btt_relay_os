#include <stdio.h>
#include <STC15Fxx.h>
#include <delay.h>

#define LED_OFF     P30     /* Power off led (red) */
#define LED_ON      P31     /* Power on led (green) */
#define PS_DLY      P32     /* Delayed power off */
#define SCP         (!P33)     /* Immediate power off (short protection) */
#define RELAY       P55     /* Relay control (power to printer on/off switch) */

#define RESPONSE_DELAY  100 /* how fast the sofware responds to input changes */
#define START_DELAY_S   45  /* time at startup to ignore PS_DLY state */
#define STOP_DELAY_S    15  /* time at startup to ignore PS_DLY state */

#define PWUP_DLY_TICKS  (START_DELAY_S*1000UL/RESPONSE_DELAY*1UL)
#define PWDN_DLY_TICKS  (STOP_DELAY_S*1000UL/RESPONSE_DELAY*1UL)

/* stated of the device */
typedef enum
{
    powerup,    /* first startup/reset will ignore PS_DLY state */
    running,    /* normal running state */
    psdelay,    /* PS_DLY was triggered, but we wait for power off delay */
    tripped,    /* PS_DLY or SCP trigger occurred */

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
	LED_ON = 0xff;
    /* vars */
    states_t state = powerup;
    uint32_t updelay = 0;
    /* program loop */
	while(1) {
        /* wait response time */
        delay_ms(RESPONSE_DELAY);
        /* check SCP, state independent */
        if (SCP)
            state = tripped;
        /* check states */
        switch (state)
        {
        case powerup:
            /* check power up delay */
            updelay += 1;
            if (updelay > PWUP_DLY_TICKS) {
                updelay = 0;
                state = running;
            }
            break;
        case running:
            updelay = 0;
            /* check PS_DLY */
            if (PS_DLY)
                state = psdelay;
            break;        
        case psdelay:
            /* check power up delay */
            updelay += 1;
            if (updelay > PWDN_DLY_TICKS) {
                state = tripped;
                LED_OFF = 0xff;
                LED_ON = 0;
            }
#if 0
            /* check PS_DLY */
            if (!PS_DLY) {
                updelay = 0;
                state = running;
            }
#endif
            break;   
     
        default:
            /* tripped state */
            RELAY = 0xff;
            break;
        }       
    }
}