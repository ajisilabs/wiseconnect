# SL ULP CALENDAR

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Overview](#overview)
- [About Example Code](#about-example-code)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This example demonstrates the clock configuration, one millisecond trigger, one second trigger, alarm configuration and time conversion in this example.
- Before enabling any trigger, it is recommended to calibrate the clock.

## Overview

- Calendar calculates milliseconds, seconds, minutes, hours, days, months and years up to 4 centuries.
- It also calculates days of week and takes care of number of days in month as well as leap year.
- It can also configure alarm for desired time as a one shot trigger.
- It can generate triggers on one second and one millisecond time interval.
- It uses APB for read and write operations in real time.
- RC clock and RO clock are configurable, and it can also be calibrated using the APIs.

## About Example Code

- This example demonstrates clock configuration, set calendar date-time, calendar get date-time, set alarm date-time, get alarm date-time, alarm trigger, one millisecond trigger, one second trigger and clock calibration.
- To configure the calendar clock, select the clock from UC. \ref sl_si91x_calendar_config is used to set the calendar clock.
- A structure is created which contains default values for calendar date-time. It is created using \ref sl_si91x_calendar_build_datetime_struct, After entering all the parameters, it returns a structure filled with all the parameters.
- Calendar date-time is configured using \ref sl_si91x_calendar_set_date_time API. It configures the date time and the calendar blocks starts counting from that time.
- To verify if the desired time is set, \ref sl_si91x_calendar_get_date_time API is used, It returns a structure which has current date-time.

- If **ALARM_EXAMPLE** macro is enabled:

  - A date-time structure is created using \ref sl_si91x_calendar_build_datetime_struct for configuring the Alarm.
  - Alarm is configured using \ref sl_si91x_calendar_set_alarm API.
  - Now callback is registered to perform action at the time of trigger using \ref sl_si91x_calendar_register_alarm_trigger_callback API.
  - To verify if the desired alarm is set, \ref sl_si91x_calendar_get_alarm API is used, It returns a structure which has configured alarm date-time.
  - At the time of trigger, it prints current date-time on the console.

- If **CLOCK_CALIBRATION** macro is enabled:
  
  - This clock calibration applies to RO and RC Clock only. For RO clock \ref sl_si91x_calendar_roclk_calibration should be used and for RC clock \ref sl_si91x_calendar_rcclk_calibration should be used.
  - It is recommended to calibrate clock before activating any trigger after every power cycle.
  - Initialization of clock is performed using \ref sl_si91x_calendar_calibration_init API.
  - To select the clock in UC, follow the procedure mentioned in "Configuration and Steps for Execution" section.
  - According to the clock configured in UC, either RO or RC, use respective API to configure the clock. In this example RC clock is selected so \ref sl_si91x_calendar_roclk_calibration API is used.
  - This API expects \ref clock_calibration_config_t structure. For rc_trigger_time, \ref RC_CLOCK_CALIBRATION_ENUM enum can be used, and for ro_trigger_time \ref RO_CLOCK_CALIBRATION_ENUM enum can be used.
  - After calibration \ref sl_si91x_calendar_rtc_start is called to start the calendar clock.

- If **SEC_INTR** macro is enabled:

  - Callback is registered for one second trigger using \ref sl_si91x_calendar_register_sec_trigger_callback API.
  - At every one second, the console prints `one_sec_callback`.

- If **MILLI_SEC_INTR** macro is enabled:

  - Callback is registered for one millisecond trigger using \ref sl_si91x_calendar_register_msec_trigger_callback API.
  - At every one second, the console prints `on_msec_callback triggered 1000 times`.
  - It accumulates the 1000 trigger at one millisecond time frame and prints at only one second.

- If **TIME_CONVERSION** macro is enabled:
  - This converts NTP time to Unix Time and vice versa.
  - In this example, \ref sl_si91x_calendar_convert_unix_time_to_ntp_time API is called to convert unix time to ntp time, it expects unix time as parameter.
  - It updates the variable with ntp time which is passed as parameter.
  - After conversion, ntp time and unix time are printed on the console.
  - Now \ref sl_si91x_calendar_convert_ntp_time_to_unix_time API is used to convert ntp time to unix time, it expects ntp time as parameter.
  - It updates the variable with unix time which is passed as parameter.
  - After conversion, ntp time and unix time are printed on the console.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Setup Diagram

 ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

- Configure UC from the slcp component.

   ![Figure: Introduction](resources/uc_screen/calendar_uc_screen.png)

- Open **sl_si91x_calendar.slcp** project file select **software component** tab and search for **Calendar** in search bar.
- Using configuration wizard one can configure the Calendar clock, i.e., RO, RC and XTAL.
- Configuration file is generated in **config folder**, if not changed then the code will run on default UC values.

- Set any of the macro in calender_example.h whose functionality needs to be tested.

- To enable alarm trigger callback set the ALARM_EXAMPLE macro.

  ```C
   #define ALARM_EXAMPLE       1 ///< To enable alarm trigger \n
  ```

- To enable clock calibration set the CLOCK_CALIBRATION macro.

   ```C
    #define CLOCK_CALIBRATION  1 ///< To enable clock calibration \n
   ```
- To enable second trigger callback set the SEC_INTR macro.

   ```C
    #define SEC_INTR           1 ///< To enable one second trigger \n
   ```

- To enable millisecond trigger callback set the MILLI_SEC_INTR macro.

   ```C
    #define MILLI_SEC_INTR     1 ///< To enable one millisecond trigger \n
   ```

- To enable time conversion set the TIME_CONVERSION macro.

   ```C
    #define TIME_CONVERSION    1 ///< To enable time conversion trigger \n
   ```

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the SL ULP CALENDAR example in Studio.
- Flash, run and debug the application.
- After successful program execution the prints in serial console looks as shown below.

  >![output](resources/readme/output_ulp_calendar.png)

> **Note:**
>- This application is executed from RAM.
>- In this application while changing the MCU mode from PS4 to PS2, M4 flash will be turned off.
>- The debug feature of Simplicity Studio will not work after M4 flash is turned off.
>- To Erase the chip follow the below procedure
>- Press ISP and RESET button at same time and then release, now perform Chip erase through commander.  

