# LightSoakV1 Firmware
This is the firmware for the LightSoakV1 project, developed as a part of my Master's thesis. It's intended to run on LightSoakV1 hardware. See https://github.com/mrmp17/LightSoakFW-sdkfhdksf for hardware design files. For Python data logging software, see https://github.com/mrmp17/LightSoakFW-Python.

## Light soaking measurements
Perovskite solar cells exhibit a phenomenon called light soaking, where the cell's forward voltage increases when exposed to light for some time. The relaxation of which is challenging to measure, as to be able to measure forward voltage, the cell needs to be illuminated, which in turn changes the relaxation curve.

LightSoakV1 project implements a flashmeasure measurement technique, where the cell is illuminated with a short pulse of light (1-10ms), during which forward voltage is measured. Hardware is specifically designed to accommodate this requirement.


## Hardware description

LightSoakV1 hardware is a specialized data acquisition device, designed specifically for measuring light soaking properties of perovskite solar cell samples. It consists of:
- Six identical daq channels implementing:
  - High input impedance voltage measurement (4-wire connection) with 1.5V range and 100kHz sample rate
  - Voltage forcing circuitry, allowing measurement of IV curves
  - Multi-range current measurement with 5mA range and low-leakage disconnect switch, allowing real open-circuit measurements.
- Illumination board, a high power high bandwidth LED driver
  - Up to 48V 1.5A
  - Zero ripple (linear regulation)
  - Ultra-fast rise and fal times (20us to within 0.5% of steady state)
  - Temperature compensated for constant light output
- DUT temperature control with TEC-1091 module (software implemented only in Python data logging software)

For specs, see https://github.com/mrmp17/LightSoakFW-sdkfhdksf.

## Command line interface

This firmware implements a command line interface over serial port at 230400 default baud rate at the device's USB-C port. This enables an easy-to-use standalone usage of the device for initial measurements and preparation of a more complex sequence, for which Python data logging software is used. The CLI provides a consistent interface for both standalone and Python usage. All data to and from the device is in human-readable format, making it easy to debug and develop new features.

## Quick start quide
Connect the hardware to your computer using a USB-C cable. If using without external power connected, make sure *VBUS-PWR* jumper is connected. The device will be powered from USB but analog performance may be degraded.

Make sure the device is detected as a CP2104 USB-UART converter. Drivers are not needed for MacOS and Linux but may be required for some Windows installs. See https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers in case of issues.

Connect to the device using a serial terminal of your choice. The terminal should support VT100 escape codes for proper display of the CLI. *Screen* is recomended on MacOS and Linux. Default baud rate is 230400.

Once connected, type *reboot* to reboot the device. While typing, text should appear in the terminal. Once rebooted, a welcome message with some basic instrutions will appear. The device is now ready to use.

### Status LEDs
The device has two status LEDs:
- Green LED lights up when the device is booted and ready.
- Blue LED is illuminated when ADC is sampling.

### Description of CLI commands
List of all commands can be printed via CLI by issuing a *help* command. Help for each command can be accessed by appending *-h* parameter to the command. This is where the parameters of each command are described.

- ***reboot*** - Reboots the device.
- ***ready?*** - Echos *READY*
- ***setbaud*** - Changes the buad rate. Default is 230400. This command is not persistent and will be reset on reboot. Maximum supported baud is 2000000.
- ***gettimestamp*** - Returns the microsecond timestamp counter.
- ***resettimestamp*** - Resets the microsecond timestamp counter to zero. This counter is used for timestamping measurements.

- ***getvolt*** - Measures the voltage on specified channel/s. Measurement is the average of a certain number of samples. See *getnumavg* and *setnumavg*.
- ***getcurr*** - Measures the current on specified channel/s. Measurement is the average of a certain number of samples. See *getnumavg* and *setnumavg*. Does not change any current measuring parameters. By default, the measurement will be zero, as current measurement circuitry is disconnected.
- ***getivpoint*** - Enables current measurement circuitry and measures current at a specified voltage. This measurement can take a while, depending on various conditions. See *setdutsettle* and *getdutsettle* to specify the settling time of DUT.
- ***getivchar*** - Measures an IV curve on specified channel. Only one channel at the same time is supported. This measurement can take a while, depending on various conditions. It consists of multiple *getivpoint* measurements.
- ***measuredump*** - Dumps a certain number of samples (at full 100kHz sample rate) for specified channel/s. A maximum number of samples is 2000 (20ms). Voltage, current or both signals can be dumped, as both are sampled concurrently. The transfer of data can take a while, depending on the number of samples and the baud rate.
- ***flashmeasure*** - Performs a flashmeasure measurement on specified channel/s. This measurement consists of a short pulse of light, during which forward voltage is measured. By defaul, voltage is measured as an average of a certain number of samples at a certain time during the flash. *-DUMP* parameter can be used to dump the voltage samples of the whole flashmeasure measurement.
- ***getnoise*** - Evaluates the noise on input channels as mV RMS noise and SNR.

