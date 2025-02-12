/***************************************************************************/ /**
 * @file i2c_leader_example.c
 * @brief I2C examples functions
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
#include "sl_si91x_peripheral_i2c.h"
#include "i2c_leader_example.h"
#include "rsi_debug.h"
#include "rsi_rom_egpio.h"
#include "rsi_power_save.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SIZE_BUFFERS                         15           // Size of buffer
#define FOLLOWER_I2C_ADDR                    0x50         // EEPROM I2C address
#define RX_LENGTH                            SIZE_BUFFERS // Number of bytes to receive
#define TX_LENGTH                            SIZE_BUFFERS // Number of bytes to send
#define OFFSET_LENGTH                        1            // Offset length
#define FIFO_THRESHOLD                       0x0          // FIFO threshold
#define ZERO_FLAG                            0            // Zero flag, No argument
#define PINMUX_MODE                          6            // I2C pinmux mode
#define PORT_ZERO                            0            // Port zero
#define HP_MAX_GPIO                          64           // High Power GPIO Maximum number
#define LAST_DATA_COUNT                      0            // Last read-write count
#define DATA_COUNT                           1            // Last second data count for verification
#define BIT_SET                              1            // Set bit
#define STOP_BIT                             9            // Bit to send stop command
#define RW_MASK_BIT                          8            // Bit to mask read and write
#define I2C_INSTANCE                         2            // I2C Instance for Pin configuration
#define I2C                                  I2C2         // I2C Instance
#define MAX_7BIT_ADDRESS                     127          // Maximum 7-bit address
#define REFERENCE_CLOCK_FREQUENCY            (32000000u)  // Reference clock frequency
#define HIGH_SPEED_REFERENCE_CLOCK_FREQUENCY (40000000u)  // Reference clock frequency
#define I2C_STANDARD_MODE_CLOCK_FREQUENCY    (32000000u)  // clock frequency for i2c standard mode
#define I2C_FAST_MODE_CLOCK_FREQUENCY        (32000000u)  // clock frequency for i2c Fast mode
#define I2C_FAST_PLUS_MODE_CLOCK_FREQUENCY   (80000000u)  // clock frequency for i2c fast plus mode
#define I2C_HIGH_SPEED_MODE_CLOCK_FREQUENCY  (80000000u)  // clock frequency for i2c high speed mode
#define ULP_CLOCK_DIV_FACTOR                 0            // division factor value for ULP clock
#define ULP_PRO_CLOCK_DIV_FACTOR             0            // division factor value for ULP pro clock
#define EVEN_DIVISION_FACTOR                 0            // ulp clock division factor type
#define DELAY_DISABLE                        0            // to disable delay function callback for ulp pro clock
/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
// Structure to hold the pin configuration
typedef const struct {
  uint8_t port;    // GPIO port
  uint8_t pin;     // GPIO pin
  uint8_t mode;    // GPIO mode
  uint8_t pad_sel; // GPIO pad selection
} I2C_PIN_;

// Enum for different transmission scenarios
typedef enum {
  SEND_DATA,              // Send mode
  RECEIVE_DATA,           // Receive mode
  TRANSMISSION_COMPLETED, // Transmission completed mode
} i2c_mode_enum_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
#if (I2C_INSTANCE == 0)
static I2C_PIN scl = { RTE_I2C0_SCL_PORT, RTE_I2C0_SCL_PIN, RTE_I2C0_SCL_MUX, RTE_I2C0_SCL_PAD };
static I2C_PIN sda = { RTE_I2C0_SDA_PORT, RTE_I2C0_SDA_PIN, RTE_I2C0_SDA_MUX, RTE_I2C0_SDA_PAD };
#endif

#if (I2C_INSTANCE == 1)
static I2C_PIN scl = { RTE_I2C1_SCL_PORT, RTE_I2C1_SCL_PIN, RTE_I2C1_SCL_MUX, RTE_I2C1_SCL_PAD };
static I2C_PIN sda = { RTE_I2C1_SDA_PORT, RTE_I2C1_SDA_PIN, RTE_I2C1_SDA_MUX, RTE_I2C1_SDA_PAD };
#endif

#if (I2C_INSTANCE == 2)
static I2C_PIN scl = { RTE_I2C2_SCL_PORT, RTE_I2C2_SCL_PIN, RTE_I2C2_SCL_MUX, 0 };
static I2C_PIN sda = { RTE_I2C2_SDA_PORT, RTE_I2C2_SDA_PIN, RTE_I2C2_SDA_MUX, 0 };
#endif

static i2c_mode_enum_t current_mode = SEND_DATA;
boolean_t i2c_send_complete         = false;
boolean_t i2c_receive_complete      = false;
boolean_t send_data_flag            = false;
boolean_t receive_data_flag         = false;
static uint32_t write_number        = 0;
static uint32_t write_count         = 0;
static uint32_t read_number         = 0;
static uint32_t read_count          = 0;
static uint8_t *write_data;
static uint8_t *read_data;
static uint8_t read_buffer[SIZE_BUFFERS + OFFSET_LENGTH];
static uint8_t write_buffer[SIZE_BUFFERS];

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pin_configurations(void);
static void i2c_send_data(const uint8_t *data, uint32_t data_length, uint16_t follower_address);
static void i2c_receive_data(uint8_t *data, uint32_t data_length, uint16_t follower_address);
static void i2c_clock_init(I2C_TypeDef *i2c);
static void compare_data(void);
static void handle_leader_transmit_irq(void);
static void handle_leader_receive_irq(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * I2C example initialization function
 ******************************************************************************/
