/***************************************************************************/ /**
 * @file button_config.h
 * @brief Simple Led-Button Driver Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************/

#ifndef BUTTON_CONFIG_H
#define BUTTON_CONFIG_H

typedef enum {
  btn_gpioPortA = 0,
  btn_gpioPortB = 1,
  btn_gpioPortC = 2,
  btn_gpioPortD = 3,
  btn_gpioPortE = 4,
  btn_gpioPortF = 5,
  btn_gpioPortG = 6,
  btn_gpioPortH = 7,
  btn_gpioPortI = 8,
  btn_gpioPortJ = 9,
  btn_gpioPortK = 10,
} btn_GPIO_Port_TypeDef;

// <<< Use Configuration Wizard in Context Menu >>>
#define BTN_0 0
#define BTN_1 1

// <h> Simple BUTTON configuration
// <o BUTTON_SELECT>
// <BTN_0=> BUTTON0
// <BTN_1=> BUTTON1
// <i> Default: BTN_0
#define BUTTON_SELECT BTN_0
// </h> end BUTTON configuration

// <<< end of configuration section >>>

// $[BUTTON]
#define BSP_BUTTON_PRESENT (1)

#define BSP_BUTTON0_PIN  (2U) //Button0------->UULP GPIO2-------------->F12
#define BSP_BUTTON0_PORT (btn_gpioPortA)

#define BSP_BUTTON1_PIN  (11U)
#define BSP_BUTTON1_PORT (btn_gpioPortA) //Button1------->GPIO 11------------>F13

#define BSP_BUTTON_GPIO_DOUT (HAL_GPIO_DOUT_LOW)
#define BSP_BUTTON_GPIO_MODE (HAL_GPIO_MODE_INPUT)
// [BUTTON]$

#define HAL_SERIAL_APP_BAUD_RATE 115200
#define HAL_SERIAL_APP_RXSTART   16
#define HAL_SERIAL_APP_RXSTOP    16
#undef HAL_SERIAL_APP_RX_QUEUE_SIZE
#define HAL_SERIAL_APP_RX_QUEUE_SIZE 64
#define HAL_SERIAL_APP_TX_QUEUE_SIZE 128

#define HAL_BUTTON_COUNT (2)

#define HAL_SERIAL_VUART_ENABLE  (1)
#define HAL_SERIAL_RXWAKE_ENABLE (1)

#define HAL_VCOM_ENABLE (1)
#define HAL_WDOG_ENABLE (1)

#define HAL_GPIO_DOUT_LOW  false
#define HAL_GPIO_DOUT_HIGH true

#endif // BUTTON_USER_CONFIG_H