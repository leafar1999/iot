/**
 * Bibliotecas a serem importadas
 * Bluetooth:
 * - ESP32 BLE Arduino
 * - Neil Kolban - https://github.com/nkolban/ESP32_BLE_Arduino
 *
 * PulseSensor:
 * - PulseSensor Playground
 * - Joel Murphy - https://github.com/WorldFamousElectronics/PulseSensorPlayground
 */

/**
 * Importando bibliotecas necessarias para conexão bluetooth
 */
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

/**
 * Variaveis do sensor cardiaco
 */
int PulseSensorPurplePin = 36;
int Signal;

/**
 * Variveis para a comunicação bluetooth
 */
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define SERVICE_UUID           "00001101-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID_RX "00001101-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID_TX "00001101-0000-1000-8000-00805F9B34FB"

/**
 * Criando classes para a comunicação do bluetooth
 */
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


void setup() {
  Serial.begin(115200);
  /**
  * Setup do Bluetooth
  */
  BLEDevice::init("Kiga Bluetooth");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX,BLECharacteristic::PROPERTY_NOTIFY);
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  // Start the service  
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Esperando pela conexão do cliente para notificar...");
}

void loop() {
  if (deviceConnected) {
    Signal = analogRead(PulseSensorPurplePin);
    
    char txString[8]; // make sure this is big enuffz
    dtostrf(Signal, 1, 0, txString); // float_val, min_width, digits_after_decimal, char_buffer
    
    pCharacteristic->setValue(txString);
    
    pCharacteristic->notify(); // Send the value to the app!
    Serial.print("*** Enviado: ");
    Serial.print(String(Signal));
    Serial.println(" ***");
    
    delay(1000);
  }
}