void i2c_leader_example_init(void)
{
  i2c_clock_init(I2C);
  sl_i2c_init_params_t config;
  // Filling the structure with default values.
  config.clhr = SL_I2C_STANDARD_BUS_SPEED;
  if (config.clhr == SL_I2C_STANDARD_BUS_SPEED) {
    RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
    RSI_CLK_SetSocPllFreq(M4CLK, I2C_STANDARD_MODE_CLOCK_FREQUENCY, REFERENCE_CLOCK_FREQUENCY);
    RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
    config.freq = I2C_STANDARD_MODE_CLOCK_FREQUENCY;
  }
  if (config.clhr == SL_I2C_FAST_BUS_SPEED) {
    RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
    RSI_CLK_SetSocPllFreq(M4CLK, I2C_FAST_MODE_CLOCK_FREQUENCY, REFERENCE_CLOCK_FREQUENCY);
    RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
    config.freq = I2C_FAST_MODE_CLOCK_FREQUENCY;
  }
  if (config.clhr == SL_I2C_FAST_PLUS_BUS_SPEED) {
    RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
    RSI_CLK_SetSocPllFreq(M4CLK, I2C_FAST_PLUS_MODE_CLOCK_FREQUENCY, REFERENCE_CLOCK_FREQUENCY);
    RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
    if (I2C == I2C2) {
      RSI_ULPSS_ClockConfig(M4CLK, ENABLE, ULP_CLOCK_DIV_FACTOR, EVEN_DIVISION_FACTOR);
      RSI_ULPSS_UlpProcClkConfig(ULPCLK, ULP_PROC_SOC_CLK, ULP_PRO_CLOCK_DIV_FACTOR, DELAY_DISABLE);
      DEBUGINIT();
    }
    config.freq = sl_si91x_i2c_get_frequency(I2C);
  }
  if (config.clhr == SL_I2C_HIGH_BUS_SPEED) {
    RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
    RSI_CLK_SetSocPllFreq(M4CLK, I2C_HIGH_SPEED_MODE_CLOCK_FREQUENCY, HIGH_SPEED_REFERENCE_CLOCK_FREQUENCY);
    RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
    if (I2C == I2C2) {
      RSI_ULPSS_ClockConfig(M4CLK, ENABLE, ULP_CLOCK_DIV_FACTOR, EVEN_DIVISION_FACTOR);
      RSI_ULPSS_UlpProcClkConfig(ULPCLK, ULP_PROC_SOC_CLK, ULP_PRO_CLOCK_DIV_FACTOR, DELAY_DISABLE);
      DEBUGINIT();
    }
    config.freq = sl_si91x_i2c_get_frequency(I2C);
  }
  config.mode = SL_I2C_LEADER_MODE;
  // Passing the structure and i2c instance for the initialization.
  sl_si91x_i2c_init(I2C, &config);
  DEBUGOUT("I2C leader is initialized successfully \n");
  // Pin is configured here.
  pin_configurations();
  DEBUGOUT("Pins are configured successfully \n");
  // Generating a buffer with values that needs to be sent.
  for (uint32_t loop = 0; loop < SIZE_BUFFERS; loop++) {
    write_buffer[loop] = (uint8_t)(loop + 0x1);
    read_buffer[loop]  = 0;
  }
  send_data_flag = true;
}

