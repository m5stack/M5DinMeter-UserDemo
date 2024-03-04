/**
 * @file key_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../factory_test.h"

void FactoryTest::_power_on()
{
    /* Hold pwr pin */
    gpio_reset_pin((gpio_num_t)POWER_HOLD_PIN);
    pinMode(POWER_HOLD_PIN, OUTPUT);
    digitalWrite(POWER_HOLD_PIN, HIGH);
}

void FactoryTest::_power_off()
{
    _rtc.clearIRQ();
    _rtc.disableIRQ();

    _disp->fillScreen(TFT_BLACK);

    while ((!_btn_pwr.read()))
    {
        delay(100);
    }

    delay(200);

    printf("power off\n");
    digitalWrite(POWER_HOLD_PIN, 0);
    delay(10000);

    while (1)
    {
        delay(1000);
    }
}

void FactoryTest::_get_test_mode()
{
    _canvas->fillScreen(TFT_BLACK);
    _canvas->setFont(&fonts::Font0);
    _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
    _canvas->setTextSize(3);
    _canvas->setCursor(0, 40);
    _canvas->printf(" Factory Test\n");
    // _canvas->setTextSize(2);
    // _canvas->setCursor(0, 60);
    // _canvas->printf(" Release Button to start ");
    _canvas_update();

    /* Check if is test mode */
    _is_test_mode = false;

    uint16_t time_count = 0;
    while (!_btn_pwr.read())
    {
        time_count++;
        delay(10);

        if (time_count == 200)
        {
            printf("test mode\n");
            _is_test_mode = true;
            _tone(5000, 50);
            delay(100);
            _tone(5000, 50);
        }
    }

    delay(200);

    // _disp->fillScreen(TFT_RED);
    // while (1) {
    //     // _wait_next();

    //     _check_next();
    //     _check_encoder();
    // }
}

bool FactoryTest::_check_test_mode()
{
    _is_test_mode = false;
    int hold_time_count = 0;
    const int update_interval = 20;
    
    while (!_btn_pwr.read())
    {
        // If hold long enough
        delay(update_interval);
        hold_time_count += update_interval;
        if (hold_time_count > 800)
            _is_test_mode = true;

        if (_is_test_mode)
        {
            _tone(5000, 50);
            delay(100);
            _tone(5000, 50);

            _canvas->fillScreen(TFT_BLACK);
            _canvas->setTextColor(TFT_YELLOW);
            _canvas->setTextSize(1);
            _canvas->setFont(&fonts::efontCN_24);
            _canvas->setCursor(0, 10);
            _canvas->printf("[DinMeter] 测试固件\n%s (%s)\n\n松开按键进入测试模式", FW_VERISON, __DATE__);
            _canvas_update();

            while (!_btn_pwr.read())
            {
                delay(10);
            }
            return _is_test_mode;
        }
    }
    return false;
}

void FactoryTest::_key_init()
{
    _btn_pwr.begin();

    // while (1) {
    //     _wait_next();
    // }

    // while (1) {
    //     static int pos = 0;
    //     _encoder.tick();

    //     int newPos = _encoder.getPosition();
    //     if (pos != newPos) {
    //         printf("%d  %d\n", newPos, _encoder.getDirection());
    //         pos = newPos;
    //     }
    // }
}

void FactoryTest::_key_test() {}

void FactoryTest::_check_reboot() {}

// bool FactoryTest::_check_next(bool checkPowerOff)
// {
//     /* Double click */
//     if (_btn_pwr.pressed()) {
//         _tone(2500, 50);

//         for (int i = 0; i < 4; i++) {
//             delay(50);
//             if (_btn_pwr.pressed()) {
//                 _tone(2500, 50);
//                 printf("double\n");
//                 if (checkPowerOff) {
//                     _power_off();
//                 }
//             }
//         }

//         printf("not double\n");
//         return true;
//     }

//     return false;
// }

bool FactoryTest::_check_next(bool checkPowerOff)
{
    /* Long press power off */
    if (!_btn_pwr.read())
    {
        _tone(2500, 50);

        uint8_t time_count = 0;
        while (!_btn_pwr.read())
        {

            time_count++;
            if (time_count > 100)
            {
                _canvas->fillScreen(TFT_BLACK);
                _canvas->setFont(&fonts::Font0);
                _canvas->setTextColor(TFT_RED);
                _canvas->setTextSize(2);
                _canvas->drawCenterString("Release Button", _canvas->width() / 2, _canvas->height() / 2 - 20);
                _canvas->drawCenterString("To Power Off", _canvas->width() / 2, _canvas->height() / 2);
                _canvas_update();

                while (!_btn_pwr.read())
                {
                    delay(10);
                }
                _power_off();
            }

            delay(10);
        }

        printf("not long pressed\n");
        return true;
    }

    return false;
}

