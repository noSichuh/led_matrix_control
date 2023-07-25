/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <onenet.h>
#include <drv_matrix_led.h>
#include <drv_lcd.h>

#define DBG_ENABLE
#define DBG_COLOR
#define DBG_SECTION_NAME    "onenet.sample"
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 配置 KEY 输入引脚  */
#define PIN_LEFT        GET_PIN(C, 0)
#define PIN_DOWN        GET_PIN(C, 1)
#define PIN_RIGHT        GET_PIN(C, 4)
#define PIN_UP       GET_PIN(C, 5)

/* define LED  */
enum
{
    EXTERN_LED_0,
    EXTERN_LED_1,
    EXTERN_LED_2,
    EXTERN_LED_3,
    EXTERN_LED_4,
    EXTERN_LED_5,
    EXTERN_LED_6,
    EXTERN_LED_7,
    EXTERN_LED_8,
    EXTERN_LED_9,
    EXTERN_LED_10,
    EXTERN_LED_11,
    EXTERN_LED_12,
    EXTERN_LED_13,
    EXTERN_LED_14,
    EXTERN_LED_15,
    EXTERN_LED_16,
    EXTERN_LED_17,
    EXTERN_LED_18,
};

enum
{
    EXTERN_KEY_LEFT = 1, EXTERN_KEY_DOWN, EXTERN_KEY_RIGHT, EXTERN_KEY_UP,
};

int current_state = EXTERN_LED_18;
int key_press = 0;
int key_direction = 0;

#ifdef FINSH_USING_MSH
#include <finsh.h>

/* onenet mqtt command response callback function */
static void onenet_cmd_rsp_cb(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    LOG_D("received data is %s ",recv_data);

    if (strcmp(recv_data,"up") == 0)
    {
        rt_kprintf("receive up command");
        key_press = 1;
        key_direction = EXTERN_KEY_UP;
        lcd_clear(WHITE);
        lcd_show_string(50, 104, 32, "UP");
    }

    if (strcmp(recv_data,"down") == 0)
    {
        rt_kprintf("receive down command");
        key_press = 1;
        key_direction = EXTERN_KEY_DOWN;
        lcd_clear(WHITE);
        lcd_show_string(50, 104, 32, "DOWN");
    }

    if (strcmp(recv_data,"left") == 0)
    {
        rt_kprintf("receive left command");
        key_press = 1;
        key_direction = EXTERN_KEY_LEFT;
        lcd_clear(WHITE);
        lcd_show_string(50, 104, 32, "LEFT");
    }

    if (strcmp(recv_data,"right") == 0)
    {
        rt_kprintf("receive right command");
        key_press = 1;
        key_direction = EXTERN_KEY_RIGHT;
        lcd_clear(WHITE);
        lcd_show_string(50, 104, 32, "RIGHT");
    }

    memset(recv_data,0,strlen(recv_data));
}

/* set the onenet mqtt command response callback function */
int onenet_set_cmd_rsp(int argc, char **argv)
{
    onenet_set_cmd_rsp_cb(onenet_cmd_rsp_cb);
    return 0;
}
MSH_CMD_EXPORT(onenet_set_cmd_rsp, set cmd response function);

#endif /* FINSH_USING_MSH */

rt_thread_t key_press_thread;

static void key_press_entry()
{
    while (1)
    {
        rt_thread_mdelay(10);
        if (rt_pin_read(PIN_LEFT) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_LEFT) == PIN_LOW)
            {
                key_press = 1;
                key_direction = EXTERN_KEY_LEFT;
                lcd_clear(WHITE);
                lcd_show_string(50, 104, 32, "LEFT");
            }
        }

        else if (rt_pin_read(PIN_DOWN) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_DOWN) == PIN_LOW)
            {
                key_press = 1;
                key_direction = EXTERN_KEY_DOWN;
                lcd_clear(WHITE);
                lcd_show_string(50, 104, 32, "DOWN");
            }
        }

        else if (rt_pin_read(PIN_RIGHT) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_RIGHT) == PIN_LOW)
            {
                key_press = 1;
                key_direction = EXTERN_KEY_RIGHT;
                lcd_clear(WHITE);
                lcd_show_string(50, 104, 32, "RIGHT");
            }
        }

        else if (rt_pin_read(PIN_UP) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_UP) == PIN_LOW)
            {
                key_press = 1;
                key_direction = EXTERN_KEY_UP;
                lcd_clear(WHITE);
                lcd_show_string(50, 104, 32, "UP");
            }
        }
    }
}

rt_thread_t led_key_thread;

