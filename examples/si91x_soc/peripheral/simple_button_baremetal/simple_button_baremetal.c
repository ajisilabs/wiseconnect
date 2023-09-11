/***************************************************************************/ /**
 * @file simple_button_baremetal.c
 * @brief Simple button baremetal examples functions
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

#include "simple_button_baremetal.h"
#include "rsi_board.h"
#include "rsi_chip.h"
#include "led.h"
#include "button.h"
#include "button_config.h"
#include "led_config.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#if BUTTON_SELECT == BTN_0
#ifndef BUTTON_INSTANCE_0
#define BUTTON_INSTANCE_0 BSP_BUTTON0_PIN
#endif
#endif

#if BUTTON_SELECT == BTN_1
#ifndef BUTTON_INSTANCE_0
#define BUTTON_INSTANCE_0 BSP_BUTTON1_PIN
#endif
#endif

#if LED_SELECT == LED_0
#ifndef LED_INSTANCE_0
#define LED_INSTANCE_0 BOARDLED0
#endif
#endif

#if LED_SELECT == LED_1
#ifndef LED_INSTANCE_0
#define LED_INSTANCE_0 BOARDLED1
#endif
#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initialize example.
 ******************************************************************************/
void simple_button_init(void)
{
  halInternalInitButton();
  halInternalInitLed();
}

/***************************************************************************/ /**
 * Call back on button change.
 ******************************************************************************/
void simple_button_process_action(void)
{
  if (halButtonState(BUTTON_INSTANCE_0) == BUTTON_PRESSED) {
    halToggleLed(LED_INSTANCE_0);
  }
}
