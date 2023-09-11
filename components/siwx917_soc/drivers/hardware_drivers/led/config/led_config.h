/***************************************************************************/ /**
 * @file led_config.h
 * @brief Simple Led-Button Driver Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************/

#ifndef LED_CONFIG_H
#define LED_CONFIG_H

typedef enum {
  led_gpioPortA = 0,
  led_gpioPortB = 1,
  led_gpioPortC = 2,
  led_gpioPortD = 3,
  led_gpioPortE = 4,
  led_gpioPortF = 5,
  led_gpioPortG = 6,
  led_gpioPortH = 7,
  led_gpioPortI = 8,
  led_gpioPortJ = 9,
  led_gpioPortK = 10,
} led_GPIO_Port_TypeDef;

// <<< Use Configuration Wizard in Context Menu >>>
#define LED_0 0
#define LED_1 1

// <h> Simple LED configuration
// <o LED_SELECT>
// <LED_0=> LED0
// <LED_1=> LED1
// <i> Default: LED_0
#define LED_SELECT LED_0
// </h> end led configuration

// <<< end of configuration section >>>

// $[LED]
#define BSP_LED_PRESENT (2)

#define BSP_LED0_PIN  (2U) // LED0--->ULP GPIO 2--->F12
#define BSP_LED0_PORT (led_gpioPortA)

#define BSP_LED1_PIN  (11U) // LED1-->GPIO 10----->F13
#define BSP_LED1_PORT (led_gpioPortA)

#define BSP_LED_POLARITY (1)

#define HAL_SERIAL_APP_BAUD_RATE 115200
#define HAL_SERIAL_APP_RXSTART   16
#define HAL_SERIAL_APP_RXSTOP    16
#undef HAL_SERIAL_APP_RX_QUEUE_SIZE
#define HAL_SERIAL_APP_RX_QUEUE_SIZE 64
#define HAL_SERIAL_APP_TX_QUEUE_SIZE 128

#define HAL_LED_COUNT (2)

#define HAL_SERIAL_VUART_ENABLE  (1)
#define HAL_SERIAL_RXWAKE_ENABLE (1)

#define HAL_VCOM_ENABLE (1)
#define HAL_WDOG_ENABLE (1)

#define HAL_GPIO_DOUT_LOW  false
#define HAL_GPIO_DOUT_HIGH true

#endif // LED_USER_CONFIG_H