static void led_key_entry()
{
    rt_thread_mdelay(7000);
    while (1)
    {
        while (!key_press)
        {
            Set_LEDColor(current_state, DARK);
            RGB_Reflash();
            rt_thread_mdelay(500);
            Set_LEDColor(current_state, LT_RED);
            RGB_Reflash();
            rt_thread_mdelay(500);
        }

        switch (key_direction)
        {
        case EXTERN_KEY_LEFT:
        {
            Set_LEDColor(current_state, DARK);
            RGB_Reflash();
            switch (current_state)
            {
            case (EXTERN_LED_0):
                current_state = EXTERN_LED_11;
                break;
            case (EXTERN_LED_1):
                current_state = EXTERN_LED_0;
                break;
            case (EXTERN_LED_2):
                current_state = EXTERN_LED_12;
                break;
            case (EXTERN_LED_3):
                current_state = EXTERN_LED_13;
                break;
            case (EXTERN_LED_4):
                current_state = EXTERN_LED_14;
                break;
            case (EXTERN_LED_5):
                current_state = EXTERN_LED_6;
                break;
            case (EXTERN_LED_6):
                current_state = EXTERN_LED_7;
                break;
            case (EXTERN_LED_7):
                current_state = EXTERN_LED_8;
                break;
            case (EXTERN_LED_12):
                current_state = EXTERN_LED_17;
                break;
            case (EXTERN_LED_13):
                current_state = EXTERN_LED_18;
                break;
            case (EXTERN_LED_14):
                current_state = EXTERN_LED_15;
                break;
            case (EXTERN_LED_15):
                current_state = EXTERN_LED_8;
                break;
            case (EXTERN_LED_16):
                current_state = EXTERN_LED_9;
                break;
            case (EXTERN_LED_17):
                current_state = EXTERN_LED_10;
                break;
            case (EXTERN_LED_18):
                current_state = EXTERN_LED_16;
                break;
            default:
                break;
            }
        }
            break;

        case EXTERN_KEY_DOWN:
        {
            Set_LEDColor(current_state, DARK);
            RGB_Reflash();
            switch (current_state)
            {
            case (EXTERN_LED_0):
                current_state = EXTERN_LED_12;
                break;
            case (EXTERN_LED_1):
                current_state = EXTERN_LED_2;
                break;
            case (EXTERN_LED_2):
                current_state = EXTERN_LED_3;
                break;
            case (EXTERN_LED_3):
                current_state = EXTERN_LED_4;
                break;
            case (EXTERN_LED_8):
                current_state = EXTERN_LED_7;
                break;
            case (EXTERN_LED_9):
                current_state = EXTERN_LED_8;
                break;
            case (EXTERN_LED_10):
                current_state = EXTERN_LED_9;
                break;
            case (EXTERN_LED_11):
                current_state = EXTERN_LED_17;
                break;
            case (EXTERN_LED_12):
                current_state = EXTERN_LED_18;
                break;
            case (EXTERN_LED_13):
                current_state = EXTERN_LED_15;
                break;
            case (EXTERN_LED_14):
                current_state = EXTERN_LED_5;
                break;
            case (EXTERN_LED_15):
                current_state = EXTERN_LED_6;
                break;
            case (EXTERN_LED_17):
                current_state = EXTERN_LED_16;
                break;
            case (EXTERN_LED_18):
                current_state = EXTERN_LED_15;
                break;
            default:
                break;
            }
        }
            break;

        case EXTERN_KEY_RIGHT:
        {
            Set_LEDColor(current_state, DARK);
            RGB_Reflash();
            switch (current_state)
            {
            case (EXTERN_LED_0):
                current_state = EXTERN_LED_1;
                break;
            case (EXTERN_LED_6):
                current_state = EXTERN_LED_5;
                break;
            case (EXTERN_LED_7):
                current_state = EXTERN_LED_6;
                break;
            case (EXTERN_LED_8):
                current_state = EXTERN_LED_15;
                break;
            case (EXTERN_LED_9):
                current_state = EXTERN_LED_16;
                break;
            case (EXTERN_LED_10):
                current_state = EXTERN_LED_17;
                break;
            case (EXTERN_LED_11):
                current_state = EXTERN_LED_0;
                break;
            case (EXTERN_LED_12):
                current_state = EXTERN_LED_2;
                break;
            case (EXTERN_LED_13):
                current_state = EXTERN_LED_3;
                break;
            case (EXTERN_LED_14):
                current_state = EXTERN_LED_4;
                break;
            case (EXTERN_LED_15):
                current_state = EXTERN_LED_14;
                break;
            case (EXTERN_LED_16):
                current_state = EXTERN_LED_18;
                break;
            case (EXTERN_LED_17):
                current_state = EXTERN_LED_12;
                break;
            case (EXTERN_LED_18):
                current_state = EXTERN_LED_13;
                break;
            default:
                break;
            }
        }
            break;

        case EXTERN_KEY_UP:
        {
            Set_LEDColor(current_state, DARK);
            RGB_Reflash();
            switch (current_state)
            {
            case (EXTERN_LED_2):
                current_state = EXTERN_LED_1;
                break;
            case (EXTERN_LED_3):
                current_state = EXTERN_LED_2;
                break;
            case (EXTERN_LED_4):
                current_state = EXTERN_LED_3;
                break;
            case (EXTERN_LED_5):
                current_state = EXTERN_LED_14;
                break;
            case (EXTERN_LED_6):
                current_state = EXTERN_LED_15;
                break;
            case (EXTERN_LED_8):
                current_state = EXTERN_LED_9;
                break;
            case (EXTERN_LED_9):
                current_state = EXTERN_LED_10;
                break;
            case (EXTERN_LED_12):
                current_state = EXTERN_LED_0;
                break;
            case (EXTERN_LED_14):
                current_state = EXTERN_LED_13;
                break;
            case (EXTERN_LED_15):
                current_state = EXTERN_LED_18;
                break;
            case (EXTERN_LED_16):
                current_state = EXTERN_LED_17;
                break;
            case (EXTERN_LED_17):
                current_state = EXTERN_LED_11;
                break;
            case (EXTERN_LED_18):
                current_state = EXTERN_LED_12;
                break;
            default:
                break;
            }
        }
            break;

        default:
            break;
        }

        if (!Get_LEDColor(0).R && !Get_LEDColor(1).R && !Get_LEDColor(2).R && !Get_LEDColor(3).R && !Get_LEDColor(4).R && !Get_LEDColor(5).R && !Get_LEDColor(6).R && !Get_LEDColor(7).R && !Get_LEDColor(8).R && !Get_LEDColor(9).R && !Get_LEDColor(10).R && !Get_LEDColor(11).R && !Get_LEDColor(12).R && !Get_LEDColor(13).R && !Get_LEDColor(14).R && !Get_LEDColor(15).R && !Get_LEDColor(16).R && !Get_LEDColor(17).R && !Get_LEDColor(18).R)
        {
            lcd_show_string(24, 100, 16, "Fxxk");
            lcd_show_string(20, 108, 24, "LOVE! RT-Thread!");
            for (int count_1 = 0; count_1 <= 2; count_1++)
            {
                switch (count_1)
                {
                case 0:
                    led_matrix_fill(LT_RED);
                    break;
                case 1:
                    led_matrix_fill(LT_GREEN);
                    break;
                case 2:
                    led_matrix_fill(LT_BLUE);
                    break;
                default:
                    return;
                    break;
                }
                RGB_Reflash();
                rt_thread_mdelay(100);

                for (int j = EXTERN_LED_18; j > EXTERN_LED_0; j--)
                {
                    switch (count_1)
                    {
                    case 0:
                        Set_LEDColor(j, DARK);
                        break;
                    case 1:
                        Set_LEDColor(j, DARK);
                        break;
                    case 2:
                        Set_LEDColor(j, DARK);
                        break;
                    default:
                        return;
                        break;
                    }
                    RGB_Reflash();
                    rt_thread_mdelay(100);
                }
            }
            led_matrix_fill(LT_RED);
            RGB_Reflash();
            current_state = EXTERN_LED_18;
        }
        key_press = 0;
        key_direction = 0;
    }
}

