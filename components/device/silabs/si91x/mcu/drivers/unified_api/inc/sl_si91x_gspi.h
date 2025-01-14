/***************************************************************************/ /**
 * @file sl_si91x_gspi.h
 * @brief GSPI API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_SI91X_GSPI_H
#define SL_SI91X_GSPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "GSPI.h"

/***************************************************************************/ /**
 * @addtogroup GSPI Generic SPI
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 * 
 ******************************************************************************/
// -----------------------------------------------------------------------------
// Data Types

typedef ARM_SPI_SignalEvent_t sl_gspi_signal_event_t; ///< Renamed signal event structure
typedef ARM_SPI_STATUS sl_gspi_status_t;              ///< Renamed status structure
typedef ARM_DRIVER_SPI sl_gspi_driver_t;              ///< Renamed GSPI driver structure
typedef const void *sl_gspi_handle_t;                 ///< Created GSPI handle type

/// @brief Enumeration for different GSPI callback events
typedef enum {
  SL_GSPI_TRANSFER_COMPLETE = ARM_SPI_EVENT_TRANSFER_COMPLETE, ///< Transfer complete event
  SL_GSPI_DATA_LOST         = ARM_SPI_EVENT_DATA_LOST,         ///< Data lost event
  SL_GSPI_MODE_FAULT        = ARM_SPI_EVENT_MODE_FAULT,        ///< Mode fault event
} gspi_event_typedef_t;

/// @brief Enumeration for GSPI power state
typedef enum {
  SL_GSPI_POWER_OFF  = ARM_POWER_OFF,  ///< Power mode OFF
  SL_GSPI_LOW_POWER  = ARM_POWER_LOW,  ///< Low power mode
  SL_GSPI_FULL_POWER = ARM_POWER_FULL, ///< Full power mode
  SL_GSPI_POWER_MODE_LAST,             ///< Last member of enum for validation
} sl_gspi_power_state_t;

/// @brief Enumeration for GSPI clock modes
typedef enum {
  SL_GSPI_MODE_0 = ARM_SPI_CPOL0_CPHA0, ///< Mode 0 CPOL0_CPHA0
  SL_GSPI_MODE_3 = ARM_SPI_CPOL1_CPHA1, ///< Mode 1 CPOL1_CPHA1
} clock_mode_typedef_t;

/// @brief Enumeration for GSPI master modes
typedef enum {
  SL_GSPI_MASTER_INACTIVE = ARM_SPI_MODE_INACTIVE, ///< Master mode inactive
  SL_GSPI_MASTER_ACTIVE   = ARM_SPI_MODE_MASTER,   ///< Master mode active
  SL_GSPI_MASTER_MODE_LAST,                        ///< Last member of enum for validation
} master_mode_typedef_t;

/// @brief Enumeration for GSPI secondary select mode
typedef enum {
  SL_GSPI_MASTER_UNUSED    = ARM_SPI_SS_MASTER_UNUSED,    ///< Master unused mode
  SL_GSPI_MASTER_SW        = ARM_SPI_SS_MASTER_SW,        ///< Master software mode
  SL_GSPI_MASTER_HW_OUTPUT = ARM_SPI_SS_MASTER_HW_OUTPUT, ///< Master hardware output mode
  SL_GSPI_SLAVE_SELECT_MODE_LAST,                         ///< Last member of enum for validation
} slave_select_mode_typedef_t;

/// @brief Enumeration for GSPI Master instance, enum for one member, future provision is provided.
typedef enum {
  SL_GSPI_MASTER,             ///< GSPI Master Instance
  SL_GSPI_INSTANCE_LAST_ENUM, ///< Last member of enum for validation
} sl_gspi_instance_t;

/// @brief Enumeration for GSPI secondary numbers.
typedef enum {
  GSPI_SLAVE_0,         ///< Slave No. 1
  GSPI_SLAVE_1,         ///< Slave No. 2
  GSPI_SLAVE_2,         ///< Slave No. 2
  GSPI_SLAVE_LAST_ENUM, ///< Last member of enum for validation
} sl_gspi_slave_number_t;

/// @brief Structure to hold the parameters for the configuration of GSPI peripheral
typedef struct {
  boolean_t swap_read;        ///< true to enable and false to disable swap read
  boolean_t swap_write;       ///< true to enable and false to disable swap write
  uint8_t bit_width;          ///< Bit width either 8 bit or 16 bit
  uint32_t clock_mode;        ///< Mode 0 or Mode 3 of GSPI
  uint32_t slave_select_mode; ///< Secondary select mode either software of hardware output
  uint32_t bitrate;           ///< Bitrate for setting the clock division factor
} sl_gspi_control_config_t;

