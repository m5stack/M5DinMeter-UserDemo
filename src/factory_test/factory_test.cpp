/**
 * @file factory_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "factory_test.h"

void FactoryTest::init()
{
    _power_on();

    _rtc_init();

    _key_init();

    _disp_init();

    // Encoder init
    _enc.attachHalfQuad(40, 41);

    _enc.setCount(0);

    // if (_check_test_mode())
    // {
    //     start_factory_test();
    // }
}

void FactoryTest::start_factory_test()
{
    _wifi_start_test_task();
    _disp_test();
    _rtc_test();
    _encoder_test_new();
    _io_test();
    _wifi_test();
    _ble_test();
    _rtc_wakeup_test();
}