rt_thread_t led_matrix_thread;

static void led_matrix_entry()
{
    lcd_show_string(24, 100, 16, "Fxxk");
    lcd_show_string(20, 108, 24, "LOVE! RT-Thread!");
    for (int count = 0; count <= 2; count++)
    {
        for (int i = EXTERN_LED_0; i <= EXTERN_LED_18; i++)
        {
            switch (count)
            {
            case 0:
                Set_LEDColor(i, LT_RED);
                break;
            case 1:
                Set_LEDColor(i, LT_GREEN);
                break;
            case 2:
                Set_LEDColor(i, LT_BLUE);
                break;
            default:
                return;
                break;
            }
            RGB_Reflash();
            rt_thread_mdelay(100);
        }
    }
    led_matrix_fill(LT_WHITE);
    RGB_Reflash();

    rt_thread_mdelay(1000);

    led_matrix_fill(LT_RED);
    RGB_Reflash();
}

rt_thread_t wifi_init_thread;

static void wifi_init_entry()
{
    rt_wlan_connect("P40Pro", "23456789");
    onenet_mqtt_init();
    onenet_set_cmd_rsp_cb(onenet_cmd_rsp_cb);
}

int main(void)
{
    /* 设置 KEY0 引脚的模式为输入上拉模式 */
    rt_pin_mode(PIN_LEFT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_DOWN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_RIGHT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_UP, PIN_MODE_INPUT_PULLUP);

    led_matrix_thread = rt_thread_create("led matrix", led_matrix_entry, RT_NULL, 1024, 4, 20);
    if (led_matrix_thread == RT_NULL)
    {
        rt_kprintf("led matrix thread create failed!\n");
        return 0;
    }
    rt_thread_startup(led_matrix_thread);

    led_key_thread = rt_thread_create("led key", led_key_entry, RT_NULL, 1024, 5, 20);
    if (led_key_thread == RT_NULL)
    {
        rt_kprintf("led key thread create failed!\n");
        return 0;
    }
    rt_thread_startup(led_key_thread);

    key_press_thread = rt_thread_create("key press", key_press_entry, RT_NULL, 1024, 6, 20);
    if (key_press_thread == RT_NULL)
    {
        rt_kprintf("key press thread create failed!\n");
        return 0;
    }
    rt_thread_startup(key_press_thread);

    wifi_init_thread = rt_thread_create("wifi init", wifi_init_entry, RT_NULL, 1024, 7, 20);
    if (wifi_init_thread == RT_NULL)
    {
        rt_kprintf("wifi init thread create failed!\n");
        return 0;
    }
    rt_thread_startup(wifi_init_thread);

    return 0;
}