/// @brief Structure to hold the clock configuration parameters
typedef struct {
  uint8_t soc_pll_mm_count_value;      ///< SoC PLL count value
  uint16_t intf_pll_500_control_value; ///< Int PLL control value
  uint32_t intf_pll_clock;             ///< Intf PLL clock frequency
  uint32_t intf_pll_reference_clock;   ///< Intf PLL reference clock frequency
  uint32_t soc_pll_clock;              ///< SoC PLL clock frequency
  uint32_t soc_pll_reference_clock;    ///< SoC PLL reference clock frequency
  uint16_t division_factor;            ///< Clock Division Factor
} sl_gspi_clock_config_t;

/// @brief Structure to hold the versions number of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_gspi_version_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/
/**
 * Set the clock for the GSPI peripheral, Configures the PLL clock and
 * SOC clock with the value set by the user in the clock configuration structure.
 * 
 * @param clock_configuration Pointer to the clock configuration structure \ref sl_gspi_clock_config_t
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_FAIL (0x0001) - The function has failed 
 *         - \ref SL_STATUS_NOT_INITIALIZED (0x0011) - Clock is not initialized
 ******************************************************************************/
sl_status_t sl_si91x_gspi_configure_clock(sl_gspi_clock_config_t *clock_configuration);

/***************************************************************************/
/**
 * Initialize the GSPI. If the DMA is enabled, it also initializes the DMA.
 * Pass the address of the pointer for storing the GSPI master handle,
 * which can be used in the future for other function calls.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 * 
 * @param[in] instance GSPI Instance. \ref sl_gspi_instance_t
 * @param[in] gspi_handle Double Pointer to the GSPI driver handle \ref sl_gspi_handle_t
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_gspi_init(sl_gspi_instance_t instance, sl_gspi_handle_t *gspi_handle);

/***************************************************************************/
/**
 * Uninitialize the GSPI. If the DMA is enabled, it also uninitializes the DMA.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_init 
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle \ref sl_gspi_handle_t
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 ******************************************************************************/
sl_status_t sl_si91x_gspi_deinit(sl_gspi_handle_t gspi_handle);

/***************************************************************************/
/**
 * Control and configure the GSPI.
 * - The configurations are listed below:
 *   - swap_read (enable/disable)
 *   - swap_write (enable/disable)
 *   - bit_width (8_bit/16_bit)
 *   - clock_mode (mode0/mode3)
 *   - slave_select_mode (hw_output/sw)
 *   - bitrate 
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle \ref sl_gspi_handle_t
 * @param[in] control_configuration pointer to the configuration structure \ref sl_gspi_control_config_t
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_FAIL (0x0001) - The function has failed 
 *         - \ref SL_STATUS_NOT_SUPPORTED (0x000F) - Parameter is not supported 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy 
 *         - \ref SL_STATUS_INVALID_MODE (0x0024) - Secondary select Mode is invalid 
 *         - \ref SL_STATUS_INVALID_TYPE (0x0026) - SPI frame format is not valid 
 *         - \ref SL_STATUS_INVALID_RANGE (0x0028) - Data bits (frame length) are not in range
 ******************************************************************************/
sl_status_t sl_si91x_gspi_set_configuration(sl_gspi_handle_t gspi_handle,
                                            sl_gspi_control_config_t *control_configuration);

/***************************************************************************/
/**
 * Receive data from the secondary device.
 * If DMA is enabled, it configures the DMA channel and required parameters.
 * When the received data is equal to data_length passed in this function,
 * a callback event is generated which can be registered using \ref sl_si91x_gspi_register_event_callback
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 *      - \ref sl_si91x_gspi_set_configuration 
 *      - \ref sl_si91x_gspi_set_slave_number
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle \ref sl_gspi_handle_t
 * @param[in] data pointer to the variable that will store the received data
 * @param[in] data_length (uint32_t) number of data items to receive
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_FAIL (0x0001) - The function has failed 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_gspi_receive_data(sl_gspi_handle_t gspi_handle, void *data, uint32_t data_length);

/***************************************************************************/
/**
 * Send data to the secondary device.
 * If DMA is enabled, it configures the DMA channel and required parameters.
 * When the send data is equal to data_length passed in this function,
 * a callback event is generated which can be registered using \ref sl_si91x_gspi_register_event_callback
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 *      - \ref sl_si91x_gspi_set_configuration 
 *      - \ref sl_si91x_gspi_set_slave_number
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @param[in] data const pointer to the variable that has data which needs to be sent
 * @param[in] data_length (uint32_t) number of data items to send
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid  
 *         - \ref SL_STATUS_FAIL (0x0001) - The function has failed 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_gspi_send_data(sl_gspi_handle_t gspi_handle, const void *data, uint32_t data_length);

/***************************************************************************/
/**
 * Send and receive data to the secondary device simultaneously.
 * If DMA is enabled, it configures the DMA channel and required parameters.
 * When the received data and send data is equal to data_length passed in this function,
 * a callback event is generated which can be registered using \ref sl_si91x_gspi_register_event_callback
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 *      - \ref sl_si91x_gspi_set_configuration 
 *      - \ref sl_si91x_gspi_set_slave_number
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @param[in] data_out const pointer to the variable that has data which needs to be sent
 * @param[in] data pointer to the variable that will store the received data
 * @param[in] data_length (uint32_t) number of data items to receive
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_FAIL (0x0001) - The function has failed 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_gspi_transfer_data(sl_gspi_handle_t gspi_handle,
                                        const void *data_out,
                                        void *data_in,
                                        uint32_t data_length);

/***************************************************************************/
/**
 * Set the main state, i.e., activate/de-activate the main.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @param[in] value (boolean_t) Enable or Disable
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy 
 *         - \ref SL_STATUS_INVALID_MODE (0x0024) - Mode is invalid (Fails to activate master) 
 ******************************************************************************/
