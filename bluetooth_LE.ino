#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "cf58de3e-a97b-4803-b558-1b07ab533b44"
#define CHARACTERISTIC_UUID "6d049857-d48c-4a34-a101-c6c8bded81a6"

BLECharacteristic *pCharacteristicPM = nullptr;

bool initBLEService()
{

  BLEDevice::init("Air_Quality_Sensor");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristicPM = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );

  pCharacteristicPM->setValue("");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  return true;
}

bool setBLEPMValues(float pm_2_5, float pm_10)
{
  char text[50];
  // Write PM values is JSON format
  sprintf(text, "{ \"pm_2_5\" : \"%.1f\", \"pm_10\" : \"%.1f\" }", pm_2_5, pm_10);
  pCharacteristicPM->setValue(text);
  return true;
}
