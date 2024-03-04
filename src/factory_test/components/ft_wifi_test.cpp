/**
 * @file wifi_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../factory_test.h"

static bool wifi_test_done = false;
static int wifi_test_RSSI = 0;
static String wifi_test_result = "null";
static int wifi_best_RSSI = 0;
static int scan_num = 0;

#define WIFI_PASS_RSSI_TH -65
#define WIFI_TEST_IS_PASS() wifi_test_result == "ok"

const char* wifi_ssid = "UDUDLRLRBABA";
const char* wifi_password = "114514";

void task_wifi(void* param)
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected.
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    /* ------------------------------------- Scan test ------------------------------------- */
    /* Wifi scan and RSSI test */
    wifi_test_result = "null";
    wifi_best_RSSI = 0;

    printf("Wifi scan start\n");
    int n = WiFi.scanNetworks();
    scan_num = n;
    if (n == 0)
    {
        wifi_test_result = "Scan error";
    }
    else
    {
        /* Reset test rssi */
        wifi_best_RSSI = WiFi.RSSI(0);
        for (int i = 0; i < n; ++i)
        {
            /* Get max rssi */
            if (WiFi.RSSI(i) > wifi_best_RSSI)
                wifi_best_RSSI = WiFi.RSSI(i);
        }
        /* If not pass */
        printf("Best RSSI: %d\n", wifi_best_RSSI);
        if (wifi_best_RSSI < WIFI_PASS_RSSI_TH)
        {
            printf("Low RSSI\n");
            wifi_test_result = "Low RSSI: ";
            wifi_test_result += wifi_best_RSSI;
        }
    }
    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();

    if (wifi_test_result != "null")
    {
        delay(1000);
        wifi_test_done = true;
        vTaskDelete(NULL);
    }

    /* ------------------------------------- Scan test ------------------------------------- */

    /* ------------------------------------- Connectoin test ------------------------------------- */
    WiFi.begin(wifi_ssid, wifi_password);

    /* Wait for the connection */
    uint32_t time_conut = millis();
    printf("Connecting to %s\n", wifi_ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        printf("...\n");
        /* If time out */
        if ((millis() - time_conut) > 10000)
        {
            wifi_test_result = "Connect wifi timeout";
            break;
        }
    }
    /* If connected */
    if (wifi_test_result == "null")
    {

        /* Setup http client */
        HTTPClient http;

        printf("[HTTP] begin...\n");

        // configure traged server and url
        http.begin("http://example.com/index.html");

        // start connection and send HTTP header
        int httpCode = http.GET();
        // httpCode will be negative on error
        if (httpCode > 0)
        {
            // HTTP header has been send and Server response header has been handled
            printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK)
            {
                String payload = http.getString();
                printf(payload.c_str());

                printf("\n[HTTP] HTTP test pass\n");
                wifi_test_result = "ok";
            }
            else
            {
                printf("[HTTP] HTTP code not ok\n");
                wifi_test_result = "Http error";
            }
        }
        else
        {
            printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());

            wifi_test_result = "Http get failed";
        }

        // http.end();
        // http.~HTTPClient();
    }
    else
    {
        WiFi.disconnect(true);

        wifi_test_done = true;
        // delay(1000);
        vTaskDelete(NULL);
    }

    // wifi_test_result = "ok";

    // WiFi.mode(WIFI_OFF);
    // WiFi.disconnect(true);

    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_wifi_clear_ap_list();

    /* ------------------------------------- Connectoin test ------------------------------------- */

    wifi_test_done = true;
    // delay(1000);
    vTaskDelete(NULL);
}

void FactoryTest::_wifi_start_test_task()
{

    /* Craete task */
    printf("wifi task init\n");
    TaskHandle_t _task_wifi_handler = NULL;
    xTaskCreate(task_wifi, "wifi", 4096, NULL, 5, &_task_wifi_handler);
    if (_task_wifi_handler == NULL)
    {
        printf("wifi task create failed\n");
        return;
    }
    printf("wait result\n");
}