- ***setledcurr*** - Sets LED current. This is temperature compensated to a reference temperature of 25C. Actual led current might differ due to this, but the light output will be constant for a given current at any LED temperature.
- ***setledillum*** - Sets illumination in unit of Sun. Temperature compensated. Calibration should be done with *calibillum* as a point of LED current and illumination measured by external equipment. This should be configured for every test and is not persistent across reboots.
- ***calibillum*** - Calibrates the relation between LED current and illumination. Relation is assumed to be linear. Calibrate at the illumination that will be used during the test for best accuracy. Not persistent across reboots.


- ***enablecurrent*** - Connects the current measurement circuitry on a specific channel/s.
- ***disablecurrent*** - Disconnects the current measurement circuitry on a specific channel/s.
- ***setshunt*** - Manually sets shunt range on a specific channel/s. Defaults to 1000X. Before enabling current circuitry, always start at 1X range to avoid damage. Recomended ranges for currents are:
  - 1X: Above 0.38mA
  - 10X: Above 38uA
  - 100X: Above 3.8uA
  - 1000X: Below 3.8uA
- ***setforcevolt*** - Manually sets voltage to be forced on a specific channel/s. Keep in mind the actual voltage on DUT deffers by the voltage drop on the shunt resistor. Always measure the actual voltage on DUT with *getvolt*.
- ***autorange*** - Manually trigger shunt autoranging on all channels.

- ***ENDSEQUENCE*** - This command should be called by Python data logging code at the end of the sequence.


### Command scheduling
Most commands can be scheduled to execute at a certain time by appending *-sched ###* parameter to the command, where ### is the time in microseconds (referenced to the internal microsecond timestamp). Not all commands can be scheduled - this is indicated in the command help in CLI. 
Scheduling can be done manually through CLI but is intended for test sequence programming in Python data logging software.

## Debug UART interface
The device has a second UART interface exposed on the STLINK debug connector. It provides some usefull debug information like warnings, errors, crash reports, time it takes for measurements to complete, measurement details, etc. To access, connect a STLINK with a 14-pin cable and open the STLINK's virtual serial port with 230400 baud rate.

## A short description of code architecture
### File structure
Code is written in C. Main source files are located in */Core/Src* and */Core/Inc*. The rest of the files are generated by STM32CubeMX. The project is based on STM32Cube HAL (and partly LL) library. IDE used is CLion by JetBrains with the native *Embedded Development Support* plugin. 

### Functional modules
Code is divided into functional modules, each wih its *.c* and *.h* file. All functions and variables of a certain module have a specific prefix, making it easy to identify where they come from. If a variable is considered private, it has a *prv_* prefix. If considered global, the prefix is *g_*. Due to the nature of embedded projects, modules include other modules.

The only external code used is *lwshell* CLI library, see https://github.com/MaJerle/lwshell/tree/develop/lwshell. It's included as source files and is slightly modified.

Hardware specific parameters are specified as macros in *.h* files of individual modules.

### Execution timing
This application is writen as bare-metal, without the use of a RTOS. For simplicity and hardware limitations in handling large amounts of sampling data, measurement functions are implemented as blocking throughout the measurement and data transfer process.

CLI commands that are not scheduled, call the measurement functions directly. Scheduled commands are executed by a simple scheduler, run in the main infinite loop in *main.c*. In this loop, some other periodic tasks are executed, such as passing input characters to CLI library as well as some periodic housekeeping tasks.

Main UART communication is implemented as a background interrupt-driven process that works with a RX and a TX buffer. To send the data, writer function fills the data into a TX queue to be sent out by the interrupt-driven process - thus the function is non-blocking. However, when transfering amounts of data larger than the TX buffer, measurement functions wait in blocking mode for the space in the buffer to free up.
Debug UART interface supports only transmitting which is done via DMA and consumes very little CPU time.

ADCs are triggered by a timer to run at a constant sample rate of 100kHz. For each trigger, all six channels of voltage and current (each with its dedicated ADC) are sampled and the raw readings stored by DMA into a global buffer. A microsecond timestamp can be calculated for each individual sample.

All timing of measurement functions is based on a 32-bit timer configured to tick at 1us. A 64-bit timestamp, to which measurements are referenced, is generated by tracking timer overflows. **Warning:** behaviour of overflow at 584942 years after boot is **undefined** and **not tested!**