/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void i2c_leader_example_process_action(void)
{
  // In switch case, according to the current mode, the transmission is executed.
  // First EEPROM write is executes i.e., I2C send.
  // After that offset address is sent to the follower device.
  // Now data is read from the EEPROM using I2C recevie.
  switch (current_mode) {
    case SEND_DATA:
      if (send_data_flag) {
        // Validation for executing the API only once.
        i2c_send_data(write_buffer, TX_LENGTH + OFFSET_LENGTH, FOLLOWER_I2C_ADDR);
        send_data_flag = false;
      }
      if (i2c_send_complete) {
        // It waits till i2c_send_complete is true in IRQ handler.
        current_mode      = RECEIVE_DATA;
        receive_data_flag = true;
        i2c_send_complete = false;
      }
      break;
    case RECEIVE_DATA:
      if (receive_data_flag) {
        // Validation for executing the API only once.
        i2c_receive_data(read_buffer, RX_LENGTH + OFFSET_LENGTH, FOLLOWER_I2C_ADDR);
        receive_data_flag = false;
      }
      if (i2c_receive_complete) {
        // It waits till i2c_receive_complete is true in IRQ handler.
        current_mode         = TRANSMISSION_COMPLETED;
        i2c_receive_complete = false;
        // After the receive is completed, input and output data is compared and
        // output is printed on console.
        compare_data();
      }
      break;
    case TRANSMISSION_COMPLETED:
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Function to send the data using I2C.
 * Here the FIFO threshold, direction and interrupts are configured.
 * 
 * @param[in] data (uint8_t) Constant pointer to the data which needs to be transferred.
 * @param[in] data_length (uint32_t) Length of the data that needs to be received.
 * @param[in] follower address
 * @return none
 ******************************************************************************/
static void i2c_send_data(const uint8_t *data, uint32_t data_length, uint16_t follower_address)
{
  bool is_10bit_addr = false;
  // Disables the interrupts.
  sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
  // Updates the variables which are required for trasmission.
  write_data   = (uint8_t *)data;
  write_count  = 0;
  write_number = data_length;
  // Disables the I2C peripheral.
  sl_si91x_i2c_disable(I2C);
  // Checking is address is 7-bit or 10bit
  if (follower_address > MAX_7BIT_ADDRESS) {
    is_10bit_addr = true;
  }
  // Setting the follower address recevied in parameter structure.
  sl_si91x_i2c_set_follower_address(I2C, follower_address, is_10bit_addr);
  // Configures the FIFO threshold.
  sl_si91x_i2c_set_tx_threshold(I2C, FIFO_THRESHOLD);
  // Enables the I2C peripheral.
  sl_si91x_i2c_enable(I2C);
  // Configures the transmit empty interrupt.
  sl_si91x_i2c_set_interrupts(I2C, SL_I2C_EVENT_TRANSMIT_EMPTY);
  // Enables the interrupt.
  sl_si91x_i2c_enable_interrupts(I2C, ZERO_FLAG);
}

/*******************************************************************************
* Function to receive the data using I2C.
 * Here the FIFO threshold, direction and interrupts are configured.
 * 
 * @param[in] data (uint8_t) Constant pointer to the data which needs to be transferred.
 * @param[in] data_length (uint32_t) Length of the data that needs to be received.
 * @param[in] follower address
 * @return none
 ******************************************************************************/
static void i2c_receive_data(uint8_t *data, uint32_t data_length, uint16_t follower_address)
{
  bool is_10bit_addr = false;
  // Disables the interrupts.
  sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
  // Updates the variables which are required for trasmission.
  read_data   = (uint8_t *)data;
  read_count  = 0;
  read_number = data_length;
  // Disables the I2C peripheral.
  sl_si91x_i2c_disable(I2C);
  // Checking is address is 7-bit or 10bit
  if (follower_address > MAX_7BIT_ADDRESS) {
    is_10bit_addr = true;
  }
  // Setting the follower address recevied in parameter structure.
  sl_si91x_i2c_set_follower_address(I2C, follower_address, is_10bit_addr);
  // Configures the FIFO threshold.
  sl_si91x_i2c_set_rx_threshold(I2C, FIFO_THRESHOLD);
  // Enables the I2C peripheral.
  sl_si91x_i2c_enable(I2C);
  // Sets the direction to read.
  sl_si91x_i2c_control_direction(I2C, SL_I2C_READ_MASK);
  // Configures the receive full interrupt.
  sl_si91x_i2c_set_interrupts(I2C, SL_I2C_EVENT_RECEIVE_FULL);
  // Enables the interrupt.
  sl_si91x_i2c_enable_interrupts(I2C, ZERO_FLAG);
}

/*******************************************************************************
 * To configure the clock and power up the peripheral according to the 
 * I2C instance.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void i2c_clock_init(I2C_TypeDef *i2c)
{
  if ((uint32_t)i2c == I2C0_BASE) {
#if defined(SLI_SI917)
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
    // Initialize the I2C clock.
    RSI_CLK_I2CClkConfig(M4CLK, 1, I2C1_INSTAN);
  }
  if ((uint32_t)i2c == I2C1_BASE) {
#if defined(SLI_SI917)
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI3);
#endif
    // Initialize the I2C clock.
    RSI_CLK_I2CClkConfig(M4CLK, 1, I2C2_INSTAN);
  }
  if ((uint32_t)i2c == I2C2_BASE) {
    // Powering up the peripheral.
    RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_I2C);
    // Initialize the I2C clock.
    RSI_ULPSS_PeripheralEnable(ULPCLK, ULP_I2C_CLK, ENABLE_STATIC_CLK);
  }
}

/*******************************************************************************
 * Function to compare the input and output data
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void compare_data(void)
{
  uint32_t data_index = 0;
  for (data_index = 0; data_index < SIZE_BUFFERS; data_index++) {
    // If the read_buffer and buffer are same, it will be continued else, the for
    // loop will be break.
    if (write_buffer[data_index] != read_buffer[data_index]) {
      break;
    }
  }
  if (data_index == SIZE_BUFFERS) {
    DEBUGOUT("Leader-Follower read-write Data comparison is successful, Test Case Passed \n");
  } else {
    DEBUGOUT("Leader-Follower read-write Data comparison is not successful, Test Case Failed \n");
  }
}

/*******************************************************************************
 * Function to set the Pin configuration for I2C.
 * It configures the SDA and SCL pins.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void pin_configurations(void)
{
#if (I2C_INSTANCE == 0)
#ifndef SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2
  //SCL
  RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(scl.pin - HP_MAX_GPIO));
  RSI_EGPIO_SetPinMux(EGPIO1, PORT_ZERO, (uint8_t)(scl.pin - HP_MAX_GPIO), PINMUX_MODE);
  RSI_EGPIO_PadSelectionEnable(scl.pad_sel);
  //configure DIN0 pin
  RSI_EGPIO_SetPinMux(EGPIO, scl.port, scl.pin, scl.mode);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
  // Configuring internal pullup for follower mode
  egpio_ulp_pad_driver_disable_state(ULP_GPIO_SCL, INTERNAL_PULLUP);
#endif
  //SDA
  RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(sda.pin - HP_MAX_GPIO));
  RSI_EGPIO_SetPinMux(EGPIO1, PORT_ZERO, (uint8_t)(sda.pin - HP_MAX_GPIO), PINMUX_MODE);
  RSI_EGPIO_PadSelectionEnable(sda.pad_sel);
  //configure DIN0 pin
  RSI_EGPIO_SetPinMux(EGPIO, sda.port, sda.pin, sda.mode);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
  // Configuring internal pullup for follower mode
  egpio_ulp_pad_driver_disable_state(ULP_GPIO_SDA, INTERNAL_PULLUP);
#endif
#else
  RSI_EGPIO_PadSelectionEnable(scl.pad_sel);
  RSI_EGPIO_PadReceiverEnable(scl.pin);
  RSI_EGPIO_SetPinMux(EGPIO, scl.port, scl.pin, scl.mode);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
  egpio_pad_driver_disable_state(scl.pin, INTERNAL_PULLUP);
#endif
  //SDA
  RSI_EGPIO_PadSelectionEnable(sda.pad_sel);
  RSI_EGPIO_PadReceiverEnable(sda.pin);
  RSI_EGPIO_SetPinMux(EGPIO, sda.port, sda.pin, sda.mode);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
  egpio_pad_driver_disable_state(sda.pin, INTERNAL_PULLUP);
#endif
#endif
#endif // I2C_INSTANCE 0

#if (I2C_INSTANCE == 1)
  //SCL
  RSI_EGPIO_PadSelectionEnable(scl.pad_sel);
  RSI_EGPIO_PadReceiverEnable(scl.pin);
  RSI_EGPIO_SetPinMux(EGPIO, scl.port, scl.pin, scl.mode);
  //SDA
  RSI_EGPIO_PadSelectionEnable(sda.pad_sel);
  RSI_EGPIO_PadReceiverEnable(sda.pin);
  RSI_EGPIO_SetPinMux(EGPIO, sda.port, sda.pin, sda.mode);
#endif // I2C_INSTANCE 1

#if (I2C_INSTANCE == 2)
  // SCL
  RSI_EGPIO_UlpPadReceiverEnable(scl.pin);
  RSI_EGPIO_SetPinMux(EGPIO1, scl.port, scl.pin, scl.mode);
  // SDA
  RSI_EGPIO_UlpPadReceiverEnable(sda.pin);
  RSI_EGPIO_SetPinMux(EGPIO1, sda.port, sda.pin, sda.mode);
#endif // I2C_INSTANCE 2
}

/*******************************************************************************
 * Function to handle the transmit IRQ.
 * Transmit empty interrupt is monitored and byte by byte data is transmitted.
 * If the data transmission is completed, it clears and disables the interrupt.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void handle_leader_transmit_irq(void)
{
  if (write_number > LAST_DATA_COUNT) {
    if (write_number == DATA_COUNT) {
      I2C->IC_DATA_CMD = (uint32_t)write_data[write_count] | (BIT_SET << STOP_BIT);
    } else {
      sl_si91x_i2c_tx(I2C, write_data[write_count]);
    }
    write_count++;
    write_number--;
  } else {
    sl_si91x_i2c_clear_interrupts(I2C, SL_I2C_EVENT_TRANSMIT_EMPTY);
    sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
    i2c_send_complete = true;
  }
}

/*******************************************************************************
 * Function to handle the receive IRQ.
 * Receive full interrupt is monitored and byte by byte data is received.
 * If the data receive is completed, it clears and disables the interrupt.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void handle_leader_receive_irq(void)
{
  if (read_number > LAST_DATA_COUNT) {
    read_data[read_count] = I2C->IC_DATA_CMD_b.DAT;
    read_count++;
    read_number--;
    if (read_number == DATA_COUNT) {
      // If the last byte is there to receive, and in leader mode, it needs to send
      // the stop byte.
      I2C->IC_DATA_CMD = (BIT_SET << RW_MASK_BIT) | (BIT_SET << STOP_BIT);
    }
    if (read_number > DATA_COUNT) {
      I2C->IC_DATA_CMD = (BIT_SET << RW_MASK_BIT);
    }
  }
  if (read_number == LAST_DATA_COUNT) {
    sl_si91x_i2c_clear_interrupts(I2C, SL_I2C_EVENT_RECEIVE_FULL);
    sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
    i2c_receive_complete = true;
  }
}

/*******************************************************************************
 * IRQ handler for I2C 0.
 ******************************************************************************/
void I2C0_IRQHandler(void)
{
  uint32_t status = 0;
  uint32_t clear  = 0;
  // Checking interrupt status
  status = I2C0->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_ABORT) {
    uint32_t tx_abrt = I2C0->IC_TX_ABRT_SOURCE;
    if (tx_abrt & TX_ABRT_7B_ADDR_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C0->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_7B_ADDRESS_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C0->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS1_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C0->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS2_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C0->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_TX_DATA_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C0->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_NOACK) {
      // Clearing interrupt by reading the respective bit
      clear = I2C0->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_READ) {
      // Clearing interrupt by reading the respective bit
      clear = I2C0->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_10B_READ_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_DISABLED) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_TX_TX_FLUSH_CNT) {
    }
    if (tx_abrt & SL_I2C_ABORT_USER_ABORT) {
    }
    if (tx_abrt & SL_I2C_ABORT_SDA_STUCK_AT_LOW) {
      I2C0->IC_ENABLE_b.SDA_STUCK_RECOVERY_ENABLE = 0x1;
    }
    // Clearing all interrupts
    clear = I2C0->IC_CLR_INTR;
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(I2C0, SL_I2C_EVENT_TRANSMIT_EMPTY);
  }
  if (status & (SL_I2C_EVENT_SCL_STUCK_AT_LOW)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_MST_ON_HOLD)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_START_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_START_DET_b.CLR_START_DET;
    return;
  }
  if (status & (SL_I2C_EVENT_STOP_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear              = I2C0->IC_CLR_STOP_DET_b.CLR_STOP_DET;
    uint32_t maskReg   = 0;
    maskReg            = I2C0->IC_INTR_MASK;
    I2C0->IC_INTR_MASK = (maskReg & (~SL_I2C_EVENT_RECEIVE_FULL));
    return;
  }
  if (status & (SL_I2C_EVENT_ACTIVITY_ON_BUS)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_ACTIVITY_b.CLR_ACTIVITY;
    return;
  }
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    handle_leader_transmit_irq();
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    handle_leader_receive_irq();
  }
  if (status & (SL_I2C_EVENT_RECEIVE_UNDER)) {
    clear = I2C0->IC_CLR_RX_UNDER_b.CLR_RX_UNDER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_OVER)) {
    clear = I2C0->IC_CLR_RX_OVER_b.CLR_RX_OVER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_DONE)) {
    sl_si91x_i2c_clear_interrupts(I2C0, SL_I2C_EVENT_RECEIVE_DONE);
    return;
  }
  if (status & (SL_I2C_EVENT_GENERAL_CALL)) {
    sl_si91x_i2c_clear_interrupts(I2C0, SL_I2C_EVENT_GENERAL_CALL);
    return;
  }
  if (status & (SL_I2C_EVENT_RESTART_DET)) {
    sl_si91x_i2c_clear_interrupts(I2C0, SL_I2C_EVENT_RESTART_DET);
    return;
  }
  // to avoid unused variable warning
  (void)clear;
}

