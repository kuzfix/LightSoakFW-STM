# LightSoakV1 Firmware
This is the firmware for the LightSoakV1 project, developed as a part of my Master's thesis. It's intended to run on LightSoakV1 hardware. See https://github.com/mrmp17/LightSoakFW-STM for hardware design files. For Python data logging software, see https://github.com/mrmp17/LightSoakFW-Python.

## Light soaking measurements
Perovskite solar cells show a phenomenon called light soaking, where the cell's forward voltage increases when exposed to light for some time. The relaxation of which is challenging to measure, as to be able to measure forward voltage, the cell needs to be illuminated, which in turn changes the relaxation curve.

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

## Command line interface

This firmware implements a command line interface over serial port at 230400 default baud rate at the device's USB-C port. This enables an easy-to-use standalone usage of the device for initial measurements and preparation of a more complex sequence, for which Python data logging software is used. The CLI provides a consistent interface for both standalone and Python usage. All data to and from the device is in human-readable format, making it easy to debug and develop new features.

## Quick start quide
Connect the hardware to your computer using a USB-C cable. If using without external power connected, make sure *VBUS-PWR* jumper is connected. The device will be powered from USB but analog performance may be degraded.

Make sure the device is detected as a CP2104 USB-UART converter. Drivers are not needed for MacOS and Linux but may be required for some Windows installs. See https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers in case of issues.

Connect to the device using a serial terminal of your choice. The terminal should support VT100 escape codes for proper display of the CLI. *Screen* is recomended on MacOS and Linux. Default baud rate is 230400.

Once connected, type *reboot* to reboot the device. While typing, text should appear in the terminal. Once rebooted, a welcome message with some basic instrutions will appear. The device is now ready to use.

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
- ***setshunt*** - Manually sets shunt range on a specific channel/s. Recomended ranges for currents are:
  - 1X: Above 0.38mA
  - 10X: Above 38uA
  - 100X: Above 3.8uA
  - 1000X: Below 3.8uA
- ***setforcevolt*** - Manually sets voltage to be forced on a specific channel/s. Keep in mind the actual voltage on DUT deffers by the voltage drop on the shunt resistor. Always measure the actual voltage on DUT with *getvolt*.
- ***autorange*** - Manually trigger shunt autoranging on all channels.

- ***ENDSEQUENCE*** - This command should be called by Python data logging code at the end of the sequence.