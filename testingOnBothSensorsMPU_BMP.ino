#include "mpu6500.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SDA_PIN 21 // need to defind the sda pin for lcd
#define SCL_PIN 22 // need to defind the scl (serial clock) for lcd 

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

/* Mpu6500 object */
bfs::Mpu6500 imu;

struct BMP280Data {                  
  float temp;
  float pressure;
  float altitude;
};                                    

struct MPU6500Data {                   
  float ax;
  float ay;
  float az;
};    


void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  Wire.setClock(400000); // set the max clock f to 400kHz
  
  // BMP280
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */  

  /* I2C bus,  0x68 address */
  imu.Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);
  /* Initialize and configure IMU */
}



BMP280Data readBMP280() {
  BMP280Data data;
  data.temp     = bmp.readTemperature(); 
  data.pressure = bmp.readPressure();
  data.altitude = bmp.readAltitude(1006.39);
  return data;
}

MPU6500Data readMPU6500() {
  MPU6500Data data;
  data.ax = imu.accel_x_mps2();
  data.ay = imu.accel_y_mps2();
  data.az = imu.accel_z_mps2();
  return data;
}


void loop() {
  BMP280Data bmpData = readBMP280();
  MPU6500Data mpuData = readMPU6500();
  Serial.println("===== BMP280 =====");
  Serial.print("  Temp     : "); Serial.print(bmpData.temp);Serial.println("C");
  Serial.print("  Pressure     : "); Serial.print(bmpData.pressure);Serial.println("Pa");
  Serial.print("  Altitude     : "); Serial.print(bmpData.altitude);Serial.println("m");
  Serial.println("===== MPU6500 =====");
  Serial.print("  Accel X  : "); Serial.print(mpuData.ax, 3); Serial.println(" m/s2");
  Serial.print("  Accel Y  : "); Serial.print(mpuData.ay, 3); Serial.println(" m/s2");
  Serial.print("  Accel Z  : "); Serial.print(mpuData.az, 3); Serial.println(" m/s2");

  Serial.println("-------------------");
  delay(1000);

}





              