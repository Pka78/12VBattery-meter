#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

const int voltagePin = 35; // analog input pin for voltage measurement
const int ledPin = 2; // pin for the built-in LED
const float vDivider = 0.176; // voltage divider factor
const int reportInterval = 60000; // report interval in milliseconds

BLEServer *pServer = NULL;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
    }
    void onDisconnect(BLEServer* pServer) {
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("12V Battery Meter");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService("0x180F");

  // Create the BLE Characteristic for voltage measurement
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         BLEUUID("2A19"),
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

}

void loop() {
  float voltage = 0.0;
  int numSamples = 100;
  for (int i = 0; i < numSamples; i++) {
    voltage += analogRead(voltagePin);
    delay(1);
  }
  voltage /= numSamples;
  voltage = voltage * vDivider;
  Serial.printf("Voltage: %.2fV\n", voltage);

  // blink the LED
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);

  // Update the BLE Characteristic with the current voltage
  String voltageStr = String(voltage, 2);
  pServer->updateCharacteristicValue(pCharacteristic->getHandle(), voltageStr.c_str(), voltageStr.length());

  // Wait for the next report interval
  delay(reportInterval);
}
