#include <application.h>

#define RADIO_DELAY 10000

#define BATTERY_UPDATE_INTERVAL (60 * 60 * 1000)


// LED instance
twr_led_t led;

// Button instance
twr_button_t button;

// Accelerometer
twr_lis2dh12_t acc;
twr_lis2dh12_result_g_t a_result;

twr_lis2dh12_alarm_t alarm;

void button_event_handler(twr_button_t *self, twr_button_event_t event, void *event_param)
{
    if (event == TWR_BUTTON_EVENT_PRESS)
    {
        static uint16_t button_press_count = 0;

        twr_led_pulse(&led, 100);
        button_press_count++;

        twr_radio_pub_push_button(&button_press_count);
    }
}

void lis2_event_handler(twr_lis2dh12_t *self, twr_lis2dh12_event_t event, void *event_param)
{
    (void) self;
    (void) event_param;

    if (event == TWR_LIS2DH12_EVENT_UPDATE)
    {
        twr_lis2dh12_get_result_g(&acc, &a_result);
        twr_log_debug("X: %f, Y: %f, Z: %f", a_result.x_axis, a_result.y_axis, a_result.z_axis);
    }
    else if (event == TWR_LIS2DH12_EVENT_ALARM)
    {
        static uint16_t accelerometer_count = 0;
        static twr_tick_t radio_delay = 0;

        twr_log_debug("Alarm");
        twr_led_pulse(&led, 20);
        accelerometer_count++;

        // Limit the number of transmitted messages
        if (twr_tick_get() >= radio_delay)
        {
            // Make longer pulse when transmitting
            twr_led_pulse(&led, 100);

            twr_radio_pub_event_count(TWR_RADIO_PUB_EVENT_ACCELEROMETER_ALERT, &accelerometer_count);
            radio_delay = twr_tick_get() + RADIO_DELAY;
        }
    }
    else
    {
        twr_log_debug("Error");
    }
}

// This function dispatches battery events
void battery_event_handler(twr_module_battery_event_t event, void *event_param)
{
    // Update event?
    if (event == TWR_MODULE_BATTERY_EVENT_UPDATE)
    {
        float voltage;

        // Read battery voltage
        if (twr_module_battery_get_voltage(&voltage))
        {
            twr_log_info("APP: Battery voltage = %.2f", voltage);

            // Publish battery voltage
            twr_radio_pub_battery(&voltage);
        }
    }
}

void application_init(void)
{
    // Initialize logging
    twr_log_init(TWR_LOG_LEVEL_DUMP, TWR_LOG_TIMESTAMP_ABS);

    // Initialize LED
    twr_led_init(&led, TWR_GPIO_LED, false, false);
    twr_led_pulse(&led, 2000);

    // Initialize button
    twr_button_init(&button, TWR_GPIO_BUTTON, TWR_GPIO_PULL_DOWN, false);
    twr_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize battery
    twr_module_battery_init();
    twr_module_battery_set_event_handler(battery_event_handler, NULL);
    twr_module_battery_set_update_interval(BATTERY_UPDATE_INTERVAL);

    // Initialize radio
    twr_radio_init(TWR_RADIO_MODE_NODE_SLEEPING);

    // Send radio pairing request
    twr_radio_pairing_request("shock-sensor", VERSION);

    twr_lis2dh12_init(&acc, TWR_I2C_I2C0, 0x19);
    twr_lis2dh12_set_event_handler(&acc, lis2_event_handler, NULL);

    memset(&alarm, 0, sizeof(alarm));

    // Activate alarm when axis Z is above 0.25 or below -0.25 g
    alarm.z_high = true;
    alarm.threshold = 0.25;
    alarm.duration = 0;

    twr_lis2dh12_set_alarm(&acc, &alarm);
}

