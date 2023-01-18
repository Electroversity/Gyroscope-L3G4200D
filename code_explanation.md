---
layout: default
title: Working of the Code
nav_order: 3
---

# Code Explanation
---

```c
#include <Wire.h>
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

byte L3G4200D_address = 0b1101001; 
```
For the above code
- We have included `Wire` library which allows Arduino Uno to communicate with the I2C/TWI devices. 
- Next, we have initialised some variables to store the raw values from gyroscope sensor
    - `rotX`: rotation in x axis
    - `rotY`: rotation in y axis
    - `rotZ`: rotation in z axis
- If we refer the datasheet [L3G4200D datasheet](https://github.com/Electroversity/Gyroscope-L3G4200D/blob/website_gyro/L3G4200D%20datasheet.pdf), we can see the address for the device is 105
    - Thus we have two possible addresses: 1101001 or 1101000.

```c
void setup() {
  Serial.begin(9600);
  Wire.begin(); 
  setupGyro50(250);  
  Serial.println("Sensor Initiated");
  delay(1500);  
}

void loop() {
  gyroValues(250);
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
  
  writeRegister(L3G4200D_address, 0x20, 0b00001111); 
  writeRegister(L3G4200D_address, 0x21, 0b00000000); // must be set to 0 to ensure proper operation of device
  if (scale==250){
    writeRegister(L3G4200D_address, 0x23, 0b00000000);
  }
  else if (scale==500){
    writeRegister(L3G4200D_address, 0x23, 0b00010000);
  }
  else{
    writeRegister(L3G4200D_address, 0x23, 0b00110000);
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

**Function description in short**
- `beginTransmission()`: begins the I2C communication protocol with the given address
- Subsequently, queue bytes for transmission with the `write()` function and transmit them by calling `endTransmission()`

```c
long gyroValues(int scale){
  // Refer page 27
  byte xMSB = readRegister(L3G4200D_address, 0x29);
  byte xLSB = readRegister(L3G4200D_address, 0x28);
  gyroX = ((xMSB << 8) | xLSB); // bitshift left and bitwise or
  
  byte yMSB = readRegister(L3G4200D_address, 0x2B);
  byte yLSB = readRegister(L3G4200D_address, 0x2A);
  gyroY = ((yMSB << 8) | yLSB);
  
  byte zMSB = readRegister(L3G4200D_address, 0x2D);
  byte zLSB = readRegister(L3G4200D_address, 0x2C);
  gyroZ = ((zMSB << 8) | zLSB);

  processGyroData(scale);  

  
// 8.5, 17.5 and 70 have been obtained from the datasheet

float processGyroData(int dps){
  if (dps==250){
      rotX = gyroX*8.5 / 1000;
      rotY = gyroY*8.5 / 1000;
      rotZ = gyroZ*8.5 / 1000;
  }
  else if (dps==500){
      rotX = gyroX*17.5 / 1000;
      rotY = gyroY*17.5 / 1000;
      rotZ = gyroZ*17.5 / 1000;
  }else{
      rotX = gyroX*70 / 1000;
      rotY = gyroY*70 / 1000;
      rotZ = gyroZ*70 / 1000;
}
}
```
Here, *MSB* refers to Most Significant Bit and *LSB* refers to Least Significant Bit
- `xMSB << 8` performs a *bitshift left operation* 
  - eg. 3 << 8 will result to 11000
- `result | xLSB` performs a *bitwise or operation*
  - eg. 11000 | 10 will result to 1101

`processGyroData()` is responsible to result in *scale of rotation along the 3 axis in degrees*

```c
int readRegister(byte deviceAddress, byte address){
  int v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1);

  while(!Wire.available()){
    
  }
  v = Wire.read();
  Serial.print("v = "); Serial.println(v);
  return v;
}
```
The function is used to read values from the registers. 

- `Wire.requestFrom(address, quantity)`: used by the master to request bytes from a slave device
  - *quantity*: the number of bytes to request
- `Wire.available()`: returns the number of bytes available for reading
- `Wire.read()`: reads a byte that was transmitted from a slave device to a master after a call to `requestFrom()` or was transmitted from a master to a slave

**Final Code**

```c

/*
################ ELECTROVERSITY ##############
With great open source power comes great open source responsibility.
#############################################
*/

#include <Wire.h>

// For storing the raw values from gyro - rotational velocity
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

byte L3G4200D_address = 0b1101001; // 105

void setup() {
  Serial.begin(9600);
  Wire.begin(); // for initialising I2C communication
  setupGyro50(250);  // Configure L3G4200D - 250, 500 or 2000 deg/sec
  Serial.println("Sensor Initiated");
  delay(1500);  // wait for the sensor to be ready
}

void loop() {
  gyroValues(250);
  printData();
  delay(100);
}

int setupGyro50(int scale){
  // Listing of the 8 bit registers embedded in the device, and the related addresses
  // For the following please refer from page 31 on knowing how to assign the value
  
  writeRegister(L3G4200D_address, 0x20, 0b00001111); 
  writeRegister(L3G4200D_address, 0x21, 0b00000000); // must be set to 0 to ensure proper operation of device
  if (scale==250){
    writeRegister(L3G4200D_address, 0x23, 0b00000000);
  }
  else if (scale==500){
    writeRegister(L3G4200D_address, 0x23, 0b00010000);
  }
  else{
    writeRegister(L3G4200D_address, 0x23, 0b00110000);
  }
}

void writeRegister(byte deviceAddress, byte address, byte val){
  Wire.beginTransmission(deviceAddress);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}

long gyroValues(int scale){
  // Refer page 29
  byte xMSB = readRegister(L3G4200D_address, 0x29);
  byte xLSB = readRegister(L3G4200D_address, 0x28);
  Serial.print("xMSB = "); Serial.println(xMSB);
  Serial.print("xLSB = "); Serial.println(xLSB);
  gyroX = ((xMSB << 8) | xLSB); // bitshift left and bitwise or
  Serial.print("gyroX value= ");
  Serial.println(gyroX);
  
  byte yMSB = readRegister(L3G4200D_address, 0x2B);
  byte yLSB = readRegister(L3G4200D_address, 0x2A);
  gyroY = ((yMSB << 8) | yLSB);
  
  byte zMSB = readRegister(L3G4200D_address, 0x2D);
  byte zLSB = readRegister(L3G4200D_address, 0x2C);
  gyroZ = ((zMSB << 8) | zLSB);

  processGyroData(scale);  
}

int readRegister(byte deviceAddress, byte address){
  int v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1);

  while(!Wire.available()){
    
  }
  v = Wire.read();
  Serial.print("v = "); Serial.println(v);
  return v;
}

