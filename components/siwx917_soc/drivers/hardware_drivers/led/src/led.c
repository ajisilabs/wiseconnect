/***************************************************************************/ /**
 * @file
 * @brief LED manipulation routines; stack and example APIs
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
// full hal needs the board header to get pulled in
#if !defined(MINIMAL_HAL) && defined(BOARD_HEADER)
#include BOARD_HEADER
#endif
#include "led.h"
#include "em_device.h"
#include "rsi_chip.h"
#include "rsi_board.h"
#include "si91x_device.h"

typedef struct {
  led_GPIO_Port_TypeDef port;
  uint8_t pin;
} tLedArray;

static const tLedArray ledArray[BSP_LED_COUNT] = BSP_LED_INIT;

// Stub out LED functions when there aren't any LEDs
#if HAL_LED_COUNT == 0
void halInternalInitLed(void)
{
  /* no-op */
}
void halSetLed(HalBoardLed led)
{
  /* no-op */
}
void halClearLed(HalBoardLed led)
{
  /* no-op */
}
void halToggleLed(HalBoardLed led)
{
  /* no-op */
}
#else //(HAL_LED_COUNT == 0)

void halInternalInitLed(void)
{
  uint8_t enableLeds[HAL_LED_COUNT] = HAL_LED_ENABLE;
#if defined(BSP_LED_POLARITY) && (BSP_LED_POLARITY == 0)

  RSI_EGPIO_SetPinMux(EGPIO1, ledArray[enableLeds[0]].port, ledArray[enableLeds[0]].pin, EGPIO_PIN_MUX_MODE0);
  // LED0--->ULP GPIO 2--->F12
  // Set output direction
  RSI_EGPIO_SetDir(EGPIO1, ledArray[enableLeds[0]].port, ledArray[enableLeds[0]].pin, EGPIO_CONFIG_DIR_OUTPUT);
  RSI_EGPIO_SetPin(EGPIO1, ledArray[enableLeds[0]].port, ledArray[enableLeds[0]].pin, 1);
  if (HAL_LED_COUNT == 2) {
    RSI_EGPIO_SetPinMux(EGPIO,
                        ledArray[enableLeds[1]].port,
                        ledArray[enableLeds[1]].pin,
                        EGPIO_PIN_MUX_MODE0); // LED1-->GPIO 10----->F13
    // Set output direction
    RSI_EGPIO_SetDir(EGPIO, ledArray[enableLeds[1]].port, ledArray[enableLeds[1]].pin, EGPIO_CONFIG_DIR_OUTPUT);
    RSI_EGPIO_SetPin(EGPIO, ledArray[enableLeds[1]].port, ledArray[enableLeds[1]].pin, 1);
  }

#else
  RSI_EGPIO_SetPinMux(EGPIO1, ledArray[enableLeds[0]].port, ledArray[enableLeds[0]].pin, EGPIO_PIN_MUX_MODE0);
  // LED0--->ULP GPIO 2--->F12
  // Set output direction
  RSI_EGPIO_SetDir(EGPIO1, ledArray[enableLeds[0]].port, ledArray[enableLeds[0]].pin, EGPIO_CONFIG_DIR_OUTPUT);
  RSI_EGPIO_SetPin(EGPIO1, ledArray[enableLeds[0]].port, ledArray[enableLeds[0]].pin, 0);
  if (HAL_LED_COUNT == 2) {
    RSI_EGPIO_SetPinMux(EGPIO,
                        ledArray[enableLeds[1]].port,
                        ledArray[enableLeds[1]].pin,
                        EGPIO_PIN_MUX_MODE0); // LED1-->GPIO 10----->F13
    // Set output direction
    RSI_EGPIO_SetDir(EGPIO, ledArray[enableLeds[1]].port, ledArray[enableLeds[1]].pin, EGPIO_CONFIG_DIR_OUTPUT);
    RSI_EGPIO_SetPin(EGPIO, ledArray[enableLeds[1]].port, ledArray[enableLeds[1]].pin, 0);
  }
#endif //(defined(BSP_LED_POLARITY) && (BSP_LED_POLARITY == 0))
}

void halSetLed(HalBoardLed led)
{
#if defined(BSP_LED_POLARITY) && (BSP_LED_POLARITY == 0)
  if (ledArray[led].pin == BSP_LED0_PIN)
    RSI_EGPIO_SetPin(EGPIO1, ledArray[led].port, ledArray[led].pin, 0);
  else
    RSI_EGPIO_SetPin(EGPIO, ledArray[led].port, ledArray[led].pin, 0);
#else
  // if(ledArray[led].pin==BSP_LED0_PIN)
  RSI_EGPIO_SetPin(EGPIO1, ledArray[led].port, ledArray[led].pin, 1);
  // else
  RSI_EGPIO_SetPin(EGPIO, ledArray[led].port, ledArray[led].pin, 1);
#endif //(defined(BSP_LED_POLARITY) && (BSP_LED_POLARITY == 0))
}

void halClearLed(HalBoardLed led)
{
#if defined(BSP_LED_POLARITY) && (BSP_LED_POLARITY == 0)
  if (ledArray[led].pin == BSP_LED0_PIN)
    RSI_EGPIO_SetPin(EGPIO1, ledArray[led].port, ledArray[led].pin, 1);
  else
    RSI_EGPIO_SetPin(EGPIO, ledArray[led].port, ledArray[led].pin, 1);
#else
  if (ledArray[led].pin == BSP_LED0_PIN)
    RSI_EGPIO_SetPin(EGPIO1, ledArray[led].port, ledArray[led].pin, 0);
  else
    RSI_EGPIO_SetPin(EGPIO, ledArray[led].port, ledArray[led].pin, 0);
#endif //(defined(BSP_LED_POLARITY) && (BSP_LED_POLARITY == 0))
}

void halToggleLed(HalBoardLed led)
{
  if (ledArray[led].pin == BSP_LED0_PIN)
    RSI_Board_LED_Toggle(0);
  else
    RSI_Board_LED_Toggle(1);
}
#endif ////(defined(BSP_LED_POLARITY) && (BSP_LED_POLARITY == 0))

#ifndef MINIMAL_HAL
void halStackIndicateActivity(bool turnOn)
{
#if NO_LED
  // Don't touch LEDs when built with NO_LED
#else  //!NO_LED
  if (turnOn) {
    halSetLed(BOARD_ACTIVITY_LED);
  } else {
    halClearLed(BOARD_ACTIVITY_LED);
  }
#endif //NO_LED
}

#endif //MINIMAL_HAL