/*******************************************************************************
 * IRQ handler for I2C 1.
 ******************************************************************************/
void I2C1_IRQHandler(void)
{
  uint32_t status = 0;
  uint32_t clear  = 0;
  // Checking interrupt status
  status = I2C1->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_ABORT) {
    uint32_t tx_abrt = I2C1->IC_TX_ABRT_SOURCE;
    if (tx_abrt & TX_ABRT_7B_ADDR_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C1->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_7B_ADDRESS_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C1->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS1_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C1->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS2_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C1->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_TX_DATA_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C1->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_NOACK) {
      // Clearing interrupt by reading the respective bit
      clear = I2C1->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_READ) {
      // Clearing interrupt by reading the respective bit
      clear = I2C1->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_10B_READ_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_DISABLED) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_TX_TX_FLUSH_CNT) {
    }
    if (tx_abrt & SL_I2C_ABORT_USER_ABORT) {
    }
    if (tx_abrt & SL_I2C_ABORT_SDA_STUCK_AT_LOW) {
      I2C1->IC_ENABLE_b.SDA_STUCK_RECOVERY_ENABLE = 0x1;
    }
    // Clearing all interrupts
    clear = I2C1->IC_CLR_INTR;
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(I2C1, SL_I2C_EVENT_TRANSMIT_EMPTY);
  }
  if (status & (SL_I2C_EVENT_SCL_STUCK_AT_LOW)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_MST_ON_HOLD)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_START_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_START_DET_b.CLR_START_DET;
    return;
  }
  if (status & (SL_I2C_EVENT_STOP_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear              = I2C1->IC_CLR_STOP_DET_b.CLR_STOP_DET;
    uint32_t maskReg   = 0;
    maskReg            = I2C1->IC_INTR_MASK;
    I2C0->IC_INTR_MASK = (maskReg & (~SL_I2C_EVENT_RECEIVE_FULL));
    return;
  }
  if (status & (SL_I2C_EVENT_ACTIVITY_ON_BUS)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_ACTIVITY_b.CLR_ACTIVITY;
    return;
  }
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    handle_leader_transmit_irq();
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    handle_leader_receive_irq();
  }
  if (status & (SL_I2C_EVENT_RECEIVE_UNDER)) {
    clear = I2C1->IC_CLR_RX_UNDER_b.CLR_RX_UNDER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_OVER)) {
    clear = I2C1->IC_CLR_RX_OVER_b.CLR_RX_OVER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_DONE)) {
    sl_si91x_i2c_clear_interrupts(I2C1, SL_I2C_EVENT_RECEIVE_DONE);
    return;
  }
  if (status & (SL_I2C_EVENT_GENERAL_CALL)) {
    sl_si91x_i2c_clear_interrupts(I2C1, SL_I2C_EVENT_GENERAL_CALL);
    return;
  }
  if (status & (SL_I2C_EVENT_RESTART_DET)) {
    sl_si91x_i2c_clear_interrupts(I2C1, SL_I2C_EVENT_RESTART_DET);
    return;
  }
  // to avoid unused variable warning
  (void)clear;
}

