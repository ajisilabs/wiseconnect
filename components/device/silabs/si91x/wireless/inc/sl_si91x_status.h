/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#pragma once
typedef enum {
  RSI_ERROR_NONE                             = 0,
  RSI_ERROR_TIMEOUT                          = -1,
  RSI_ERROR_INVALID_PARAM                    = -2,
  RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE     = -3,
  RSI_ERROR_PKT_ALLOCATION_FAILURE           = -4,
  RSI_ERROR_COMMAND_NOT_SUPPORTED            = -5,
  RSI_ERROR_INSUFFICIENT_BUFFER              = -6,
  RSI_ERROR_IN_OS_OPERATION                  = -7,
  RSI_ERROR_INVALID_MEMORY                   = -8,
  RSI_ERROR_BOOTUP_OPTIONS_NOT_SAVED         = -9,
  RSI_ERROR_BOOTUP_OPTIONS_CHECKSUM_FAIL     = -10,
  RSI_ERROR_BOOTLOADER_VERSION_NOT_MATCHING  = -11,
  RSI_ERROR_WAITING_FOR_BOARD_READY          = -12,
  RSI_ERROR_INVALID_ADDRESS                  = -13,
  RSI_ERROR_VALID_FIRMWARE_NOT_PRESENT       = -14,
  RSI_ERROR_INVALID_OPTION                   = -15,
  RSI_ERROR_EXCEEDS_MAX_CALLBACKS            = -16,
  RSI_ERROR_SET_ITIMER                       = -17,
  RSI_ERROR_SIG_ACTION                       = -18,
  RSI_ERROR_NOT_IN_CONNECTED_STATE           = -19,
  RSI_ERROR_NOT_IN_IPCONFIG_STATE            = -20,
  RSI_ERROR_SPI_BUSY                         = -21,
  RSI_ERROR_SPI_FAIL                         = -22,
  RSI_ERROR_SPI_TIMEOUT                      = -23,
  RSI_ERROR_CARD_READY_TIMEOUT               = -24,
  RSI_ERROR_BOARD_READY_TIMEOUT              = -25,
  RSI_ERROR_INVALID_PACKET                   = -26,
  RSI_ERROR_FW_UPGRADE_TIMEOUT               = -27,
  RSI_ERROR_FW_LOAD_OR_UPGRADE_TIMEOUT       = -28,
  RSI_ERROR_GPIO_WAKEUP_TIMEOUT              = -29,
  RSI_ERROR_RESPONSE_TIMEOUT                 = -30,
  RSI_ERROR_BLE_DEV_BUF_FULL                 = -31,
  RSI_ERROR_NWK_CMD_IN_PROGRESS              = -32,
  RSI_ERROR_SOCKET_CMD_IN_PROGRESS           = -33,
  RSI_ERROR_WLAN_CMD_IN_PROGRESS             = -34,
  RSI_ERROR_COMMON_CMD_IN_PROGRESS           = -35,
  RSI_ERROR_ANT_DEV_BUF_FULL                 = -36,
  RSI_ERROR_BT_BLE_CMD_IN_PROGRESS           = -37,
  RSI_ERROR_ANT_CMD_IN_PROGRESS              = -38,
  RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS          = -39,
  RSI_ERROR_MEMORY_NOT_ALIGNED               = -40,
  RSI_ERROR_SEMAPHORE_CREATE_FAILED          = -41,
  RSI_ERROR_SEMAPHORE_DESTROY_FAILED         = -42,
  RSI_ERROR_IN_WLAN_CMD                      = -43,
  RSI_ERROR_RX_BUFFER_CHECK                  = -44,
  RSI_ERROR_PARAMETER_LENGTH_EXCEEDS_MAX_VAL = -45,
  RSI_ERROR_IN_COMMON_CMD                    = -46,
  RSI_ERROR_TX_BUFFER_FULL                   = -47,
  RSI_ERROR_SDIO_TIMEOUT                     = -48,
  RSI_ERROR_SDIO_WRITE_FAIL                  = -49,
  RSI_ERROR_INVALID_SET_CONFIG_FLAG          = -50,
  RSI_ERROR_BLE_DEV_BUF_IS_IN_PROGRESS       = -51,
  RSI_ERROR_SET_REGION_NOT_ENABLED           = -52,
  RSI_ERROR_BLE_INVALID_CALLBACK_CNT         = -53,
  RSI_ERROR_BLE_ADV_EXT_COMMAND_NOT_ALLOWED  = -54,
  RSI_ERROR_BLE_LEGACY_COMMAND_NOT_ALLOWED   = -55,
  RSI_ERROR_TRNG_DUPLICATE_ENTROPY           = -56,
  RSI_ERROR_INVALID_MAC_ADDRESS              = -58,
  RSI_ERROR_INVALID_QOS_PRIORITY             = -59,
  RSI_ERROR_INVALID_CHANNEL                  = -60,
  RSI_ERROR_INVALID_DATA_RATE                = -61,
  RSI_ERROR_INVALID_BTR_CONFIG               = -62
} si91x_status_t;
