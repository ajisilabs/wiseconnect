# Wi-Fi - Wlan RF Test

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)
  - [Transmit Spectrum Example](#transmit-spectrum-example)

## Purpose/Scope

This application demonstrates how to configure the SiWx91x to transmit Wi-Fi packets at varying data rates and power levels. The application can be used to assist with regulatory certification testing.

## Prerequisites/Setup Requirements

To use this application, the following hardware, software and project setup is required.

### Hardware Requirements

- Windows PC.
- Spectrum Analyzer and RF connector
- Signal Generator and RF connectors
- SiWx91x Wi-Fi Evaluation Kit
- **SoC Mode**:
  - Standalone
    - BRD4002A Wireless pro kit mainboard [SI-MB4002A]
    - Radio Boards
    - BRD4338A [SiWx917-RB4338A]
    - BRD4339B [SiWx917-RB4339B]
    - BRD4340A [SiWx917-RB4340A]
  - Kits
  - SiWx917 Pro Kit [Si917-PK6031A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-pro-kit?tab=overview)
  - SiWx917 Pro Kit [Si917-PK6032A]

- **NCP Mode**:
  - Standalone
    - BRD4002A Wireless pro kit mainboard [SI-MB4002A]
    - EFR32xG24 Wireless 2.4 GHz +10 dBm Radio Board [xG24-RB4186C](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)
    - NCP EFR Expansion Kit with NCP Radio board (BRD4346A + BRD8045A) [SiWx917-EB4346A]
  - Kits
  - EFR32xG24 Pro Kit +10 dBm [xG24-PK6009A](https://www.silabs.com/development-tools/wireless/efr32xg24-pro-kit-10-dbm?tab=overview)

### Software Requirements

- Simplicity Studio IDE

### Setup Diagram

  ![Figure: Setup Diagram for SoC and NCP mode Transmit Test Example](resources/readme/image217soc_ncp.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

The application can be configured to suit user requirements and development environment. Read through the following sections and make any changes needed.

- Configure the following parameters in **app.c** to test rf_test app as per requirements

```c
  sl_wifi_data_rate_t rate = SL_WIFI_DATA_RATE_6;
  sl_si91x_request_tx_test_info_t tx_test_info = {
    .enable      = 1,          // Enable/disable tx test mode
    .power       = 4,         // Tx RF power in the range [2:18] dBm
    .rate        = rate,      // WLAN data rate of 6Mbps
    .length      = 30,        // Tx packet length in the range [24:1500] bytes in burst mode, 
    .mode        = 0,         // Selects burst mode or continuous mode
    .channel     = 1,         // Channel number in 2.4 or 5 GHz
    // Other configurable parameters
  }
```

    The different values of mode:
      0 - Burst Mode
      1 - Continuous Mode
      2 - Continuous wave Mode (non modulation) in DC mode
      3 - Continuous wave Mode (non modulation) in single tone mode (center frequency -2.5MHz)
      4 - Continuous wave Mode (non modulation) in single tone mode (center frequency +5MHz)

  Receive stats pre-processor macro:

    #define RECEIVE_STATS           0 // Enable/Disable receive stats testing
    #define MAX_RECEIVE_STATS_COUNT 5 // Number of iterations for which the receive stats would be displayed

**NOTE**:
    Receive stats testing should be done in a controlled environment (RF shield box or chamber).

- In the Project explorer pane, expand as follows **wiseconnect3_sdk_xxx** > **components** > **si91x** > **inc** folder and open **sl_wifi_device.h** file. Change the region_code to the respective region in sl_wifi_transmit_test_configuration.

        region_code - US, EU, JP, WORLD_DOMAIN, KR

        Supported Channels in the specific regions
        US           -     1-11
        EUROPE       -     1-13
        JAPAN        -     1-14
        WORLD_DOMAIN -     1-14
        KOREA        -     1-13

    Note: Currently SG region is not supported.

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application

  - Application prints

  ![Figure: Application prints ](resources/readme/application_prints_soc_ncp.png)

Follow the steps below for successful execution of the application:

### Transmit Spectrum Example

```c
  sl_wifi_data_rate_t rate = SL_WIFI_DATA_RATE_6;
  sl_si91x_request_tx_test_info_t tx_test_info = {
    .enable      = 1,
    .power       = 127,
    .rate        = rate,
    .length      = 100,
    .mode        = 1,     // Continuous Mode
    .channel     = 6,
    // Other configurable parameters
  }                  
```

The below result is for PER Mode with Channel '6' with 6 Mbps data rate and max 127 power index, in Continuous mode, OFDM modulation technique.

![Figure: Spectrum Analyzer speed 6Mbps](resources/readme/continuous_mode_spectrum_analyser.png)

    NOTE:
    A) Before starting CW mode, it is required to start Continuous mode with power and channel values which is intended to be used in CW mode as follows:
      - Start Continuous mode with intended power value and channel value; pass any valid values for rate and length
      - Stop Continuous mode
      - Start CW mode.
    B) Receive stats testing should be done in a controlled environment (RF shield box or chamber).

### RF Test with wifi 6

- Configure the following parameters in **app.c** to perform rf_test with wifi 6 as per requirements

```c
  sl_wifi_data_rate_t rate = SL_WIFI_DATA_RATE_MCS0;
  sl_si91x_request_tx_test_info_t tx_test_info = {
    .enable      = 1,
    .power       = 127,
    .rate        = rate,
    .length      = 100,
    .mode        = 1,     // Continuous Mode
    .channel     = 1,
    .aggr_enable = 0,
  #ifdef SLI_SI917
    .enable_11ax            = 1, // 11AX_ENABLE 0-disable, 1-enable
    ....
    // Other configurable parameters
    // The description of the configurable parameters can be found in sl_si91x_request_tx_test_info_t structure
  #endif
  }

  osDelay(100000); //add delay as per the requirement
```

The below is the result for PER Mode with Channel '1' with MCS index 0 transfer rate, in continuous mode.

![Figure: Spectrum Analyzer RF Test with wifi6](resources/readme/continuous_mode_spectrum_analyser_rf_test_wifi6.png)
