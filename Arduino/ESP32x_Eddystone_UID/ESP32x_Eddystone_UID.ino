/*
 * Copyright(c) 2023 SLAVR00
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "esp_sleep.h"

// Beacon UUID (Eddystone=0xFEAA)
#define BeaconUUID 0xFEAA
// Device Name
#define BLEDeviceName "ESP32x Eddystone"
// Advertisement Interval
#define DEEP_SLEEP_DURATION  3
// Eddystone UID for 10-byte Namespace + 6-byte Instance (excluding hyphens)
#define EDDYSTONE_UID "0123456789ABCDEF"

BLEAdvertising *pAdvertising;

const uint64_t DeepSleepDuration
  = 1000000LL * DEEP_SLEEP_DURATION;

/* Build Eddystone-UID Frame Data */
const std::string
  frameTop = {
    0x00, // Frame Type ("UID"=0x00)
    0x20, // Ranging Data
  },
  frameUid = {
    EDDYSTONE_UID // UID Data (16-byte)
  },
  frameEnd = {
    0x00, // RFU (Reserved for future use, must be 0x00)
    0x00, // RFU (Reserved for future use, must be 0x00)
  },
  FrameData = frameTop + frameUid + frameEnd;

BLEUUID BleUUID = BLEUUID((uint16_t)BeaconUUID);

inline void initBLEDevice() {
  BLEDevice::init(BLEDeviceName);
  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();
  BLEDevice::startAdvertising();
}

inline void transmitBeacon() {
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  // Data setup
  oAdvertisementData.setFlags(0x06);
  oAdvertisementData.setCompleteServices(BleUUID);
  oAdvertisementData.setServiceData(BleUUID, FrameData);
  pAdvertising->setScanResponseData(oAdvertisementData);
  // Transmit
  pAdvertising->start();
  delay(100);
  pAdvertising->stop();
}

void setup() {
  initBLEDevice();
  transmitBeacon();
  esp_deep_sleep(DeepSleepDuration);
}

void loop() {}