#include <application.h>

#define RADIO_DELAY 3000

#define BATTERY_UPDATE_INTERVAL (60 * 60 * 1000)


// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

// Accelerometer
bc_lis2dh12_t acc;
bc_lis2dh12_result_g_t a_result;

bc_lis2dh12_alarm_t alarm;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        static uint16_t button_press_count = 0;

        bc_led_pulse(&led, 100);
        button_press_count++;

        bc_radio_pub_push_button(&button_press_count);
    }
}

void lis2_event_handler(bc_lis2dh12_t *self, bc_lis2dh12_event_t event, void *event_param)
{
    (void) self;
    (void) event_param;

    if (event == BC_LIS2DH12_EVENT_UPDATE)
    {
        bc_lis2dh12_get_result_g(&acc, &a_result);
        bc_log_debug("X: %f, Y: %f, Z: %f", a_result.x_axis, a_result.y_axis, a_result.z_axis);
    }
    else if (event == BC_LIS2DH12_EVENT_ALARM)
    {
        static uint16_t accelerometer_count = 0;
        static bc_tick_t radio_delay = 0;

        bc_log_debug("Alarm");
        bc_led_pulse(&led, 20);
        accelerometer_count++;

        // Limit the number of transmitted messages
        if (bc_tick_get() >= radio_delay)
        {
            // Make longer pulse when transmitting
            bc_led_pulse(&led, 100);

            bc_radio_pub_event_count(BC_RADIO_PUB_EVENT_ACCELEROMETER_ALERT, &accelerometer_count);
            radio_delay = bc_tick_get() + RADIO_DELAY;
        }
    }
    else
    {
        bc_log_debug("Error");
    }
}

// This function dispatches battery events
void battery_event_handler(bc_module_battery_event_t event, void *event_param)
{
    // Update event?
    if (event == BC_MODULE_BATTERY_EVENT_UPDATE)
    {
        float voltage;

        // Read battery voltage
        if (bc_module_battery_get_voltage(&voltage))
        {
            bc_log_info("APP: Battery voltage = %.2f", voltage);

            // Publish battery voltage
            bc_radio_pub_battery(&voltage);
        }
    }
}

void application_init(void)
{
    // Initialize logging
    bc_log_init(BC_LOG_LEVEL_DUMP, BC_LOG_TIMESTAMP_ABS);

    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_pulse(&led, 2000);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize battery
    bc_module_battery_init();
    bc_module_battery_set_event_handler(battery_event_handler, NULL);
    bc_module_battery_set_update_interval(BATTERY_UPDATE_INTERVAL);

    // Initialize radio
    bc_radio_init(BC_RADIO_MODE_NODE_SLEEPING);

    // Send radio pairing request
    bc_radio_pairing_request("shock-sensor", VERSION);

    bc_lis2dh12_init(&acc, BC_I2C_I2C0, 0x19);
    bc_lis2dh12_set_event_handler(&acc, lis2_event_handler, NULL);

    memset(&alarm, 0, sizeof(alarm));

    // Activate alarm when axis Z is above 0.25 or below -0.25 g
    alarm.z_high = true;
    alarm.threshold = 0.25;
    alarm.duration = 0;

    bc_lis2dh12_set_alarm(&acc, &alarm);
}

