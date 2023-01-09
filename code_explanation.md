
```c
#include <Wire.h>
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

byte L3GD20_address = 0b1101001; 
```
For the above code
- We have included `Wire` library which allows Arduino Uno to communicate with the I2C/TWI devices. 
- Next, we have initialised some variables to store the raw values from gyroscope sensor
    - `rotX`: rotation in x axis
    - `rotY`: rotation in y axis
    - `rotZ`: rotation in z axis
- If we refer the datasheet [L3GD20 datasheet](/L3G4200D%20datasheet.pdf), we can see that the address associated with it is 110101xb but here in our case for some reason it doesn't work whereas it works with the address 110100xb (the address for L3G4200D). The SAO pin in our gyroscope allows us to choose the last bit of the address by setting it high or low. 
    - Thus we have two possible addresses: 1101001 or 1101000.

```c
void setup() {
  Serial.begin(9600);
  Wire.begin(); 
  setupGyro50(2000);  
  Serial.println("Sensor Initiated");
  delay(1500);  
}

void loop() {
  gyroValues(2000);
  printData();
  delay(100);
}
```
- Setup Section
    - Here we set the baud rate for serial communication along with initialising the wire library
    - `setupGyro50(n)` where *n* is scale of degrees per seconds that we want.
- Loop Section
    - `gyroValues(n)` is responsible to get the raw  values of `gyroX`, `gyroY` and `gyroZ` measured by the gyroscope.
    - We have also defined a function `printData` which is responsible for printing the processed Gyro data on to the serial monitor and create a delay of 100ms to slow down the transmission of data and make the values readable.

```c
int setupGyro50(int scale){
  // Listing of the 8 bit registers embedded in the device, and the related addresses
  // For the following please refer from page 29 on knowing how to assign the value
  
  writeRegister(L3GD20_address, 0x20, 0b00001111); 
  writeRegister(L3GD20_address, 0x21, 0b00000000); // must be set to 0 to ensure proper operation of device
  if (scale==250){
    writeRegister(L3GD20_address, 0x23, 0b00000000);
  }
  else if (scale==500){
    writeRegister(L3GD20_address, 0x23, 0b00010000);
  }
  else{
    writeRegister(L3GD20_address, 0x23, 0b00110000);
  }
}
```
According to the datasheet, we have following set of control registers that we need to configure 
- `0x20`: CTRL_REG1 register
  - To configure last 4 bits
  - To enable Normal power mode i.e., assigning it 1
  - To enable Z, Y, X axis i.e., assigning them 1
- `0x21`: CTRL_REG2 register
  - Here the values are loaded at boot. Values must not change and should be 0 for proper functioning.
- `0x23`: CTRL_REG4 register
  - Now depending upon the *dps* specified the 3rd and 4th bit are configured
  - **250 dps** = 00
  - **500 dps** = 01
  - **2000 dps** = 10 or 11

```c
void writeRegister(byte deviceAddress, byte address, byte val){
  Wire.beginTransmission(deviceAddress);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}
```
Detailed Information on Wire Library: [Wire-Arduino](https://www.arduino.cc/reference/en/language/functions/communication/wire/)

Here the input arguments are
1. *deviceAddress* for I2C Communication - defined earlier
2. *address* of control register 
3. *val* is the value to assign the control register for operation

Function description in short
- `beginTransmission()`: begins the I2C communication protocol with the given address
- Subsequently, queue bytes for transmission with the `write()` function and transmit them by calling `endTransmission()`

