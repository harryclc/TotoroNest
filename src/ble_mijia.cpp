#include "ble_mijia.h"

#include <ArduinoJson.h>
#include <NimBLEDevice.h>

#include "config.h"
#include "secrets.h"
#include "network.h"
#include "status.h"

const static NimBLEAdvertisedDevice *advDevice;
bool doConnect = false;
bool connected = false;
BLEUUID miServiceUUID(BLE_MI_SERVICE_UUID);
BLEUUID miTempNotifyCharUUID(BLE_MI_TEMP_NOTIFY_CHAR_UUID);

// 蓝牙回调
class ScanCallbacks final : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice *advertisedDevice) override {
        // Serial.printf("发现设备: %s\n", advertisedDevice->toString().c_str());
        if (advertisedDevice->getAddress().toString() == MI_MAC) {
            Serial.printf("发现目标米家温湿度计设备！%s\n", advertisedDevice->toString().c_str());
            NimBLEDevice::getScan()->stop();
            advDevice = advertisedDevice;
            doConnect = true;
        }
    }

    void onScanEnd(const NimBLEScanResults &results, int reason) override {
        // Serial.printf("扫描结束, reason: %d, 发现设备数: %d; 重新开始扫描\n", reason, results.getCount());
        NimBLEDevice::getScan()->start(BLE_SCAN_TIME_MS, false, true);
    }
} scanCallbacks;

// 蓝牙通知回调
void notifyMi(NimBLERemoteCharacteristic *pChar, const uint8_t *pData, const size_t length, bool isNotify) {
    if (length >= 3) {
        const uint8_t tempLow = pData[0];
        const uint8_t tempHigh = pData[1];
        const uint8_t humidity = pData[2];

        const bool sign = tempHigh & 0x80;
        const int tempRawUnsigned = (tempHigh & 0x7F) << 8 | tempLow;
        const int tempRaw = sign ? tempRawUnsigned - 32767 : tempRawUnsigned;
        const float temperature = static_cast<float>(tempRaw) / 100.0f;

        Serial.printf("[Notify] 温度: %.2f °C, 湿度: %d%%\n", temperature, humidity);
        publishAmbient(temperature, humidity);
        ambient_temp = temperature;
        ambient_humidity = humidity;
    } else {
        Serial.println("[Notify] 数据长度错误！");
    }
}

// 初始化蓝牙
bool initBLE() {
    NimBLEDevice::init("TotoroNest-Client");
    // NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_SC);
    NimBLEScan *pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(&scanCallbacks, false);
    pScan->setActiveScan(true);
    return pScan->start(BLE_SCAN_TIME_MS);
}

// 蓝牙连接
bool connectBLE() {
    NimBLEClient *pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
    if (!pClient) {
        if (NimBLEDevice::getCreatedClientCount() >= NIMBLE_MAX_CONNECTIONS) {
            return false;
        }
        pClient = NimBLEDevice::createClient();
        // pClient->setConnectionParams(12, 12, 0, 150);
        // pClient->setConnectTimeout(5 * 1000);
    }

    if (!pClient->isConnected()) {
        if (!pClient->connect(advDevice)) {
            return false;
        }
    }

    // 连接成功后
    Serial.printf("已连接: %s RSSI: %d\n", pClient->getPeerAddress().toString().c_str(), pClient->getRssi());

    const NimBLERemoteService *miService = pClient->getService(miServiceUUID);
    if (miService) {
        // Serial.println("检测到米家温湿度计服务！");
        const NimBLERemoteCharacteristic *miTempChar = miService->getCharacteristic(miTempNotifyCharUUID);
        if (miTempChar && miTempChar->canNotify()) {
            // Serial.println("找到温湿度通知特征，准备开启Notify...");
            return miTempChar->subscribe(true, notifyMi);
        }
    }
    return false;
}

// 检查蓝牙连接状态
void ensureBLE() {
    if (doConnect) {
        doConnect = false;
        if (connectBLE()) {
            connected = true;
            // Serial.printf("蓝牙连接成功! 开始获取通知, 正在扫描更多设备!\n");
        } else {
            connected = false;
            Serial.println("蓝牙连接失败");
        }
        NimBLEDevice::getScan()->start(BLE_SCAN_TIME_MS, false, true);
    }
}

bool isBLEConnected() {
    return connected;
}
