#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
// Set LCD I2C address (try 0x27 or 0x3F if it doesn't work)
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SDA_PIN 21 // need to defind the sda pin for lcd
#define SCL_PIN 22 // need to defind the scl (serial clock) for lcd 

// BMP280 I2C address (try 0x76 or 0x77)
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
	// initialize the LCD
	Wire.setPins(21, 22);
	Wire.begin();
	lcd.begin();  
  lcd.backlight();

  unsigned status;
  status = bmp.begin(0x76);

  // Initialize BMP280
  if (!status) {
    lcd.setCursor(0, 0);
    lcd.print("BMP280 Error!");
    while (1);
  }
  
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  lcd.setCursor(0, 0);
  lcd.print("BMP280 + ESP32");
  delay(2000);
  lcd.clear();
}

void loop() {
  float temperature = bmp.readTemperature();       // in °C
  float pressure    = bmp.readPressure() / 100.0F; // in hPa
  float altitude    = bmp.readAltitude(1013.25);   // in meters

  // Row 0: Temperature
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C P:");
  lcd.print(pressure, 0);

  // Row 1: Altitude
  lcd.setCursor(0, 1);
  lcd.print("Alt: ");
  lcd.print(altitude, 1);
  lcd.print(" m");

  Serial.print("Temp: "); Serial.println(temperature);
  Serial.print("Pres: "); Serial.println(pressure);

  delay(2000);
}
