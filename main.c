
#include "dfu_transport.h"
#include "bootloader.h"
#include "bootloader_util.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "ble.h"
#include "nrf.h"
#include "ble_hci.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_error.h"
#include "boards.h"
#include "softdevice_handler_appsh.h"
#include "pstorage_platform.h"
#include "nrf_mbr.h"
#include "nrf_log.h"

#ifdef HAS_LF_XTAL
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
#else
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                             .rc_ctiv       = 16,                                \
                             .rc_temp_ctiv  = 2,                                \
                             .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
#endif


static void leds_init(void)
{
#if LEDS_NUMBER > 0
// these come from board definition included from boards.h
//    nrf_gpio_range_cfg_output(LED_START, LED_STOP); // set range of pins as output
//    nrf_gpio_pins_set(LEDS_MASK); // set all pins to 1 (= turn LED off)
// moved here from dfu_transport_ble - leds_init
    LED_OUTPUT(ADVERTISING_LED_PIN_NO);
    LED_OUTPUT(CONNECTED_LED_PIN_NO);
    LED_OFF(ADVERTISING_LED_PIN_NO);
    LED_OFF(CONNECTED_LED_PIN_NO);
}
#endif
}

/**@brief Function for initializing the button module.
 */
static void buttons_init(void)
{

#if BUTTONS_NUMBER > 0
//    nrf_gpio_cfg_sense_input(BOOTLOADER_BUTTON, BUTTON_PULL, NRF_GPIO_PIN_SENSE_LOW);
  nrf_gpio_cfg_input(BUTTON_1,BUTTON_1_PULL);
#define button1_state() (nrf_gpio_pin_read(BUTTON_1)==BUTTON_1_VALUE)
#else
#define button1_state() (false)
#endif

#ifdef POWER_PIN
  nrf_gpio_cfg_input(POWER_PIN,POWER_PULL);
#define power_on() (nrf_gpio_pin_read(POWER_PIN)==POWER_ON)
#else
#define power_on() (true)
#endif

#ifdef MOTOR_PIN
#define motor_on() nrf_gpio_pin_write(MOTOR_PIN,MOTOR_ON)
#define motor_off() nrf_gpio_pin_write(MOTOR_PIN,1-MOTOR_ON)
  motor_off();
  nrf_gpio_cfg_output(MOTOR_PIN);
#else
#define motor_on()
#define motor_off()
#endif

}


#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                                       /**< Include the service_changed characteristic. For DFU this should normally be the case. */
#define APP_TIMER_PRESCALER             0                                                       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                                       /**< Size of timer operation queues. */
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, 0)                        /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE                20                                                      /**< Maximum number of events in the scheduler queue. */


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] file_name   File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}



/**@brief Function for initializing the timer handler module (app_timer).
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
}



/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void sys_evt_dispatch(uint32_t event)
{
    pstorage_sys_event_handler(event);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 *
 * @param[in] init_softdevice  true if SoftDevice should be initialized. The SoftDevice must only 
 *                             be initialized if a chip reset has occured. Soft reset from 
 *                             application must not reinitialize the SoftDevice.
 */
static void ble_stack_init(bool init_softdevice)
{
    uint32_t         err_code;
    sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    if (init_softdevice)
    {
        err_code = sd_mbr_command(&com);
        APP_ERROR_CHECK(err_code);
    }

    err_code = sd_softdevice_vector_table_base_set(BOOTLOADER_REGION_START);
    APP_ERROR_CHECK(err_code);

    SOFTDEVICE_HANDLER_APPSH_INIT(&clock_lf_cfg, true);

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    // Only one connection as a central is used when performing dfu.
    err_code = softdevice_enable_get_default_config(1, 1, &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for event scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Function for bootloader main entry.
 */
int main(void)
{
    uint32_t err_code;
#ifdef DISABLE_BUTTONLESS_DFU
    bool     app_reset = false;
#else
    bool     app_reset = (NRF_POWER->GPREGRET == BOOTLOADER_DFU_START);
#endif
    bool     dfu_start = app_reset || (NRF_POWER->GPREGRET == 1);
    bool     app_valid;

    if (dfu_start)
    {
        NRF_POWER->GPREGRET = 0;
    }

    leds_init();

    // This check ensures that the defined fields in the bootloader corresponds with actual
    // setting in the chip.
    APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);
    APP_ERROR_CHECK_BOOL(NRF_FICR->CODEPAGESIZE == CODE_PAGE_SIZE);

    // Initialize.
    timers_init();
    buttons_init();

    (void)bootloader_init();

    if (bootloader_dfu_sd_in_progress())
    {
        LED_ON(UPDATE_IN_PROGRESS_LED);

        err_code = bootloader_dfu_sd_update_continue();
        APP_ERROR_CHECK(err_code);

        ble_stack_init(!app_reset);
        scheduler_init();

        err_code = bootloader_dfu_sd_update_finalize();
        APP_ERROR_CHECK(err_code);

        LED_OFF(UPDATE_IN_PROGRESS_LED);
    }
    else
    {
        // If stack is present then continue initialization of bootloader.
        ble_stack_init(!app_reset);
        scheduler_init();
    }

    app_valid = bootloader_app_is_valid(DFU_BANK_0_REGION_START);
#if BUTTONS_NUMBER > 0
#pragma message("Buttons enabled")
    if (NRF_POWER->RESETREAS==0 && app_valid){
	// cold boot with valid app - allow enter dfu when on charger and holding button
#if defined(BUTTON_1_ISTOUCH) && (BUTTON_1_ISTOUCH == 1)
#pragma message("Touch button enabled")
	// touch button doesn't work when it is held at cold boot, try to vibrate and wait for touch 3 seconds
	motor_on();
	nrf_delay_ms(300);
	motor_off();
	if (power_on()) {
	    int count=3000;//3 seconds
	    while (--count>0){
	        nrf_delay_ms(1);//1 ms
	        if(!button1_state()) continue;
	        dfu_start=true; // go to bootloader when button held on powerup
	        break;
	    }
	    motor_on();
	    nrf_delay_ms(300);
	    motor_off();
	}
#else
	if(button1_state()) dfu_start=true;
#endif
    }
    if (app_reset && !(button1_state()||power_on())) dfu_start=false; // if buttonless DFU request, require also holding button to enter DFU
#endif

    if (dfu_start || (!app_valid))
    {
        LED_ON(UPDATE_IN_PROGRESS_LED);

        // Initiate an update of the firmware.
        err_code = bootloader_dfu_start();
        APP_ERROR_CHECK(err_code);

        LED_OFF(UPDATE_IN_PROGRESS_LED);
    }

    if (bootloader_app_is_valid(DFU_BANK_0_REGION_START) && !bootloader_dfu_sd_in_progress())
    {
        // Select a bank region to use as application region.
        // @note: Only applications running from DFU_BANK_0_REGION_START is supported.
        bootloader_app_start(DFU_BANK_0_REGION_START);
    }

    NVIC_SystemReset();
}
