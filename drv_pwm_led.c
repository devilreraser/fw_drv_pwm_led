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
#include "esp_intr_alloc.h"
#include "soc/ledc_periph.h"

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
bool timer_is_busy[LEDC_TIMER_MAX] = {0};
bool channel_is_busy[LEDC_CHANNEL_MAX] = {0};
intr_handle_t isr_handle = NULL;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */



void ledc_init(drv_pwm_led_e_pin_t e_pin, drv_pwm_led_e_channel_t e_channel, drv_pwm_led_e_timer_t e_timer, float set_high_point_percent)
{
    set_high_point_timer_value = set_high_point_percent * pow(2, LEDC_DUTY_RES) / 100;

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = e_channel,
        .timer_sel      = e_timer,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = e_pin,
        .duty           = 0, // Set duty to 0%
        .hpoint         = set_high_point_timer_value
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

drv_pwm_led_e_timer_t drv_pwm_led_free_timer_get(void)
{
    for (int index = 0; index < LEDC_TIMER_MAX; index++)
    {
        if (timer_is_busy[index] == false)
        {
            return (drv_pwm_led_e_timer_t)index;
        }
    }
    return (drv_pwm_led_e_timer_t)LEDC_TIMER_MAX;
}

drv_pwm_led_e_channel_t drv_pwm_led_free_channel_get(void)
{
    for (int index = 0; index < LEDC_CHANNEL_MAX; index++)
    {
        if (channel_is_busy[index] == false)
        {
            return (drv_pwm_led_e_channel_t)index;
        }
    }
    return (drv_pwm_led_e_channel_t)LEDC_CHANNEL_MAX;
}


void drv_pwm_led_init_timer(drv_pwm_led_e_timer_t e_timer, uint32_t frequency_hz)
{

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = e_timer,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = frequency_hz,  // Set output frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    timer_is_busy[e_timer] = true;
}

void drv_pwm_led_init(drv_pwm_led_e_channel_t e_channel, drv_pwm_led_e_pin_t e_pin, drv_pwm_led_e_timer_t e_timer, float duty_percent, float set_high_point_percent)
{
    duty_timer_value = duty_percent * pow(2, LEDC_DUTY_RES) / 100;

    // Set the LEDC peripheral configuration
    ledc_init(e_pin, e_channel, e_timer, set_high_point_percent);
    // Set duty to 50%
    //ESP_ERROR_CHECK(ledc_set_duty_with_hpoint(LEDC_MODE, e_channel, LEDC_DUTY, (int)(set_high_point_percent * pow(2, LEDC_DUTY_RES) / 100)));
    //ESP_LOGW(TAG, "PWM_LED_%d Set High point : %d", e_pin, (int)(set_high_point_percent * pow(2, LEDC_DUTY_RES) / 100));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, e_channel, duty_timer_value));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, e_channel));
    channel_is_busy[e_channel] = true;
}

void drv_pwm_led_set_duty(drv_pwm_led_e_channel_t e_channel, float duty_percent)
{
    duty_timer_value = duty_percent * pow(2, LEDC_DUTY_RES) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, e_channel, duty_timer_value));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, e_channel));
}



void drv_pwm_led_configure_ledc_isr(void (*isr_handler)(void *), void *arg) 
{
    esp_err_t err = esp_intr_alloc(ETS_LEDC_INTR_SOURCE, ESP_INTR_FLAG_IRAM, isr_handler, arg, &isr_handle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "LEDC ISR Register Failure: %s", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGD(TAG, "LEDC ISR Register Success: %s", esp_err_to_name(err));
        err = esp_intr_enable(isr_handle);
        if (err != ESP_OK) 
        {
            ESP_LOGE(TAG, "esp_intr_enable Failure: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGD(TAG, "esp_intr_enable Success: %s", esp_err_to_name(err));
        }
    }
}

