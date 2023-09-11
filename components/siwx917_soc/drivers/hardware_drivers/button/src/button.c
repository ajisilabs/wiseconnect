/***************************************************************************/ /**
 * @file
 * @brief  Sample API functions for operating buttons (external interrupts).
 *  Please see the board header file for the definition of the BUTTON*
 *  instances used below.  The BUTTON* defines used here are designed to make
 *  this library generic such that only the board header needs to be modified
 *  to change which GPIO the buttons are on.
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

#include "rsi_chip.h"
#include "rsi_board.h"
#include "button.h"
#include "button_config.h"
#include "si91x_device.h"

#define PININT2_NVIC_NAME    NPSS_TO_MCU_GPIO_INTR_IRQn /* External interrupt number */
#define NPSS_GPIO_IRQHandler IRQ021_Handler             /* UULP GPIO interrupt IRQ function name   */

#define PININT_IRQ_HANDLER IRQ059_Handler   /* GPIO interrupt IRQ function name            */
#define PININT_NVIC_NAME   EGPIO_PIN_7_IRQn /* GPIO interrupt NVIC interrupt name          */
#define M4_GPIO_PORT       BSP_BUTTON1_PORT /* GPIO port number           0                 */
#define M4_GPIO_PIN        BSP_BUTTON1_PIN  /* GPIO pin number                             */
#define PIN_INT            7                /* Pin  interrupt number(0 to 7)               */

// This state is kept track of so the IRQ ISRs knows when to notify buttonIsrs.
#if (HAL_BUTTON_COUNT > 0)
static uint8_t buttonState[BSP_BUTTON_COUNT];
void halInternalButtonIsr(uint8_t pin);
typedef struct {
  btn_GPIO_Port_TypeDef port;
  uint8_t pin;
} tButtonArray;
static const tButtonArray buttonArray[BSP_BUTTON_COUNT] = BSP_BUTTON_INIT;
#endif //(HAL_BUTTON_COUNT > 0)

#define HAL_BUTTON_INVALID_INDEX BSP_BUTTON_COUNT
#if (HAL_BUTTON_COUNT > 0)
static uint8_t halInternalButtonIndex(uint8_t pin)
{
  uint8_t i;
  for (i = 0; i < BSP_BUTTON_COUNT; i++) {
    if (buttonArray[i].pin == pin) {
      break;
    }
  }
  return i;
}
#endif //(HAL_BUTTON_COUNT > 0)

/**
 * @brief Interrupt handler
 * @return  Nothing
 */

void NPSS_GPIO_IRQHandler(void)
{
  volatile uint32_t intrStatus = 0;

  intrStatus = RSI_NPSSGPIO_GetIntrStatus();

  if (intrStatus & NPSS_GPIO_2_INTR) {
    RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);
    halInternalButtonIsr(BSP_BUTTON0_PIN);
  }
}
/**
 * @brief Interrupt handler
 * @return  Nothing
 */
void PININT_IRQ_HANDLER(void)
{
  uint32_t gintStatus;

  /*get interrupt status*/
  gintStatus = RSI_EGPIO_GetIntStat(EGPIO, PIN_INT);

  if ((gintStatus & EGPIO_PIN_INT_CLR_RISING) || (gintStatus & EGPIO_PIN_INT_CLR_FALLING)) {
    /*clear interrupt*/
    RSI_EGPIO_IntClr(EGPIO, PIN_INT, INTERRUPT_STATUS_CLR);
  } else {
    RSI_EGPIO_IntMask(EGPIO, PIN_INT);
  }
  halInternalButtonIsr(BSP_BUTTON1_PIN);
}

void halInternalInitButton(void)
{
#if (HAL_BUTTON_COUNT > 0)
  uint8_t enableButtons[HAL_BUTTON_COUNT] = HAL_BUTTON_ENABLE;
  //Button0------->UULP GPIO2-------------->F12
  RSI_NPSSGPIO_InputBufferEn(buttonArray[enableButtons[0]].pin, 1);

  /* Set the NPSS GPIO pin MUX */
  RSI_NPSSGPIO_SetPinMux(buttonArray[enableButtons[0]].pin, 0);

  /* Set the direction of the NPSS GPIO */
  RSI_NPSSGPIO_SetDir(buttonArray[enableButtons[0]].pin, NPSS_GPIO_DIR_INPUT);
  RSI_NPSSGPIO_SetPin(buttonArray[enableButtons[0]].pin, BSP_BUTTON_GPIO_DOUT);
  /* Enables rise edge interrupt detection for UULP_VBAT_GPIO_2 */
  RSI_NPSSGPIO_SetIntRiseEdgeEnable(NPSS_GPIO_2_INTR);

  /* Enables fall edge interrupt detection for UULP_VBAT_GPIO_2 */
  RSI_NPSSGPIO_SetIntFallEdgeEnable(NPSS_GPIO_2_INTR);

  /* un mask the NPSS GPIO interrupt */
  RSI_NPSSGPIO_IntrUnMask(NPSS_GPIO_2_INTR);

  /*NVIC enable */
  NVIC_EnableIRQ(PININT2_NVIC_NAME);
  if (HAL_BUTTON_COUNT == 2) {
    uint8_t pad_sel = 1;
    /*Enable clock for EGPIO module*/
    RSI_CLK_PeripheralClkEnable(M4CLK, EGPIO_CLK, ENABLE_STATIC_CLK);

    /*PAD selection*/
    for (pad_sel = 1; pad_sel < 34; pad_sel++) {
      if (pad_sel != 9) {
        RSI_EGPIO_PadSelectionEnable(pad_sel);
      }
    }

    RSI_EGPIO_SetDir(EGPIO, M4_GPIO_PORT, M4_GPIO_PIN, 1);
    /*REN enable */
    RSI_EGPIO_PadReceiverEnable(M4_GPIO_PIN);

    /*Configure default GPIO mode(0) */
    RSI_EGPIO_SetPinMux(EGPIO, M4_GPIO_PORT, M4_GPIO_PIN, EGPIO_PIN_MUX_MODE0);

    /*Selects the pin interrupt for the GPIO*/
    RSI_EGPIO_PinIntSel(EGPIO, PIN_INT, M4_GPIO_PORT, M4_GPIO_PIN);
    RSI_EGPIO_SetIntRiseEdgeEnable(EGPIO, PIN_INT);

    RSI_EGPIO_SetIntFallEdgeEnable(EGPIO, PIN_INT);

    /*Unmask the  interrupt*/
    RSI_EGPIO_IntUnMask(EGPIO, PIN_INT);

    /*NVIC enable */
    NVIC_EnableIRQ(PININT_NVIC_NAME);
    RSI_EGPIO_IntUnMask(EGPIO, PIN_INT);
  }

#endif //(HAL_BUTTON_COUNT > 0)
}

