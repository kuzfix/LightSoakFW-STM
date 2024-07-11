# LightSoakV1 Firmware
This is the firmware for the LightSoakV1 project, developed as a part of the original author's Master's thesis (original: https://github.com/mrmp17/LightSoakFW-STM). It's intended to run on LightSoakV1 hardware. See https://github.com/kuzfix/LightSoak-Hardware for hardware design files. For Python data logging software, see https://github.com/kuzfix/LightSoakFW-Python.

This fork has been moved to the STM32CubeIDE.

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

For specs, see https://github.com/kuzfix/LightSoak-Hardware.

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

- ***getvolt*** - Measures the voltage on specified channel/s [V]. Measurement is the average of a certain number of samples. See *getnumavg* and *setnumavg*.

- ***getcurr*** - Measures the current on specified channel/s [uA]. Measurement is the average of a certain number of samples. See *getnumavg* and *setnumavg*. Does not change any current measuring parameters. By default, the measurement will be zero, as current measurement circuitry is disconnected.

- ***setnumavg*** - Set number of measurements to average. Applies to *getvolt* and *getcurr*. Buffer size for measurement results is 12000, meaning max. value can be 12000 if only 1 channel is to be mesured, or 2000 if all 6 are to be measured - not tested!
Example: *setnumavg -n 20* Set number of measurements to average to 20.

- ***getivpoint*** - Enables current measurement circuitry and measures current at a specified voltage. This measurement can take a while, depending on various conditions. See *setdutsettle* and *getdutsettle* to specify the settling time of DUT.
- ***getivchar*** - Measures an IV curve on specified channel. Only one channel at the same time is supported. This measurement can take a while, depending on various conditions. It consists of multiple *getivpoint* measurements.
- ***measuredump*** - Dumps a certain number of samples (at full 100kHz sample rate) for specified channel/s. A maximum number of samples is 2000 (20ms). Voltage, current or both signals can be dumped, as both are sampled concurrently. The transfer of data can take a while, depending on the number of samples and the baud rate.
Parameters:
	- *-c* - channel (1-6 or 0 for all (default))
	- *-n* - number of samples
	- *-VOLT/-CURR/-IV* - measure either voltage or current or both
Example: *measuredump -c 0 -n 1000 -VOLT* will measure voltage on all channels for a time period of 10ms.

- ***flashmeasure*** - Performs a flashmeasure measurement on specified channel/s. This measurement consists of a short pulse of light, during which forward voltage is measured. By defaul, voltage is measured as an average of a certain number of samples at a certain time during the flash. *-DUMP* parameter can be used to dump the voltage samples of the whole flashmeasure measurement.
Example: *flashmeasure -illum 1.0 -t 100 -DUMP* will generate a 100us long pulse of light with 1 sun irradiance and return all voltage measurements during the duration of the pulse.
Example: *flashmeasure -illum 1.0 -t 100 -m 10 -n 4* will generate a 100us long pulse of light with 1 sun irradiance and start measuring voltages 10us after the start of the pulse. The result will be the average of 4 measurements.

- ***getnoise*** - Evaluates the noise on input channels (voltage current or both) as RMS and SNR ratio. Evaluated on maximum possible number of buffered samples (2000).

- ***setledcurr*** - Sets LED current. This is temperature compensated to a reference temperature of 25 C. Actual led current might differ due to this, but the light output will be constant for a given current at any LED temperature. (Max current is 1.5 A, allowing for temperature compensation even a bit less. Practical resolution is about 1% or 15 mA (compared to theoretical 1/4096 or 0.37 mA))
Warning: The system does NOT keep correcting LED current to compensate for temperature - it could interfere with timing of other commands. Instead periodically schedule the same command when there is enough time to do so.

- ***setledillum*** - Sets illumination in unit of Sun. Temperature compensated. Calibration should be done with *calibillum* as a point of LED current and illumination measured by external equipment. This should be configured for every test and is not persistent across reboots.
Warning: The system does NOT keep correcting LED current to compensate for temperature - it could interfere with timing of other commands. Instead periodically schedule the same command when there is enough time to do so.

- ***calibillum*** - Calibrates the relation between LED current and illumination. Relation is assumed to be linear. Calibrate at the illumination that will be used during the test for best accuracy. Not persistent across reboots. Parameters:
	- *-illum*: illumination, unit: [sun]
	- *-i*: current, unit: [A]
Example: *calibillum -illum 1.0 -i 1.4* means that at current of 1.4 A, an illumination of 1.0 suns is achieved.
To get the calibration parameters, use *setledcurr* to set the current, and measure the achieved illumination with external equipment.

- ***enablecurrent*** - Connects the current measurement circuitry on a specific channel/s.
Example 1: *enablecurrent* Enable current measurement circuitry on all channels.
Example 2: *enablecurrent -c 3* Enable current measurement circuitry on channel 3.

- ***disablecurrent*** - Disconnects the current measurement circuitry on a specific channel/s.
Example 1: *disablecurrent* Disable current measurement circuitry on all channels.
Example 2: *disablecurrent -c 3* Disable current measurement circuitry on channel 3.

- ***setshunt*** - Manually sets shunt range on a specific channel/s. Defaults to 1000X. Before enabling current circuitry, always start at 1X range to avoid damage. Recomended ranges for currents are:
  - 1X: Above 0.38mA
  - 10X: Above 38uA
  - 100X: Above 3.8uA
  - 1000X: Below 3.8uA
Example 1: *setshunt -1x*	Set 1X range on all channels
Example 2: *setshunt -c 1 -1000x* Set 1000X range on channel 1

- ***setforcevolt*** - Manually sets voltage to be forced on a specific channel/s. Keep in mind the actual voltage on DUT deffers by the voltage drop on the shunt resistor. Always measure the actual voltage on DUT with *getvolt*.
- ***autorange*** - Manually trigger shunt autoranging on all channels.

- ***ENDSEQUENCE*** - This command should be called by Python data logging code at the end of the sequence. Reboots the device one second after this command is invoked.


### Command scheduling
Most commands can be scheduled to execute at a certain time by appending *-sched ###* parameter to the command, where ### is the time in microseconds (referenced to the internal microsecond timestamp). Not all commands can be scheduled - this is indicated in the command help in CLI. 
Scheduling can be done manually through CLI but is intended for test sequence programming in Python data logging software.

#Limitations:#
- The minimum time separation between 2 commands in the schedule is about 5 ms. The limiting factor is the time required to output debug messages on the debug serial interface. This limitation can be reduced by changing the debug level in debug.h. Any lower time limitations need to be further tested (Example: Current measurement takes about 0.64ms and measurement results reporting apporoximately 0.6ms more. With a bit of overhead that means each measurement takes a bit over 1.4ms to complete if debug level is set to DBG_WARNING).
- Number of commands sent to the scheduler can also be a limiting factor! By default the minimum dead time before the measurements start is 10s precisely for this reason. However, if there are many commands in short sucession, this may not be enough. If the "ENDSEQUENCE" has not yet been received the firmware asks the PC for more commands only if there is more than 50 ms break in the schedule. The maximum length of the scheduler queue is 512 commands. If the sequence is longer, the scheduler will fetch more commands when time in the schedule allows it.
- Python script can also be a limiting factor!

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
