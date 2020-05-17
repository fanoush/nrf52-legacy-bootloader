
#ifdef BSP_LED_0

#define ADVERTISING_LED_PIN_NO               BSP_LED_0                                               /**< Is on when device is advertising. */
#define CONNECTED_LED_PIN_NO                 BSP_LED_1                                               /**< Is on when device has connected. */
#define LED_OFF(x) nrf_gpio_pin_set(x)
#define LED_ON(x) nrf_gpio_pin_clear(x)
#define LED_OUTPUT(x) nrf_gpio_cfg_output(x)

#else

#define LED_OFF(x)
#define LED_ON(x)
#define LED_OUTPUT(x)

#endif