void FactoryTest::_wait_next()
{
    while (!_check_next())
    {
        _check_encoder();
        _check_reboot();
        delay(10);
    }
    printf("go next\n");
}

bool FactoryTest::_check_encoder(bool playBuzz)
{
    // _enc.tick();
    // delay(100);
    // printf("%d\n", _enc.getPosition());

    if (_enc_pos != _enc.getPosition())
    {
        /* Bi */
        if (playBuzz)
        {
            _noTone();
            // _tone(((_enc.getDirection() == RotaryEncoder::Direction::CLOCKWISE) ? 3000 : 3500), 20);
            _tone(((_enc.getPosition() > _enc_pos) ? 3000 : 3500), 20);
            // delay(20);
        }

        _enc_pos = _enc.getPosition();
        // printf("%d\n", _enc_pos);

        return true;
    }

    return false;
}

void FactoryTest::_encoder_test()
{
    printf("encoder test\n");

    /* Display */
    _canvas->fillScreen(TFT_BLACK);
    _canvas->setFont(&fonts::efontCN_24);
    _canvas->setTextSize(1);
    _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);

    _canvas->setCursor(0, 0);
    _canvas->printf(" [编码器测试]\n");
    _canvas->setTextColor(TFT_WHITE);
    _canvas->printf(" 转到左边数字\n 然后按下按钮\n");

    _canvas->setTextSize(1.5);

    _enc_pos = 0;
    _enc.setPosition(_enc_pos);

    int8_t target_num_list[] = {-2, -10, 6, 15};

    for (int i = 0; i < 4; i++)
    {

        while (1)
        {

            _check_encoder();

            _canvas->setCursor(0, 90);
            _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
            _canvas->printf(" > %d    ", target_num_list[i]);
            _canvas->setTextColor(((target_num_list[i] == _enc_pos) ? TFT_GREEN : TFT_RED), TFT_BLACK);
            _canvas->printf("%d    ", _enc_pos);

            if (_check_next())
            {
                if (target_num_list[i] == _enc_pos)
                {
                    break;
                }
            }

            _canvas_update();
        }
    }

    _canvas->setFont(&fonts::Font0);

    printf("quit encoder test\n");
}

void FactoryTest::_encoder_test_new()
{
    printf("encoder test\n");

    /* Display */
    _canvas->fillScreen(TFT_BLACK);
    _canvas->setFont(&fonts::efontCN_24);
    _canvas->setTextSize(1);
    _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
    _canvas->setCursor(0, 0);
    _canvas->printf(" [编码器测试]\n");
    _canvas->setTextColor(TFT_WHITE);
    _canvas->printf(" 转到左边数字\n");
    _canvas->setTextSize(1.5);

    _enc_pos = 0;
    _enc.setPosition(_enc_pos);

    int8_t target_num_list[] = {-6, 6};

    for (int i = 0; i < 2; i++)
    {

        while (1)
        {

            _check_encoder();

            _canvas->setCursor(0, 90);
            _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
            _canvas->printf(" > %d    ", target_num_list[i]);
            _canvas->setTextColor(((abs(_enc_pos) >= 6) ? TFT_GREEN : TFT_RED), TFT_BLACK);
            _canvas->printf("%d    ", _enc_pos);

            if (abs(_enc_pos) >= 6)
            {
                _enc_pos = 0;
                _enc.setPosition(_enc_pos);
                break;
            }

            _canvas_update();
        }
    }

    _canvas->setFont(&fonts::Font0);
    printf("quit encoder test\n");
}

void FactoryTest::_encoder_test_user()
{
    printf("encoder test\n");

    _canvas->setFont(&fonts::Font0);

    char string_buffer[20];

    _enc_pos = 0;
    _enc.setPosition(_enc_pos);

    while (1)
    {
        _canvas->fillScreen((uint32_t)0x6AB8A0);

        _canvas->fillRect(0, 0, 240, 25, (uint32_t)0x163820);
        _canvas->setTextSize(2);
        _canvas->setTextColor((uint32_t)0x6AB8A0);
        snprintf(string_buffer, 20, "Encoder Test");
        _canvas->drawCenterString(string_buffer, _canvas->width() / 2, 5);

        _canvas->setTextSize(5);
        _canvas->setTextColor((uint32_t)0x163820);
        snprintf(string_buffer, 20, "%d", _enc_pos);
        _canvas->drawCenterString(string_buffer, _canvas->width() / 2, 55);

        _canvas_update();

        _check_encoder();
        if (_check_next())
        {
            break;
        }
    }

    printf("quit encoder test\n");
}