uint8_t halButtonState(uint8_t button)
{
  // Note: this returns the "soft" state rather than reading the port
  //  so it gives with the interrupts and their callbacks
#if (HAL_BUTTON_COUNT > 0)
  uint8_t index = halInternalButtonIndex(button);
  if (index != HAL_BUTTON_INVALID_INDEX) {
    return buttonState[index];
  }
#endif //(HAL_BUTTON_COUNT > 0)
  // return BUTTON_RELEASED if no matching button was found
  return BUTTON_RELEASED;
}

void halInternalButtonStateSet(uint8_t button, uint8_t state)
{
  // Note: this sets the "soft" state
  //  so it gives with the interrupts and their callbacks
#if (HAL_BUTTON_COUNT > 0)
  uint8_t index = halInternalButtonIndex(button);
  if (index != HAL_BUTTON_INVALID_INDEX) {
    buttonState[index] = state;
  }
#endif //(HAL_BUTTON_COUNT > 0)
}

void halInternalButtonToggleState(uint8_t button)
{
  // Note: this toggles the "soft" state
  //  so it gives with the interrupts and their callbacks
#if (HAL_BUTTON_COUNT > 0)
  uint8_t index = halInternalButtonIndex(button);
  if (index != HAL_BUTTON_INVALID_INDEX) {
    buttonState[index] = !buttonState[index];
  }
#endif //(HAL_BUTTON_COUNT > 0)
}

uint8_t halButtonPinState(uint8_t button)
{
#if (HAL_BUTTON_COUNT > 0)
  uint8_t index = halInternalButtonIndex(button);
  if (index != HAL_BUTTON_INVALID_INDEX) {
    // index=1;
    if (buttonArray[index].pin == BSP_BUTTON1_PIN)
      return RSI_EGPIO_GetPin(EGPIO, buttonArray[index].port, buttonArray[index].pin) ? BUTTON_RELEASED
                                                                                      : BUTTON_PRESSED;
    else
      return RSI_NPSSGPIO_GetPin(buttonArray[index].pin) ? BUTTON_RELEASED : BUTTON_PRESSED;
  }
#endif //(HAL_BUTTON_COUNT > 0)
  return BUTTON_RELEASED;
}

/***************************************************************************/ /**
*
* DEBOUNCE operation is based upon the theory that when multiple reads in a row
* return the same value, we have passed any debounce created by the mechanical
* action of a button.  The define "DEBOUNCE" says how many reads in a row
* should return the same value.  The value '5', below, is the recommended value
* since this should require the signal to have stabalized for approximately
* 100us which should be much longer than any debounce action.
* Uncomment the following line to enable software debounce operation:
* #define DEBOUNCE 5
*
 ******************************************************************************/

//Typically, software debounce is disabled by defaulting to a value of '0'
//which will cause the preprocessor to strip out the debounce code and save
//flash space.
#ifndef DEBOUNCE
#define DEBOUNCE 5
#endif //DEBOUNCE

#if (HAL_BUTTON_COUNT > 0)
void halInternalButtonIsr(uint8_t pin)
{
  uint8_t buttonStateNow;
#if (DEBOUNCE > 0)
  uint8_t buttonStatePrev;
  uint32_t debounce;
#endif //(DEBOUNCE > 0)

  buttonStateNow = halButtonPinState(pin);
#if (DEBOUNCE > 0)
  //read button until get "DEBOUNCE" number of consistent readings
  for (debounce = 0; debounce < DEBOUNCE; debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0) {
    buttonStatePrev = buttonStateNow;
    buttonStateNow  = halButtonPinState(pin);
  }
#endif //(DEBOUNCE > 0)

  if (halButtonState(pin) != buttonStateNow) { //state changed, notify app
    halInternalButtonStateSet(pin, buttonStateNow);
    halButtonIsr(pin, halButtonState(pin));
  } else {                             //state unchanged, then notify app of a double-transition
    halInternalButtonToggleState(pin); //invert temporarily for calling Isr
    halButtonIsr(pin, halButtonState(pin));
    halInternalButtonToggleState(pin); //and put it back to current state
    halButtonIsr(pin, halButtonState(pin));
  }
}

void halButtonIsr(uint8_t button, uint8_t state)
{
  (void)button;
  (void)state;
}
#endif // (HAL_BUTTON_COUNT > 0)
