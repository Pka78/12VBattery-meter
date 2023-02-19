#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define ADC_EN          14
#define ADC_PIN         34
#define ADC_ATTEN       ADC_ATTEN_DB_11 // maksimijännite 3.9V

#define RED_LED_PIN     2
#define BLUE_LED_PIN    4

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
int voltage = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ADC_EN, OUTPUT);
  digitalWrite(ADC_EN, HIGH);

  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);

  pinMode(BLUE_LED_PIN, OUTPUT);
  digitalWrite(BLUE_LED_PIN, LOW);

  BLEDevice::init("ESP32 12V Voltage Sensor");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void loop() {
  if (deviceConnected) {
    digitalWrite(RED_LED_PIN, LOW);

    voltage = analogRead(ADC_PIN) * 2 / 4095.0 * 12.0;
    char val[5];
    sprintf(val, "%d", voltage);
    pCharacteristic->setValue(val);
    pCharacteristic->notify();

    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(500);
    digitalWrite(BLUE_LED_PIN, LOW);
  } else {
    digitalWrite(RED_LED_PIN, HIGH);
  }

  if (deviceConnected != oldDeviceConnected) {
    if (deviceConnected) {
      Serial.println("Device connected");
    } else {
      Serial.println("Device disconnected");
    }
    oldDeviceConnected = deviceConnected;
  }

  delay(60000); // odota yksi minuutti
}