// 8.5, 17.5 and 70 have been obtained from the datasheet

float processGyroData(int dps){
  if (dps==250){
      rotX = gyroX*8.5 / 1000;
      rotY = gyroY*8.5 / 1000;
      rotZ = gyroZ*8.5 / 1000;
  }
  else if (dps==500){
      rotX = gyroX*17.5 / 1000;
      rotY = gyroY*17.5 / 1000;
      rotZ = gyroZ*17.5 / 1000;
  }else{
      rotX = gyroX*70 / 1000;
      rotY = gyroY*70 / 1000;
      rotZ = gyroZ*70 / 1000;
}

void printData(){
  // below values have been specified to 0.6 because I found them to min and max values(the values fluctuated between this range) when sensor was left stationary
  float low = -0.6;
  float high = 0.6;

  Serial.print("Gyro (deg)");
  Serial.print(" X=");

  if((rotX>low)&&(rotX<high)){
  Serial.print("0.00");
  } else{
    Serial.print(rotX);
  }

  Serial.print(" Y=");
  if((rotY>low)&&(rotY<high)){
  Serial.print("0.00");
  } else{
    Serial.print(rotY);
  }

  Serial.print(" Z=");
  if((rotZ>low)&&(rotZ<high)){
  Serial.print("0.00");
  } else{
    Serial.print(rotZ);
  }
}


```