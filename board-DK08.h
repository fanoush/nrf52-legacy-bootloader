// LEDs definitions 
#define LEDS_NUMBER    0
/*
#define LED_START      17
#define LED_1          17
#define LED_2          18
#define LED_3          19
#define LED_4          20
#define LED_STOP       20

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3
#define BSP_LED_3      LED_4

#define BSP_LED_0_MASK (1<<BSP_LED_0)
#define BSP_LED_1_MASK (1<<BSP_LED_1)
#define BSP_LED_2_MASK (1<<BSP_LED_2)
#define BSP_LED_3_MASK (1<<BSP_LED_3)

#define LEDS_MASK      (BSP_LED_0_MASK | BSP_LED_1_MASK | BSP_LED_2_MASK | BSP_LED_3_MASK)
/ * all LEDs are lit when GPIO is low * /
#define LEDS_INV_MASK  LEDS_MASK
*/
#define BUTTONS_NUMBER 1

#define BUTTON_START   8
#define BUTTON_1       8
#define BUTTON_STOP    8

#define BUTTON_1_PULL	NRF_GPIO_PIN_PULLDOWN
#define BUTTON_1_VALUE	1
#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1
/*
#define BSP_BUTTON_1   BUTTON_2
#define BSP_BUTTON_2   BUTTON_3
#define BSP_BUTTON_3   BUTTON_4
*/

#define BSP_BUTTON_0_MASK (1<<BSP_BUTTON_0)
/*
#define BSP_BUTTON_1_MASK (1<<BSP_BUTTON_1)
#define BSP_BUTTON_2_MASK (1<<BSP_BUTTON_2)
#define BSP_BUTTON_3_MASK (1<<BSP_BUTTON_3)
*/
#define POWER_PIN 24
#define POWER_VALUE 1
#define POWER_PULL NRF_GPIO_PIN_NOPULL

#define CHARGE_DET_PIN 23
#define CHARGE_DET_PULL NRF_GPIO_PIN_PULLUP

#define MOTOR_PIN 6
#define MOTOR_VALUE 1