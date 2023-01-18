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
