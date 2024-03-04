/**
 * @file disp_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../factory_test.h"
#include "ft_disp_lgfx_cfg.hpp"

void FactoryTest::_disp_init()
{
    _disp = new LGFX_DinMeter;
    _disp->init();
    _disp->setRotation(3);

    _canvas = new LGFX_Sprite(_disp);
    _canvas->createSprite(_disp->width(), _disp->height());

    // _disp->fillScreen(TFT_RED);
    // _disp->setTextColor(TFT_WHITE);
    // _disp->printf("qweqweqweqwe\n");
}

void FactoryTest::_disp_test()
{
    if (_is_test_mode)
    {
        /* Frame test */
        _canvas->fillScreen(TFT_BLACK);
        _canvas->drawRect(0, 0, _canvas->width(), _canvas->height(), TFT_GREEN);
        _canvas->drawRect(1, 1, _canvas->width() - 2, _canvas->height() - 2, TFT_GREEN);
        _canvas->drawLine(0, 0, _canvas->width(), _canvas->height(), TFT_GREEN);
        _canvas->drawLine(0, _canvas->height(), _canvas->width(), 0, TFT_GREEN);

        _canvas_update();
        _wait_next();

        /* Color test */
        std::vector<int> color_list = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_BLACK, TFT_WHITE};
        for (auto i : color_list)
        {
            _canvas->fillScreen(i);
            _canvas_update();
            _wait_next();
        }
    }
    else
    {
        float color_r, color_g, color_b;

        color_r = 0;
        color_g = 0;
        color_b = 255;

        for (int i = 0; i < 384; i = i + 4)
        {
            if (i < 128)
            {
                color_r = i * 2;
                color_g = 0;
                color_b = 255 - (i * 2);
            }
            else if ((i >= 128) && (i < 256))
            {
                color_r = 255 - ((i - 128) * 2);
                color_g = (i - 128) * 2;
                color_b = 0;
            }
            else if ((i >= 256) && (i < 384))
            {
                color_r = 0;
                color_g = 255 - ((i - 256) * 2);
                ;
                color_b = (i - 256) * 2;
                ;
            }
            _canvas->fillRect(0, 0, 240, 135, _canvas->color565(color_r, color_g, color_b));
            _canvas_update();
        }
        
        // if (_check_test_mode())
        // {
        //     return;
        // }

        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
            case 0:
                color_r = 0;
                color_g = 0;
                color_b = 0;
                break;
            case 1:
                color_r = 255;
                color_g = 0;
                color_b = 0;
                break;
            case 2:
                color_r = 0;
                color_g = 255;
                color_b = 0;
                break;
            case 3:
                color_r = 0;
                color_g = 0;
                color_b = 255;
                break;
            }
            for (int n = 0; n < 240; n++)
            {
                color_r = (color_r < 255) ? color_r + 1.0625 : 255U;
                color_g = (color_g < 255) ? color_g + 1.0625 : 255U;
                color_b = (color_b < 255) ? color_b + 1.0625 : 255U;
                _canvas->drawLine(n, i * 33.75, n, (i + 1) * 33.75, _canvas->color565(color_r, color_g, color_b));
            }
        }
        _canvas_update();
        delay(500);

        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
            case 0:
                color_r = 255;
                color_g = 255;
                color_b = 255;
                break;
            case 1:
                color_r = 255;
                color_g = 0;
                color_b = 0;
                break;
            case 2:
                color_r = 0;
                color_g = 255;
                color_b = 0;
                break;
            case 3:
                color_r = 0;
                color_g = 0;
                color_b = 255;
                break;
            }
            for (int n = 0; n < 240; n++)
            {
                color_r = (color_r > 2) ? color_r - 1.0625 : 0U;
                color_g = (color_g > 2) ? color_g - 1.0625 : 0U;
                color_b = (color_b > 2) ? color_b - 1.0625 : 0U;
                _canvas->drawLine(239 - n, i * 33.75, 239 - n, (i + 1) * 33.75, _canvas->color565(color_r, color_g, color_b));
            }
        }
        _canvas_update();
        delay(500);
    }
}

void FactoryTest::_disp_set_brightness()
{
    printf("set brightness\n");

    _canvas->setFont(&fonts::Font0);

    int brightness = _disp->getBrightness();
    long old_position = _enc_pos;
    char string_buffer[20];

    _enc_pos = 0;
    _enc.setPosition(_enc_pos);

    while (1)
    {
        _canvas->fillScreen((uint32_t)0x87C38F);

        _canvas->fillRect(0, 0, 240, 25, (uint32_t)0x07430F);
        _canvas->setTextSize(2);
        _canvas->setTextColor((uint32_t)0x87C38F);
        snprintf(string_buffer, 20, "Set Brightness");
        _canvas->drawCenterString(string_buffer, _canvas->width() / 2, 5);

        _canvas->setTextSize(5);
        _canvas->setTextColor((uint32_t)0x07430F);
        snprintf(string_buffer, 20, "%d", brightness);
        _canvas->drawCenterString(string_buffer, _canvas->width() / 2, 55);

        _canvas_update();

        if (_check_encoder())
        {
            if (_enc_pos > old_position)
            {
                brightness += 5;

                printf("add\n");
            }
            else
            {
                brightness -= 5;

                printf("min\n");
            }

            if (brightness > 255)
            {

                brightness = 255;

                printf("hit top\n");
            }
            else if (brightness < 0)
            {
                brightness = 0;

                printf("hit bottom\n");
            }

            old_position = _enc_pos;
            _disp->setBrightness(brightness);
        }

        if (_check_next())
        {
            break;
        }
    }

    printf("quit set brightness\n");
}
