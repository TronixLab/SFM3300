# Sensirion Flow Meter (SFM3300)
## Description
The [SFM3300 Sensirion flow meter](https://www.sensirion.com/en/flow-sensors/mass-flow-meters-for-high-precise-measurement-of-gases/proximal-flow-sensors-sfm3300-autoclavable-washable-or-single-use/) is a sensor for proximal flow measurement in respiratory applications such as ventilation or anesthesia. There are two versions available: the SFM3300-AW is *Autoclavable* and *Washable* and the SFM3300-D is *Disposal*.

| ![space-1.jpg](https://github.com/TronixLab/SFM3300/blob/main/docs/Sensirion%201.jpg) | 
|:--:| 
| **Fig. 1** *Sensirion Proximal Flow Sensors SFM3300-AW & SFM3300-D* |

The SFM3300 features outstanding measuring accuracy and robustness as well as a short signal processing time. The digital flow sensor measures bidirectional flow volumes of up to 250 slm and is fully calibrated for air, N2 and O2 gases. The special design of the flow channel results in a low dead space volume and the slightly greater pressure drop ensures more stability compared to variable inlet conditions. The sensor also features medical cones for pneumatic connection to the respiratory cycle and has a mechanical interface for a user-friendly electrical connection. Thanks to its performance characteristics, the SFM3300 mass flow meter is ideally suited for single-use proximal flow measurements in medical ventilation, hospital applications, residential care applications and emergency situations.

## Features
* Autoclavable / washable (SFM3300-AW) or single-use (SFM3300-D)
* Flow rates: ±250 slm (bidirectional)
* Fully calibrated
* Very fast update time (0.5 ms)
* Small dead space: < 10 ml
* Best-in-class accuracy
* Robust
* Low dependency on different inlet conditions
* Medical cones for pneumatic connection to standard breathing circuits
* Mechanical interface for electrical reconnection

## Applications
* Proximal flow measurement
* Expiratory measurement
* Ventilation
* Anesthesia
* Respiratory measurements
* Metabolic measurements

## Arduino Interfacing
This secton describes the [I2C communication with the SFM3X00 sensor series](https://github.com/TronixLab/SFM3300/blob/main/docs/Sensirion_GF_AN_SFM-05_I2C_Functional_Description_D1.pdf). Communication between the master and the SFM3X00 series sensor runs via the digital I2C-interface. 

| ![space-1.jpg](https://github.com/TronixLab/SFM3300/blob/main/docs/ArduinoInterfacingV2.jpeg) | 
|:--:| 
| **Fig. 2** *Sensirion Flow Sensor Arduino Interfacing* |

### **I2C Connection**
Bi-directional bus lines are implemented by the devices (master and slave) using open-drain output stages and a pull-up resistor (R) connected to the positive supply voltage (5V +- 5%). The recommended pull-up resistor value is 10kOhm. In case of long wires (>10cm) it is necessary to shield SDA and SCL. Wires longer than 30cm should not be used.
### **I2C Address**
By default, the I2C address is set to 64 (0x40).
### **Command Set and Data Transfer Sequences**
After a data transfer is initiated by a header byte with a Write, a command is send to the slave. A command is represented by a 16-bit command code, consisting of two 8-bit frames. The commands are described below, and the data transfer sequences are specified.
| Command |           Description        |
| ------- | ---------------------------- |
| 0x1000  | Start continuous measurement |
| 0x2000  | Soft reset command           |

Flow measurements are started by writing the *start measurement command* (0x1000) to the sensor. Measurement results are continuously updated until measurement is stopped by sending any other command. After a start measurement command, the measurement results can be read out continuously. This means that an I2C header with R/W bit = 1 can be sent continuously to read out those results, without preceding it with command *0x1000*.

The *soft reset command* (0x2000) forces a sensor reset without switching the power off and on again. On receipt of this command, the sensor reinitializes the control/status register contents from the non-volatile memory and starts operating according to these settings.

**Note** that if the supply drops below 4.75V and back, the sensor will perform a Power on Reset and will stop measuring. If the master keeps on sending read headers without resending the start measurement command, those read requests will not be acknowledged. If the sensor does not acknowledge the start measurement command, a *hard reset* (reset the sensor through powering off and on the sensor, in case the sensor freezes) is required.

### **CRC-8 Redundant Data Transmission**
*Cyclic redundancy checking* (CRC) is a popular technique used for error detection in data transmission. The transmitter appends an n-bit checksum to the actual data sequence. The checksum holds redundant information about the data sequence and allows the receiver to detect transmission errors. The computed checksum can be regarded as the remainder of a polynomial division, where the dividend is the binary polynomial defined by the data sequence and the divisor is a “generator polynomial”.

The [SFM3000 sensor implements the CRC-8 standard](https://github.com/TronixLab/SFM3300/blob/main/docs/Sensirion_GF_AN_SFM-04_CRC_Checksum_D1.pdf) based on the generator polynomial: **x8 + x5 + x4 +1. (0x31)**

For easy implementation to a microcontroller, the following simple C++ routine can be used. Note that this code is not optimized for speed.
```
// CRC
#define POLYNOMIAL 0x131 //P(x)=x^8+x^5+x^4+1 = 100110001
//============================================================
u8t SMF3000_CheckCrc (u8t data[], u8t nbrOfBytes, u8t checksum)
//============================================================
// calculates checksum for n bytes of data
// and compares it with expected checksum
// input: data[] checksum is built based on this data
// nbrOfBytes checksum is built for n bytes of data
// checksum expected checksum
// return: error: CHECKSUM_ERROR = checksum does not match
// 0 = checksum matches
//============================================================
{
u8t crc = 0;
u8t byteCtr;
// calculates 8-Bit checksum with given polynomial
for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
{ 
crc ^= (data[byteCtr]);
  for (u8t bit = 8; bit > 0; --bit)
  { 
  if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
  else crc = (crc << 1);
    }
  }
if (crc != checksum) return CHECKSUM_ERROR;
else return 0;
}
```
With the type definitions:
```
typedef enum{
CHECKSUM_ERROR = 0x04
} etError;
typedef unsigned char u8t;
```
### **Converting measurement result to measured values**
In order to obtain the measured flow in the predefined unit, the flow measurement result needs to be converted
according to the following formula:

| ![space-1.jpg](https://github.com/TronixLab/SFM3300/blob/main/docs/eqn.jpg) | 
|:--:| 
| **Eqn. 1** *Flow measurement express in SLM* |

**Offset** and **scale factor** can be found in the [*product datasheet*](https://github.com/TronixLab/SFM3300/blob/main/docs/Sensirion_Mass_Flow_Meters_SFM3300_Datasheet.pdf).
*Please note that the first measurement performed directly after chip initialization might not be valid.*

## Sample Result
| ![space-1.jpg](https://github.com/TronixLab/SFM3300/blob/main/docs/waveform.png) | 
|:--:| 
| **Fig. 3** *Waveform graph using serial plotter* |
