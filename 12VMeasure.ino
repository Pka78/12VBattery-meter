#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32 Bluetooth Majakka");
}

void loop() {
  float Vin = analogRead(36) * 3.3 / 4095.0; // Mittaa jännitteen ADC-muuntimella
  float Vout = Vin * (100000.0 + 10000.0) / 10000.0; // Laske jännitteenjakajan jännite
  float V12 = Vout * 12.0 / 3.3; // Muunna Vout-jännite 12V-jännitteeksi
  SerialBT.print("12V:"); // Lähetä Bluetooth-majakkaan merkkijono "12V:"
  SerialBT.println(V12); // Lähetä Bluetooth-majakkaan mitattu 12V-jännite
  delay(60000); // Odota yksi minuutti ennen seuraavaa mittaus- ja lähetyskierrosta
}
