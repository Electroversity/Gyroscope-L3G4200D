#include <Wire.h>

// For storing the raw values from gyro - rotational velocity
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

byte L3GD20_address = 0b1101001; // currently using 105 but online stated 106 for l3gd20

void setup() {
  Serial.begin(9600);
  Wire.begin(); // for initialising I2C communication
  setupGyro50(2000);  // Configure L3G4200D - 250, 500 or 2000 deg/sec
  Serial.println("Sensor Initiated");
  delay(1500);  // wait for the sensor to be ready
}

void loop() {
  gyroValues(2000);
  printData();
  delay(100);
}

int setupGyro50(int scale){
  // Listing of the 8 bit registers embedded in the device, and the related addresses
  // For the following please refer from page 31 on knowing how to assign the value
  
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

void writeRegister(byte deviceAddress, byte address, byte val){
  Wire.beginTransmission(deviceAddress);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}

long gyroValues(int scale){
  // Refer page 29
  byte xMSB = readRegister(L3GD20_address, 0x29);
  byte xLSB = readRegister(L3GD20_address, 0x28);
  Serial.print("xMSB = "); Serial.println(xMSB);
  Serial.print("xLSB = "); Serial.println(xLSB);
  gyroX = ((xMSB << 8) | xLSB); // bitshift left and bitwise or
  Serial.print("gyroX value= ");
  Serial.println(gyroX);
  
  byte yMSB = readRegister(L3GD20_address, 0x2B);
  byte yLSB = readRegister(L3GD20_address, 0x2A);
  gyroY = ((yMSB << 8) | yLSB);
  
  byte zMSB = readRegister(L3GD20_address, 0x2D);
  byte zLSB = readRegister(L3GD20_address, 0x2C);
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

float processGyroData(int dps){
  if (dps==250){
      rotX = gyroX / 262.144;
      rotY = gyroY / 262.144;
      rotZ = gyroZ / 262.144;
  }
  else if (dps==500){
      rotX = gyroX / 131.072;
      rotY = gyroY / 131.072;
      rotZ = gyroZ / 131.072;
  }else{
      rotX = gyroX / 32.768;
      rotY = gyroY / 32.768;
      rotZ = gyroZ / 32.768;
  }
}

void printData(){
  Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print(" Y=");
  Serial.print(rotY);
  Serial.print(" Z=");
  Serial.println(rotZ);
}