/*******************************************************************************
 * IRQ handler for I2C 2.
 ******************************************************************************/
void I2C2_IRQHandler(void)
{
  uint32_t status = 0;
  uint32_t clear  = 0;
  // Checking interrupt status
  status = I2C2->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_ABORT) {
    uint32_t tx_abrt = I2C2->IC_TX_ABRT_SOURCE;
    if (tx_abrt & TX_ABRT_7B_ADDR_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C2->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_7B_ADDRESS_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C2->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS1_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C2->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS2_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C2->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_TX_DATA_NOACK) {
      // Clearing interrupt by reading the respective bit
      I2C2->IC_DATA_CMD_b.STOP = 0x1;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_NOACK) {
      // Clearing interrupt by reading the respective bit
      clear = I2C2->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_READ) {
      // Clearing interrupt by reading the respective bit
      clear = I2C2->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_10B_READ_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_DISABLED) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_TX_TX_FLUSH_CNT) {
    }
    if (tx_abrt & SL_I2C_ABORT_USER_ABORT) {
    }
    if (tx_abrt & SL_I2C_ABORT_SDA_STUCK_AT_LOW) {
      I2C2->IC_ENABLE_b.SDA_STUCK_RECOVERY_ENABLE = 0x1;
    }
    // Clearing all interrupts
    clear = I2C2->IC_CLR_INTR;
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(I2C2, SL_I2C_EVENT_TRANSMIT_EMPTY);
  }
  if (status & (SL_I2C_EVENT_SCL_STUCK_AT_LOW)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C2->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_MST_ON_HOLD)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C2->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_START_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C2->IC_CLR_START_DET_b.CLR_START_DET;
    return;
  }
  if (status & (SL_I2C_EVENT_STOP_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear              = I2C2->IC_CLR_STOP_DET_b.CLR_STOP_DET;
    uint32_t maskReg   = 0;
    maskReg            = I2C2->IC_INTR_MASK;
    I2C0->IC_INTR_MASK = (maskReg & (~SL_I2C_EVENT_RECEIVE_FULL));
    return;
  }
  if (status & (SL_I2C_EVENT_ACTIVITY_ON_BUS)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C2->IC_CLR_ACTIVITY_b.CLR_ACTIVITY;
    return;
  }
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    handle_leader_transmit_irq();
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    handle_leader_receive_irq();
  }
  if (status & (SL_I2C_EVENT_RECEIVE_UNDER)) {
    clear = I2C2->IC_CLR_RX_UNDER_b.CLR_RX_UNDER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_OVER)) {
    clear = I2C1->IC_CLR_RX_OVER_b.CLR_RX_OVER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_DONE)) {
    sl_si91x_i2c_clear_interrupts(I2C2, SL_I2C_EVENT_RECEIVE_DONE);
    return;
  }
  if (status & (SL_I2C_EVENT_GENERAL_CALL)) {
    sl_si91x_i2c_clear_interrupts(I2C2, SL_I2C_EVENT_GENERAL_CALL);
    return;
  }
  if (status & (SL_I2C_EVENT_RESTART_DET)) {
    sl_si91x_i2c_clear_interrupts(I2C2, SL_I2C_EVENT_RESTART_DET);
    return;
  }
  // to avoid unused variable warning
  (void)clear;
}