sl_status_t sl_si91x_gspi_set_master_state(sl_gspi_handle_t gspi_handle, boolean_t value);

/***************************************************************************/
/**
 * Register the user callback function.
 * At the time of events, the function passed in the parameter is called with the respective
 * event as the parameter.
 * Before calling this function again, it is mandatory to call the \ref sl_si91x_gspi_unregister_event_callback
 * function to unregister the callback, otherwise it returns SL_STATUS_BUSY error code.
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @param[in] callback_event Pointer to the function that needs to be called at the time of interrupt
 *            ( \ref sl_gspi_signal_event_t)
 * @return status
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_gspi_register_event_callback(sl_gspi_handle_t gspi_handle, sl_gspi_signal_event_t callback_event);

/***************************************************************************/
/**
 * Un-register the user callback function.
 * It is mandatory to call this function before registering the callback again.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_register_event_callback 
 * 
 * @param[in] none
 * @return none
 ******************************************************************************/
void sl_si91x_gspi_unregister_event_callback(void);

/***************************************************************************/ /**
 * Get the GSPI version.
 * It returns the API version of GSPI.
 * 
 * @param[in] none
 * @return (sl_gspi_version_t) type structure
 ******************************************************************************/
sl_gspi_version_t sl_si91x_gspi_get_version(void);

/***************************************************************************/ /**
 * Get the transfer status GSPI.
 * It returns the \ref sl_gspi_status_t type structure. The members are:
 * - Busy
 * - Data Lost
 * - Mode Fault
 * It is generally used to poll the busy status of GSPI Master.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 *      - \ref sl_si91x_gspi_set_configuration 
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @return sl_gspi_status_t type structure
 ******************************************************************************/
sl_gspi_status_t sl_si91x_gspi_get_status(sl_gspi_handle_t gspi_handle);

/***************************************************************************/ /**
 * Get data receive count of the GSPI.
 * If a receive operation is started, this function can be used to get number 
 * of data bytes received.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 *      - \ref sl_si91x_gspi_set_configuration 
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @return uint32_t value of the RX data count
 ******************************************************************************/
uint32_t sl_si91x_gspi_get_rx_data_count(sl_gspi_handle_t gspi_handle);

/***************************************************************************/
/**
 * Get the transmit data count of GSPI.
 * If a send operation is started, this function can be used to get number 
 * of data bytes send.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_gspi_configure_clock 
 *      - \ref sl_si91x_gspi_init 
 *      - \ref sl_si91x_gspi_set_configuration 
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @return uint32_t value of the tx data count
 ******************************************************************************/
uint32_t sl_si91x_gspi_get_tx_data_count(void);

/***************************************************************************/
/**
 * Fetch the clock division factor.
 * 
 * @param[in] gspi_handle Pointer to the GSPI driver handle ( \ref sl_gspi_handle_t)
 * @return factor (int32_t) The value of the clock division factor
 ******************************************************************************/
int32_t sl_si91x_gspi_get_clock_division_factor(sl_gspi_handle_t gspi_handle);

/***************************************************************************/
/**
 * Fetch the frame length, i.e., bit width.
 * The frame length ranges between 1 and 15.
 * 
 * @param[in] none
 * @return frame_length (uint32_t) The value of the frame length
 ******************************************************************************/
uint32_t sl_si91x_gspi_get_frame_length(void);

/***************************************************************************/
/**
 * Set the secondary number in multi-secondary operation.
 * For a single secondary also, this API needs to be called before transferring the
 * data
 * 
 * @param[in] number Secondary number ( \ref sl_gspi_slave_number_t)
 * @return none
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_si91x_gspi_set_slave_number(uint8_t number)
{
  sl_status_t status;
  if (number > GSPI_SLAVE_LAST_ENUM) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    RSI_GSPI_SetSlaveSelectNumber(number);
    status = SL_STATUS_OK;
  }
  return status;
}

/** @} (end addtogroup GSPI) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_GSPI_H */
