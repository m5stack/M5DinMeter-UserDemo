/**
 * @file ble_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../factory_test.h"

#define SERVICE_UUID "1bc68b2a-f3e3-11e9-81b4-2a2ae2dbcce4"
#define CHARACTERISTIC_RX_UUID "1bc68da0-f3e3-11e9-81b4-2a2ae2dbcce4"
#define CHARACTERISTIC_TX_UUID "1bc68efe-f3e3-11e9-81b4-2a2ae2dbcce4"

BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer) { deviceConnected = true; };

    void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

uint8_t* data = new uint8_t[128];

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic* pCharacteristic) { data = pCharacteristic->getData(); }
};

void FactoryTest::_ble_test()
{
    printf("ble test\n");

    if (!_is_ble_inited)
    {
        printf("init ble server\n");

        /* Init BLE server */
        uint64_t chipid = ESP.getEfuseMac();
        String blename = "M5-" + String((uint32_t)(chipid >> 32), HEX);

        BLEDevice::init(blename.c_str());
        // BLEDevice::setPower(ESP_PWR_LVL_N12);
        pServer = BLEDevice::createServer();

        pServer->setCallbacks(new MyServerCallbacks());
        pService = pServer->createService(SERVICE_UUID);
        pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_RX_UUID, BLECharacteristic::PROPERTY_NOTIFY);

        pTxCharacteristic->addDescriptor(new BLE2902());
        BLECharacteristic* pRxCharacteristic =
            pService->createCharacteristic(CHARACTERISTIC_TX_UUID, BLECharacteristic::PROPERTY_WRITE);
        pRxCharacteristic->setCallbacks(new MyCallbacks());

        delay(100);
        _is_ble_inited = true;
    }

    uint8_t senddata[2] = {0};

    pService->start();
    pServer->getAdvertising()->start();

    uint64_t chipid = ESP.getEfuseMac();
    String blename = "M5-" + String((uint32_t)(chipid >> 32), HEX);

    _canvas->setFont(&fonts::Font0);

    uint32_t time_count = millis();

    while (1)
    {
        _canvas->fillRect(0, 0, 240, 135, TFT_BLACK);

        _canvas->fillRect(0, 0, 240, 25, _canvas->color565(20, 20, 20));

        _canvas->setTextSize(2);
        _canvas->setTextColor(TFT_WHITE);
        _canvas->setCursor(0, 6);
        _canvas->printf("     BLE Server\n");

        if ((millis() - time_count) > 100)
        {

            if (deviceConnected)
            {
                // _canvas->pushImage(180, 16, 48, 48, (uint16_t *)icon_ble);
                _canvas->setTextColor(_canvas->color565(18, 150, 219));
                _canvas->setTextSize(3);
                _canvas->setCursor(12, 40);
                // Disbuff.printf("BLE connect!\n");
                _canvas->printf("BLE Send\n");
                _canvas->setTextSize(5);
                _canvas->setCursor(12, 85);

                if (senddata[0] % 4 == 0)
                {
                    _canvas->printf("0x%02X>  ", senddata[0]);
                }
                else if (senddata[0] % 4 == 1)
                {
                    _canvas->printf("0x%02X>>", senddata[0]);
                }
                else if (senddata[0] % 4 == 2)
                {
                    _canvas->printf("0x%02X >>", senddata[0]);
                }
                else if (senddata[0] % 4 == 3)
                {
                    _canvas->printf("0x%02X  >", senddata[0]);
                }

                senddata[1]++;
                if (senddata[1] > 3)
                {
                    senddata[1] = 0;
                    senddata[0]++;
                    pTxCharacteristic->setValue(senddata, 1);
                    pTxCharacteristic->notify();
                }
            }
            else
            {
                _canvas->setTextSize(2);
                _canvas->setCursor(0, 40);
                _canvas->setTextColor(TFT_RED);
                _canvas->printf(" BLE disconnect\n\n");
                // _canvas->setCursor(12, 45);
                _canvas->setTextColor(_canvas->color565(18, 150, 219));

                _canvas->printf(String(" Name: " + blename + "\n").c_str());
                // _canvas->setCursor(12, 70);
                _canvas->printf(" UUID: 1bc68b2a\n");
                // _canvas->pushImage(180, 16, 48, 48, (uint16_t *)icon_ble_disconnect);
            }

            _canvas_update();

            time_count = millis();
        }

        // delay(10);
        _check_encoder();
        if (_check_next())
        {
            break;
        }

        // M5.update();
        // delay(100);
        // checkAXPPress();
    }
    // while ((M5.BtnA.isPressed()) || (M5.BtnB.isPressed())) {
    //     M5.update();
    //     checkAXPPress();
    //     M5.Beep.tone(4000);
    //     delay(10);
    // }
    // delay(50);
    // M5.Beep.mute();

    _canvas->setTextColor(TFT_WHITE);
    pService->stop();
    pServer->getAdvertising()->stop();

    printf("quit ble test\n");
}