void FactoryTest::_wifi_test()
{
    printf("wifi test\n");

    if (_is_test_mode)
    {
        // /* Craete task */
        // printf("wifi task init\n");
        // TaskHandle_t _task_wifi_handler = NULL;
        // xTaskCreate(task_wifi, "wifi", 4096, NULL, 5, &_task_wifi_handler);
        // if (_task_wifi_handler == NULL) {
        //     printf("wifi task create failed\n");
        //     return;
        // }
        // printf("wait result\n");

        /* Display */
        _canvas->fillScreen(TFT_BLACK);
        _canvas->setFont(&fonts::efontCN_24);
        _canvas->setTextSize(1);
        _canvas->setTextColor(TFT_WHITE, TFT_BLACK);

        while (1)
        {
            _canvas->fillScreen(TFT_BLACK);

            if (wifi_test_done)
            {
                _canvas->setCursor(0, 10);
                _canvas->setFont(&fonts::Font0);
                _canvas->printf(" %s\n %s\n", wifi_ssid, wifi_password);
                _canvas->setFont(&fonts::efontCN_24);

                _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
                _canvas->printf(" [Wifi测试]\n");

                _canvas->setTextColor((wifi_test_result == "ok") ? TFT_GREEN : TFT_RED);
                _canvas->printf(" 扫描个数: %d\n 结果: %s\n", scan_num, wifi_test_result.c_str());

                _canvas_update();

                _check_encoder();
                if (_check_next())
                {
                    break;
                }
            }

            else
            {
                _canvas->setCursor(0, 10);
                _canvas->setFont(&fonts::Font0);
                _canvas->printf(" %s\n %s\n", wifi_ssid, wifi_password);
                _canvas->setFont(&fonts::efontCN_24);

                _canvas->setTextColor(TFT_YELLOW, TFT_BLACK);
                _canvas->printf(" [Wifi测试]\n");

                _canvas->setTextColor(TFT_BLUE, TFT_BLACK);
                _canvas->printf(" 等待测试结果...\n");
            }

            _canvas_update();
            delay(200);
        }
    }

    else
    {
        printf("wifi scan\n");

        _canvas->fillScreen((uint32_t)0xF6A4A4);
        _canvas->setFont(&fonts::Font0);
        _canvas->setTextSize(2);
        // _canvas->setTextSize(1);
        _canvas->setTextColor((uint32_t)0xF6A4A4);
        _canvas->setCursor(0, 6);
        _canvas->fillRect(0, 0, 240, 25, (uint32_t)0x762424);
        _canvas->printf("      WiFi Scan\n");

        _canvas->setFont(&fonts::efontCN_16_b);
        _canvas->setTextScroll(false);
        _canvas->setTextSize(1);
        _canvas->setCursor(0, 28);
        _canvas->setTextColor((uint32_t)0x762424);
        _canvas->printf(" Scanning...\n");
        _canvas_update();
        _canvas->setCursor(0, 28);

        int wifi_num = 0;
        // if (!_wifi_tested)
        {
            // _wifi_tested = true;

            // Set WiFi to station mode and disconnect from an AP if it was previously connected.
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            delay(100);

            /* ------------------------------------- Scan test ------------------------------------- */
            printf("Wifi scan start\n");
            int n = WiFi.scanNetworks();
            wifi_num = n;
            if (n == 0)
            {
                printf("scan error\n");
            }
            else
            {
                for (int i = 0; i < 6; ++i)
                {
                    // display->setTextColor((WiFi.RSSI(i) > -75) ? TFT_GREEN : TFT_RED, TFT_BLACK);
                    // display->printf(" %s ", WiFi.SSID(i).c_str());
                    // display->printf(" %d\n", WiFi.RSSI(i));
                    // displayUpdate();

                    _wifi_list[i].rssi = WiFi.RSSI(i);
                    _wifi_list[i].ssid = WiFi.SSID(i);

                    switch (WiFi.encryptionType(i))
                    {
                    case WIFI_AUTH_OPEN:
                        _wifi_list[i].ssid += " (open)";
                        break;
                    case WIFI_AUTH_WEP:
                        _wifi_list[i].ssid += " (WEP)";
                        break;
                    case WIFI_AUTH_WPA_PSK:
                        _wifi_list[i].ssid += " (WPA)";
                        break;
                    case WIFI_AUTH_WPA2_PSK:
                        _wifi_list[i].ssid += " (WPA2)";
                        break;
                    case WIFI_AUTH_WPA_WPA2_PSK:
                        _wifi_list[i].ssid += " (WPA+WPA2)";
                        break;
                    case WIFI_AUTH_WPA2_ENTERPRISE:
                        _wifi_list[i].ssid += " (WPA2-EAP)";
                        break;
                    case WIFI_AUTH_WPA3_PSK:
                        _wifi_list[i].ssid += " (WPA3)";
                        break;
                    case WIFI_AUTH_WPA2_WPA3_PSK:
                        _wifi_list[i].ssid += " (WPA2+WPA3)";
                        break;
                    case WIFI_AUTH_WAPI_PSK:
                        _wifi_list[i].ssid += " (WAPI)";
                        break;
                    default:
                        break;
                    }
                }
            }
            // Delete the scan result to free memory for code below.
            WiFi.scanDelete();

            WiFi.disconnect(true);
            esp_wifi_disconnect();
            esp_wifi_stop();
            esp_wifi_deinit();
            esp_wifi_clear_ap_list();
        }

        /* Display result */
        if (wifi_num > 6)
            wifi_num = 6;
        for (int i = 0; i < wifi_num; ++i)
        {
            _canvas->setTextColor((_wifi_list[i].rssi > -75) ? TFT_DARKGREEN : TFT_BROWN, (uint32_t)0xF6A4A4);

            _canvas->printf("%d", _wifi_list[i].rssi);
            _canvas->printf(" %s\n", _wifi_list[i].ssid.c_str());
            _canvas_update();

            // _wifi_list[i].rssi = WiFi.RSSI(i);
            // _wifi_list[i].ssid = WiFi.SSID(i);
        }

        while (1)
        {
            _check_encoder();
            if (_check_next())
            {
                break;
            }
            // delay(10);
        }
    }

    printf("quit wifi test\n");
}
