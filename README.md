# Mass Flow Meter SFM3300
## [Description](https://www.sensirion.com/en/flow-sensors/mass-flow-meters-for-high-precise-measurement-of-gases/proximal-flow-sensors-sfm3300-autoclavable-washable-or-single-use/)
The SFM3300 mass flow meter is a sensor for proximal flow measurement in respiratory applications such as ventilation or anesthesia. There are two versions available: the SFM3300-AW is *Autoclavable* and *Washable* and the SFM3300-D is *Disposal*.

| ![space-1.jpg](https://github.com/TronixLab/SFM3300/blob/main/docs/Sensirion%201.jpg) | 
|:--:| 
| **Fig. 1** *Proximal Flow Sensors Sensirion* |

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
This secton describes the I2C communication with the SFM3X00 sensor series. Communication between the master and the SFM3X00 series sensor runs via the digital I2C-interface. 

| ![space-1.jpg](https://github.com/TronixLab/SFM3300/blob/main/docs/ArduinoInterfacing.jpeg) | 
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
### **Measurement Triggering**
Flow measurements are started by writing the start measurement command (0x1000) to the sensor. Measurement results are continuously updated until measurement is stopped by sending any other command. After a start measurement command, the measurement results can be read out continuously. This means that an I2C header with R/W bit = 1 can be sent continuously to read out those results, without preceding it with command *0x1000*.
**Note** that if the supply drops below 4.75V and back, the sensor will perform a Power on Reset and will stop measuring. If the master keeps on sending read headers without resending the start measurement command, those read requests will not be acknowledged. If the sensor does not acknowledge the start measurement command, a *hard reset* (reset the sensor through powering off and on the sensor, in case the sensor freezes) is required.
