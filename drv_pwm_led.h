/* *****************************************************************************
 * File:   drv_pwm_led.h
 * Author: XX
 *
 * Created on YYYY MM DD
 * 
 * Description: ...
 * 
 **************************************************************************** */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

#include "sdkconfig.h"

#include <stdint.h>
//#include <stddef.h>
//#include <stdlib.h>
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    DRV_PWM_LED_CHANNEL_0,
    DRV_PWM_LED_CHANNEL_1,
    DRV_PWM_LED_CHANNEL_2,
    DRV_PWM_LED_CHANNEL_3,
    DRV_PWM_LED_CHANNEL_4,
    DRV_PWM_LED_CHANNEL_5,
    DRV_PWM_LED_CHANNEL_6,
    DRV_PWM_LED_CHANNEL_7,
    DRV_PWM_LED_CHANNEL_8,
}drv_pwm_led_e_channel_t;

typedef enum
{
    DRV_PWM_LED_0 = CONFIG_DRV_PWM_LED_PIN_0,
    DRV_PWM_LED_1 = CONFIG_DRV_PWM_LED_PIN_1,
    DRV_PWM_LED_2 = CONFIG_DRV_PWM_LED_PIN_2,
    DRV_PWM_LED_3 = CONFIG_DRV_PWM_LED_PIN_3,
    DRV_PWM_LED_4 = CONFIG_DRV_PWM_LED_PIN_4,
    DRV_PWM_LED_5 = CONFIG_DRV_PWM_LED_PIN_5,
    DRV_PWM_LED_6 = CONFIG_DRV_PWM_LED_PIN_6,
    DRV_PWM_LED_7 = CONFIG_DRV_PWM_LED_PIN_7,
    DRV_PWM_LED_8 = CONFIG_DRV_PWM_LED_PIN_8,
}drv_pwm_led_e_pin_t;

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void drv_pwm_led_init_timer(uint32_t frequency_hz);
void drv_pwm_led_init(drv_pwm_led_e_channel_t e_channel, drv_pwm_led_e_pin_t e_pin, float duty_percent, float set_high_point_percent);



#ifdef __cplusplus
}
#endif /* __cplusplus */


