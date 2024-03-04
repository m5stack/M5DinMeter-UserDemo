/**
 * @file io_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../factory_test.h"

void FactoryTest::_io_test()
{
    printf("io test\n");

    /* Display */
    _canvas->fillScreen(TFT_BLACK);
    _canvas->setFont(&fonts::efontCN_24);
    _canvas->setTextSize(1);
    _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);

    uint32_t time_count = 0;
    bool grove1_pass = false;
    bool grove2_pass = false;

    while (1)
    {

        if ((millis() - time_count) > 400)
        {

            _canvas->fillScreen(TFT_BLACK);
            _canvas->setCursor(0, 0);
            _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
            _canvas->printf(" [IO测试]\n");
            _canvas->setTextColor(TFT_WHITE);

            float v_bat = (float)analogReadMilliVolts(10) * 2 / 1000;

            _canvas->setCursor(0, 20);
            _canvas->setTextColor((v_bat < 3) ? TFT_RED : TFT_GREEN, TFT_BLACK);
            _canvas->printf("   电池: %.2fV\n", v_bat);

            // if (grove1_pass && grove2_pass) {
            //     time_count = millis();
            //     continue;
            // }

            _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
            _canvas->printf(" [Grove 1]\n");

            byte error, address;
            uint8_t addr = 0x00;
            Wire1.end();
            Wire1.begin(2, 1, 100000);
            for (address = 0x01; address < 0x7f; address++)
            {
                Wire1.beginTransmission(address);
                error = Wire1.endTransmission();
                if (error == 0)
                {
                    // Serial.printf("I2C device found at address 0x%02X\n", address);

                    addr = address;
                    printf("fonud 0x%02X\n", addr);
                    grove1_pass = true;
                    break;
                }
            }

            if (addr)
            {
                _canvas->setTextColor(TFT_GREEN, TFT_BLACK);
                _canvas->printf("   检测到: 0x%02X\n", addr);
            }
            else
            {
                _canvas->setTextColor(TFT_RED, TFT_BLACK);
                _canvas->printf("   无      \n");
            }

            _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
            _canvas->printf(" [Grove 2]\n");

            // byte error, address;
            addr = 0x00;
            Wire1.end();
            Wire1.begin(13, 15, 100000);
            for (address = 0x01; address < 0x7f; address++)
            {
                Wire1.beginTransmission(address);
                error = Wire1.endTransmission();
                if (error == 0)
                {
                    // Serial.printf("I2C device found at address 0x%02X\n", address);

                    addr = address;
                    printf("fonud 0x%02X\n", addr);
                    grove2_pass = true;
                    break;
                }
            }

            if (addr)
            {
                _canvas->setTextColor(TFT_GREEN, TFT_BLACK);
                _canvas->printf("   检测到: 0x%02X\n", addr);
            }
            else
            {
                _canvas->setTextColor(TFT_RED, TFT_BLACK);
                _canvas->printf("   无      \n");
            }

            _canvas_update();

            time_count = millis();
        }

        _check_encoder();
        if (_check_next())
        {
            break;
        }
    }

    printf("quit io test\n");
}

void FactoryTest::_io_test_user()
{
    printf("io test user\n");
    _canvas->setFont(&fonts::Font0);
    _canvas->fillRect(0, 0, 240, 135, _canvas->color565(0, 0, 0));

    char string_buffer[24];
    uint32_t time_count = 0;
    while (1)
    {
        if (millis() - time_count > 200)
        {
            _canvas->clear();

            _canvas->fillRect(0, 0, 240, 25, _canvas->color565(20, 20, 20));
            _canvas->setTextSize(2);
            _canvas->setTextColor(TFT_WHITE);
            snprintf(string_buffer, 20, "Bat Voltage");
            _canvas->drawCenterString(string_buffer, _canvas->width() / 2, 5);

            float v_bat = (float)analogReadMilliVolts(10) * 2 / 1000;

            _canvas->setTextSize(5);
            _canvas->setTextColor(TFT_SKYBLUE);
            snprintf(string_buffer, 20, "%.2fV", v_bat);
            _canvas->drawCenterString(string_buffer, _canvas->width() / 2, 55);

            _canvas_update();

            time_count = millis();
        }

        if (_check_next())
        {
            break;
        }
    }
}
