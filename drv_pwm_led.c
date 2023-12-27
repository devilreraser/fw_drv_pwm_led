/* *****************************************************************************
 * File:   drv_pwm_led.c
 * Author: XX
 *
 * Created on YYYY MM DD
 * 
 * Description: ...
 * 
 **************************************************************************** */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "drv_pwm_led.h"

#include <math.h>


#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define TAG "drv_pwm_led"

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
//#define LEDC_OUTPUT_IO          (33) // Define the output GPIO - setup with interface
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Set duty resolution in bits
//#define LEDC_DUTY               (4096) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
//#define LEDC_FREQUENCY          (1000) // Frequency in Hertz. Set frequency at 1 kHz - setup with interface

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
int duty_timer_value;
int set_high_point_timer_value;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */



void ledc_init(drv_pwm_led_e_pin_t e_pin, drv_pwm_led_e_channel_t e_channel, float set_high_point_percent)
{
    set_high_point_timer_value = set_high_point_percent * pow(2, LEDC_DUTY_RES) / 100;

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = e_channel,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = e_pin,
        .duty           = 0, // Set duty to 0%
        .hpoint         = set_high_point_timer_value
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void drv_pwm_led_init_timer(uint32_t frequency_hz)
{

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = frequency_hz,  // Set output frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));


}

void drv_pwm_led_init(drv_pwm_led_e_channel_t e_channel, drv_pwm_led_e_pin_t e_pin, float duty_percent, float set_high_point_percent)
{
    duty_timer_value = duty_percent * pow(2, LEDC_DUTY_RES) / 100;

    // Set the LEDC peripheral configuration
    ledc_init(e_pin, e_channel, set_high_point_percent);
    // Set duty to 50%
    //ESP_ERROR_CHECK(ledc_set_duty_with_hpoint(LEDC_MODE, e_channel, LEDC_DUTY, (int)(set_high_point_percent * pow(2, LEDC_DUTY_RES) / 100)));
    //ESP_LOGW(TAG, "PWM_LED_%d Set High point : %d", e_pin, (int)(set_high_point_percent * pow(2, LEDC_DUTY_RES) / 100));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, e_channel, duty_timer_value));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, e_channel));
}
