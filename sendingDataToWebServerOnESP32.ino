#include "mpu6500.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

Adafruit_BMP280 bmp;
bfs::Mpu6500 imu;

const char* ssid = "......";
const char* pass = "....";

WebServer server(80);

struct BMP280Data  { float temp, pressure, altitude; };
struct MPU6500Data { float ax, ay, az; };

BMP280Data readBMP280() {
  BMP280Data data;
  data.temp     = bmp.readTemperature();
  data.pressure = bmp.readPressure();
  data.altitude = bmp.readAltitude(1020.1); 
  /*float Adafruit_BMP280::readAltitude(float seaLevelhPa) {
  float altitude;

  float pressure = readPressure(); // in Si units for Pascal
  pressure /= 100;

  altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
  link: https://github.com/adafruit/Adafruit_BMP280_Library/blob/master/Adafruit_BMP280.cpp
*/
  return data;
}

MPU6500Data readMPU6500() {
  MPU6500Data data;
  data.ax = 0; data.ay = 0; data.az = 0;
  if (imu.Read()) {
    data.ax = imu.accel_x_mps2();
    data.ay = imu.accel_y_mps2();
    data.az = imu.accel_z_mps2();
  }
  return data;
}

void handleData() {
  BMP280Data b  = readBMP280();
  MPU6500Data m = readMPU6500();
  StaticJsonDocument<256> doc;
  doc["t"] = b.temp;
  doc["p"] = b.pressure;
  doc["a"] = b.altitude;
  doc["x"] = m.ax;
  doc["y"] = m.ay;
  doc["z"] = m.az;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleRoot() {
  String h = "";
  h += "<!DOCTYPE html><html><head>";
  h += "<meta charset=UTF-8>";
  h += "<meta name=viewport content='width=device-width,initial-scale=1'>";
  h += "<title>ESP32 Sensors</title>";
  h += "<style>";
  h += "body{margin:0;padding:16px;background:#0f1117;color:#e8eaf0;font-family:system-ui,sans-serif;font-size:16px;}";
  h += "header{display:flex;align-items:center;justify-content:space-between;padding-bottom:20px;border-bottom:1px solid #222;margin-bottom:20px;}";
  h += ".logo{font-size:18px;font-weight:600;display:flex;align-items:center;gap:8px;}";
  h += ".dot{width:10px;height:10px;border-radius:50%;background:#69f0ae;animation:p 2s infinite;}";
  h += "@keyframes p{0%,100%{opacity:1}50%{opacity:0.3}}";
  h += ".grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(140px,1fr));gap:10px;margin-bottom:20px;}";
  h += ".card{background:#1a1d27;border:1px solid #222;border-radius:12px;padding:18px 14px;}";
  h += ".lbl{font-size:11px;color:#8b90a0;text-transform:uppercase;letter-spacing:.08em;margin-bottom:6px;}";
  h += ".val{font-size:26px;font-weight:700;line-height:1;}";
  h += ".unt{font-size:12px;color:#8b90a0;margin-top:4px;}";
  h += ".sec{font-size:12px;font-weight:600;color:#8b90a0;text-transform:uppercase;letter-spacing:.1em;margin-bottom:10px;}";
  h += ".upd{font-size:12px;color:#8b90a0;text-align:center;margin-top:14px;}";
  h += "</style></head><body>";
  h += "<header><div class=logo><div class=dot></div>ESP32 Sensors</div>";
  h += "<span id=st style='font-size:13px;color:#8b90a0'>Live</span></header>";
  h += "<p class=sec>Environment - BMP280</p><div class=grid>";
  h += "<div class=card><div class=lbl>Temperature</div><div class=val id=tmp style='color:#ffb74d'>--</div><div class=unt>C</div></div>";
  h += "<div class=card><div class=lbl>Pressure</div><div class=val id=prs style='color:#4fc3f7'>--</div><div class=unt>hPa</div></div>";
  h += "<div class=card><div class=lbl>Altitude</div><div class=val id=alt style='color:#69f0ae'>--</div><div class=unt>m</div></div>";
  h += "</div><p class=sec>Accelerometer - MPU6500</p><div class=grid>";
  h += "<div class=card><div class=lbl>Accel X</div><div class=val id=ax style='color:#ce93d8'>--</div><div class=unt>m/s2</div></div>";
  h += "<div class=card><div class=lbl>Accel Y</div><div class=val id=ay style='color:#ce93d8'>--</div><div class=unt>m/s2</div></div>";
  h += "<div class=card><div class=lbl>Accel Z</div><div class=val id=az style='color:#ce93d8'>--</div><div class=unt>m/s2</div></div>";
  h += "</div><p class=upd id=upd>Waiting...</p>";
  h += "<script>";
  h += "function go(){fetch('/data').then(function(r){return r.json();}).then(function(d){";
  h += "document.getElementById('tmp').textContent=d.t.toFixed(1);";
  h += "document.getElementById('prs').textContent=(d.p/100).toFixed(1);";
  h += "document.getElementById('alt').textContent=d.a.toFixed(1);";
  h += "document.getElementById('ax').textContent=d.x.toFixed(2);";
  h += "document.getElementById('ay').textContent=d.y.toFixed(2);";
  h += "document.getElementById('az').textContent=d.z.toFixed(2);";
  h += "document.getElementById('upd').textContent='Updated: '+new Date().toLocaleTimeString();";
  h += "document.getElementById('st').textContent='Live';";
  h += "}).catch(function(){document.getElementById('st').textContent='Reconnecting...';});}";
  h += "go();setInterval(go,2000);";
  h += "</script></body></html>";
  server.send(200, "text/html", h);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  Wire.setClock(400000);
  bmp.begin(0x76);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  imu.Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);
  imu.Begin();
  imu.ConfigSrd(19);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("");
  Serial.print("Open in browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/",     handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Server started!");
}

void loop() {
  server.handleClient();
  
}