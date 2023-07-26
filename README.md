<font size=7 >项目名称：基于OneNET服务器的LED阵列游戏的数字孪生系统</font>

**1.** **技术方案**

**1.1 外设与软件包的使用**

本项目使用到的外设有19灯等距全彩LED、4个轻触按键、240*240并行LCD和RW007高速wifi。需要对板载设备驱动进行如下配置：

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/dfe4b3ec-0913-4316-a81f-45f886241e9c)


本项目使用到的软件包有RW007高速WiFi模块驱动、OneNET连接中国移动云平台以及它的依赖包cJSON和Paho MQTT。需要对RW007的引脚和OneNET的产品和设备信息进行如下配置：

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/f684cb00-396c-4e94-8cf3-2fa8accfcdf2)
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/2cb83435-f9b4-4b37-ac78-9aa498f1f8b6)

**1.2 程序框架**

如下图所示，本项目的目的为实现轻触按键和数字孪生对LCD与LED阵列的控制：

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/8c2ea4ab-d3b3-41c3-8d6d-aedb0219cdd9)

系统创新性：实现了PC端与开发板之间的软硬件同步协同，可以认为构建了单片机按钮功能的数字孪生模型

 

 

 

 

 

**1.3 功能设计**

①开发板上电后，利用LED阵列和LCD实现项目的欢迎动画展示：

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/00f27297-93bc-4b94-b06c-684d008f118b)
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/c8060ae4-bcd6-44c6-95bd-53d101c9ea94)


②欢迎结束后，进入项目的初始界面。我们设计的游戏为一款吃豆人游戏，闪烁的LED表示吃豆人当前所在的位置，亮着的红灯代表地图上还没吃到的豆子，而灭了的LED则代表该点的豆子已经被吃到。如下图所示，游戏一开始吃豆人位于正中间的LED上：
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/617c119e-4910-4732-b5b3-403987199ad3)
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/3c5a4866-4f46-45df-9b08-d88b930b865e)


打开RT-Thread Studio终端，显示WIFI已自动连接好、mqtt协议已初始化且onenet指针已分配好空间：

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/f37a8d70-b503-4a0c-86d2-712c1599fec4)


③如下图所示，可以通过OneNET云平台的数字可视化平台的四个虚拟按键以数字孪生的形式对吃豆人进行控制，也可以通过轻触按键进行控制，吃豆人的移动规则见1.6操作指南。：
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/279f9aca-f500-4a01-8785-18accf770fd4)
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/3549f177-c86d-4ffd-8e02-6096585d926f)


如下图所示，当控制吃豆人向左移动时，LCD显示LEFT，吃豆人移动到新的位置上，原位置的LED熄灭：
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/85ca6405-745c-4afc-887e-8113d53e72a5)


若通过OneNET云平台的数字可视化平台的四个虚拟按键以数字孪生的形式对吃豆人进行控制，终端上还会显示该操作：

![img](file:////Users/jojo/Library/Group%20Containers/UBF8T346G9.Office/msoclip1/01/clip_image026.gif)

④如下图所示，当吃豆人将地图上全部的豆子都吃完后，LED阵列和LCD将展示通关动画，并自动复位开启下一把游戏：

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/3332defd-6b09-4c13-a040-b5b858fc3c96)
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/aa69c1c7-ad8b-443c-8911-26d3961ef12d)

 

**1.4 代码设计**

**1.4.1 准备阶段**

包括头文件的包含、宏的定义、枚举设置以及用于指示的全局变量：

```c
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
#define PIN_LEFT        GET_PIN (C, 0)
#define PIN_DOWN        GET_PIN (C, 1)
#define PIN_RIGHT        GET_PIN (C, 4)
#define PIN_UP       GET_PIN (C, 5)

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
```

**1.4.2 线程入口函数**

①以命令行的设计形式，实现对OneNET云平台的数字可视化平台的四个虚拟按键下发指令的接收与响应：

```C
#ifdef FINSH_USING_MSH
#include <finsh.h>

/* onenet mqtt command response callback function */
static void onenet_cmd_rsp_cb (uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    LOG_D ("received data is %s ",recv_data);

    if  (strcmp (recv_data,"up") == 0)
    {
        rt_kprintf ("receive up command");
        key_press = 1;
        key_direction = EXTERN_KEY_UP;
        lcd_clear (WHITE);
        lcd_show_string (50, 104, 32, "UP");
    }

    if  (strcmp (recv_data,"down") == 0)
    {
        rt_kprintf ("receive down command");
        key_press = 1;
        key_direction = EXTERN_KEY_DOWN;
        lcd_clear (WHITE);
        lcd_show_string (50, 104, 32, "DOWN");
    }

    if  (strcmp (recv_data,"left") == 0)
    {
        rt_kprintf ("receive left command");
        key_press = 1;
        key_direction = EXTERN_KEY_LEFT;
        lcd_clear (WHITE);
        lcd_show_string (50, 104, 32, "LEFT");
    }

    if  (strcmp (recv_data,"right") == 0)
    {
        rt_kprintf ("receive right command");
        key_press = 1;
        key_direction = EXTERN_KEY_RIGHT;
        lcd_clear (WHITE);
        lcd_show_string (50, 104, 32, "RIGHT");
    }

    memset (recv_data,0,strlen (recv_data));
}

/* set the onenet mqtt command response callback function */
int onenet_set_cmd_rsp (int argc, char **argv)
{
    onenet_set_cmd_rsp_cb (onenet_cmd_rsp_cb);
    return 0;
}
MSH_CMD_EXPORT (onenet_set_cmd_rsp, set cmd response function);

#endif /* FINSH_USING_MSH */
```

②动态线程，通过按键消抖的方法来实现对轻触按键下发指令的接收与响应：

rt_thread_t key_press_thread;

 ```C
 rt_thread_t key_press_thread;
 
 static void key_press_entry ()
 {
     while  (1)
     {
         rt_thread_mdelay (10);
         if  (rt_pin_read (PIN_LEFT) == PIN_LOW)
         {
             rt_thread_mdelay (50);
             if  (rt_pin_read (PIN_LEFT) == PIN_LOW)
             {
                 key_press = 1;
                 key_direction = EXTERN_KEY_LEFT;
                 lcd_clear (WHITE);
                 lcd_show_string (50, 104, 32, "LEFT");
             }
         }
 
         else if  (rt_pin_read (PIN_DOWN) == PIN_LOW)
         {
             rt_thread_mdelay (50);
             if  (rt_pin_read (PIN_DOWN) == PIN_LOW)
             {
                 key_press = 1;
                 key_direction = EXTERN_KEY_DOWN;
                 lcd_clear (WHITE);
                 lcd_show_string (50, 104, 32, "DOWN");
             }
         }
 
         else if  (rt_pin_read (PIN_RIGHT) == PIN_LOW)
         {
             rt_thread_mdelay (50);
             if  (rt_pin_read (PIN_RIGHT) == PIN_LOW)
             {
                 key_press = 1;
                 key_direction = EXTERN_KEY_RIGHT;
                 lcd_clear (WHITE);
                 lcd_show_string (50, 104, 32, "RIGHT");
             }
         }
 
         else if  (rt_pin_read (PIN_UP) == PIN_LOW)
         {
             rt_thread_mdelay (50);
             if  (rt_pin_read (PIN_UP) == PIN_LOW)
             {
                 key_press = 1;
                 key_direction = EXTERN_KEY_UP;
                 lcd_clear (WHITE);
                 lcd_show_string (50, 104, 32, "UP");
             }
         }
     }
 }
 
 ```

③动态线程，实现没有按键按下时当前位置LED的闪烁、按键对LED阵列的控制逻辑以及游戏通关的判断和动画展示：

```c
rt_thread_t led_key_thread;

static void led_key_entry ()
{
    rt_thread_mdelay (7000);
    while  (1)
    {
        while  (!key_press)
        {
            Set_LEDColor (current_state, DARK);
            RGB_Reflash ();
            rt_thread_mdelay (500);
            Set_LEDColor (current_state, LT_RED);
            RGB_Reflash ();
            rt_thread_mdelay (500);
        }

        switch  (key_direction)
        {
        case EXTERN_KEY_LEFT:
        {
            Set_LEDColor (current_state, DARK);
            RGB_Reflash ();
            switch  (current_state)
            {
            case  (EXTERN_LED_0):
                current_state = EXTERN_LED_11;
                break;
            case  (EXTERN_LED_1):
                current_state = EXTERN_LED_0;
                break;
            case  (EXTERN_LED_2):
                current_state = EXTERN_LED_12;
                break;
            case  (EXTERN_LED_3):
                current_state = EXTERN_LED_13;
                break;
            case  (EXTERN_LED_4):
                current_state = EXTERN_LED_14;
                break;
            case  (EXTERN_LED_5):
                current_state = EXTERN_LED_6;
                break;
            case  (EXTERN_LED_6):
                current_state = EXTERN_LED_7;
                break;
            case  (EXTERN_LED_7):
                current_state = EXTERN_LED_8;
                break;
            case  (EXTERN_LED_12):
                current_state = EXTERN_LED_17;
                break;
            case  (EXTERN_LED_13):
                current_state = EXTERN_LED_18;
                break;
            case  (EXTERN_LED_14):
                current_state = EXTERN_LED_15;
                break;
            case  (EXTERN_LED_15):
                current_state = EXTERN_LED_8;
                break;
            case  (EXTERN_LED_16):
                current_state = EXTERN_LED_9;
                break;
            case  (EXTERN_LED_17):
                current_state = EXTERN_LED_10;
                break;
            case  (EXTERN_LED_18):
                current_state = EXTERN_LED_16;
                break;
            default:
                break;
            }
        }
            break;

        case EXTERN_KEY_DOWN:
        {
            Set_LEDColor (current_state, DARK);
            RGB_Reflash ();
            switch  (current_state)
            {
            case  (EXTERN_LED_0):
                current_state = EXTERN_LED_12;
                break;
            case  (EXTERN_LED_1):
                current_state = EXTERN_LED_2;
                break;
            case  (EXTERN_LED_2):
                current_state = EXTERN_LED_3;
                break;
            case  (EXTERN_LED_3):
                current_state = EXTERN_LED_4;
                break;
            case  (EXTERN_LED_8):
                current_state = EXTERN_LED_7;
                break;
            case  (EXTERN_LED_9):
                current_state = EXTERN_LED_8;
                break;
            case  (EXTERN_LED_10):
                current_state = EXTERN_LED_9;
                break;
            case  (EXTERN_LED_11):
                current_state = EXTERN_LED_17;
                break;
            case  (EXTERN_LED_12):
                current_state = EXTERN_LED_18;
                break;
            case  (EXTERN_LED_13):
                current_state = EXTERN_LED_15;
                break;
            case  (EXTERN_LED_14):
                current_state = EXTERN_LED_5;
                break;
            case  (EXTERN_LED_15):
                current_state = EXTERN_LED_6;
                break;
            case  (EXTERN_LED_17):
                current_state = EXTERN_LED_16;
                break;
            case  (EXTERN_LED_18):
                current_state = EXTERN_LED_15;
                break;
            default:
                break;
            }
        }
            break;

        case EXTERN_KEY_RIGHT:
        {
            Set_LEDColor (current_state, DARK);
            RGB_Reflash ();
            switch  (current_state)
            {
            case  (EXTERN_LED_0):
                current_state = EXTERN_LED_1;
                break;
            case  (EXTERN_LED_6):
                current_state = EXTERN_LED_5;
                break;
            case  (EXTERN_LED_7):
                current_state = EXTERN_LED_6;
                break;
            case  (EXTERN_LED_8):
                current_state = EXTERN_LED_15;
                break;
            case  (EXTERN_LED_9):
                current_state = EXTERN_LED_16;
                break;
            case  (EXTERN_LED_10):
                current_state = EXTERN_LED_17;
                break;
            case  (EXTERN_LED_11):
                current_state = EXTERN_LED_0;
                break;
            case  (EXTERN_LED_12):
                current_state = EXTERN_LED_2;
                break;
            case  (EXTERN_LED_13):
                current_state = EXTERN_LED_3;
                break;
            case  (EXTERN_LED_14):
                current_state = EXTERN_LED_4;
                break;
            case  (EXTERN_LED_15):
                current_state = EXTERN_LED_14;
                break;
            case  (EXTERN_LED_16):
                current_state = EXTERN_LED_18;
                break;
            case  (EXTERN_LED_17):
                current_state = EXTERN_LED_12;
                break;
            case  (EXTERN_LED_18):
                current_state = EXTERN_LED_13;
                break;
            default:
                break;
            }
        }
            break;

        case EXTERN_KEY_UP:
        {
            Set_LEDColor (current_state, DARK);
            RGB_Reflash ();
            switch  (current_state)
            {
            case  (EXTERN_LED_2):
                current_state = EXTERN_LED_1;
                break;
            case  (EXTERN_LED_3):
                current_state = EXTERN_LED_2;
                break;
            case  (EXTERN_LED_4):
                current_state = EXTERN_LED_3;
                break;
            case  (EXTERN_LED_5):
                current_state = EXTERN_LED_14;
                break;
            case  (EXTERN_LED_6):
                current_state = EXTERN_LED_15;
                break;
            case  (EXTERN_LED_8):
                current_state = EXTERN_LED_9;
                break;
            case  (EXTERN_LED_9):
                current_state = EXTERN_LED_10;
                break;
            case  (EXTERN_LED_12):
                current_state = EXTERN_LED_0;
                break;
            case  (EXTERN_LED_14):
                current_state = EXTERN_LED_13;
                break;
            case  (EXTERN_LED_15):
                current_state = EXTERN_LED_18;
                break;
            case  (EXTERN_LED_16):
                current_state = EXTERN_LED_17;
                break;
            case  (EXTERN_LED_17):
                current_state = EXTERN_LED_11;
                break;
            case  (EXTERN_LED_18):
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

        if  (!Get_LEDColor (0).R && !Get_LEDColor (1).R && !Get_LEDColor (2).R && !Get_LEDColor (3).R && !Get_LEDColor (4).R && !Get_LEDColor (5).R && !Get_LEDColor (6).R && !Get_LEDColor (7).R && !Get_LEDColor (8).R && !Get_LEDColor (9).R && !Get_LEDColor (10).R && !Get_LEDColor (11).R && !Get_LEDColor (12).R && !Get_LEDColor (13).R && !Get_LEDColor (14).R && !Get_LEDColor (15).R && !Get_LEDColor (16).R && !Get_LEDColor (17).R && !Get_LEDColor (18).R)
        {
            lcd_show_string (20, 108, 24, "LOVE! RT-Thread!");
            for  (int count_1 = 0; count_1 <= 2; count_1++)
            {
                switch  (count_1)
                {
                case 0:
                    led_matrix_fill (LT_RED);
                    break;
                case 1:
                    led_matrix_fill (LT_GREEN);
                    break;
                case 2:
                    led_matrix_fill (LT_BLUE);
                    break;
                default:
                    return;
                    break;
                }
                RGB_Reflash ();
                rt_thread_mdelay (100);

                for  (int j = EXTERN_LED_18; j > EXTERN_LED_0; j--)
                {
                    switch  (count_1)
                    {
                    case 0:
                        Set_LEDColor (j, DARK);
                        break;
                    case 1:
                        Set_LEDColor (j, DARK);
                        break;
                    case 2:
                        Set_LEDColor (j, DARK);
                        break;
                    default:
                        return;
                        break;
                    }
                    RGB_Reflash ();
                    rt_thread_mdelay (100);
                }
            }
            led_matrix_fill (LT_RED);
            RGB_Reflash ();
            current_state = EXTERN_LED_18;
        }
        key_press = 0;
        key_direction = 0;
    }
}
```



④动态线程，实现项目的欢迎动画展示：

```c
rt_thread_t led_matrix_thread;

static void led_matrix_entry ()
{
    lcd_show_string (20, 108, 24, "LOVE! RT-Thread!");
    for  (int count = 0; count <= 2; count++)
    {
        for  (int i = EXTERN_LED_0; i <= EXTERN_LED_18; i++)
        {
            switch  (count)
            {
            case 0:
                Set_LEDColor (i, LT_RED);
                break;
            case 1:
                Set_LEDColor (i, LT_GREEN);
                break;
            case 2:
                Set_LEDColor (i, LT_BLUE);
                break;
            default:
                return;
                break;
            }
            RGB_Reflash ();
            rt_thread_mdelay (100);
        }
    }
    led_matrix_fill (LT_WHITE);
    RGB_Reflash ();

    rt_thread_mdelay (1000);

    led_matrix_fill (LT_RED);
    RGB_Reflash ();
}
```



⑤动态线程，实现WIFI的自动连接、mqtt协议的初始化以及为onenet指针分配空间：

```C
rt_thread_t wifi_init_thread;

static void wifi_init_entry ()
{
    rt_wlan_connect ("P40Pro", "23456789");
    onenet_mqtt_init ();
    onenet_set_cmd_rsp_cb (onenet_cmd_rsp_cb);
}


```



**1.4.3 主函数**

对轻触按键外设进行配置，以及上述五个线程的创建与启动：

```C
int main (void)
{
    /* 设置 KEY0 引脚的模式为输入上拉模式 */
    rt_pin_mode (PIN_LEFT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode (PIN_DOWN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode (PIN_RIGHT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode (PIN_UP, PIN_MODE_INPUT_PULLUP);

    led_matrix_thread = rt_thread_create ("led matrix", led_matrix_entry, RT_NULL, 1024, 4, 20);
    if  (led_matrix_thread == RT_NULL)
    {
        rt_kprintf ("led matrix thread create failed!\n");
        return 0;
    }
    rt_thread_startup (led_matrix_thread);

    led_key_thread = rt_thread_create ("led key", led_key_entry, RT_NULL, 1024, 5, 20);
    if  (led_key_thread == RT_NULL)
    {
        rt_kprintf ("led key thread create failed!\n");
        return 0;
    }
    rt_thread_startup (led_key_thread);

    key_press_thread = rt_thread_create ("key press", key_press_entry, RT_NULL, 1024, 6, 20);
    if  (key_press_thread == RT_NULL)
    {
        rt_kprintf ("key press thread create failed!\n");
        return 0;
    }
    rt_thread_startup (key_press_thread);

    wifi_init_thread = rt_thread_create ("wifi init", wifi_init_entry, RT_NULL, 1024, 7, 20);
    if  (wifi_init_thread == RT_NULL)
    {
        rt_kprintf ("wifi init thread create failed!\n");
        return 0;
    }
    rt_thread_startup (wifi_init_thread);

    return 0;
}

```

**1.5 OneNET云平台的搭建**

如下图所示，设计了OneNET云平台的数字可视化平台界面，将开关的开启和关闭指令分别设置为left、down、right和up，通过多协议接入连接到开发板上。

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/3146b26b-88bf-4db2-a3d1-951eb9d83805)
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/7dd94cbb-5a08-4da5-a269-8229a81641d1)




 

**1.6 操作指南**

如下图所示，LED阵列使用的是SK6805 RGB三色LED, 由19个LED组成，驱动时使用串行信号。

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/7023fe8a-18be-415a-a091-96592c89a994)


LED阵列在开发板中的位置以及对应的ID号如下图所示：

![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/a5a46821-a12a-4a4b-b881-4e7e86281169)
![image](https://github.com/noSichuh/led_matrix_control/assets/104688149/208bf24a-5f8b-4321-a034-8012872597f3)


按键对LED阵列的控制方式如下表所示（X表示无法通过）：

| led  Direction | **LEFT** | **DOWN** | **RIGHT** | **UP** |
| -------------- | -------- | -------- | --------- | ------ |
| **0**          | **11**   | **12**   | **1**     | **X**  |
| **1**          | **0**    | **2**    | **X**     | **X**  |
| **2**          | **12**   | **3**    | **X**     | **1**  |
| **3**          | **13**   | **4**    | **X**     | **2**  |
| **4**          | **14**   | **X**    | **X**     | **3**  |
| **5**          | **6**    | **X**    | **X**     | **14** |
| **6**          | **7**    | **X**    | **5**     | **15** |
| **7**          | **8**    | **X**    | **6**     | **X**  |
| **8**          | **X**    | **7**    | **15**    | **9**  |
| **9**          | **X**    | **8**    | **16**    | **10** |
| **10**         | **X**    | **9**    | **17**    | **X**  |
| **11**         | **X**    | **17**   | **0**     | **X**  |
| **12**         | **17**   | **18**   | **2**     | **0**  |
| **13**         | **18**   | **14**   | **3**     | **X**  |
| **14**         | **15**   | **5**    | **4**     | **13** |
| **15**         | **8**    | **6**    | **14**    | **18** |
| **16**         | **9**    | **X**    | **18**    | **17** |
| **17**         | **10**   | **16**   | **12**    | **11** |
| **18**         | **16**   | **15**   | **13**    | **12** |

 

 

**2.**2. **人员分配**
何思成：OneNET云平台的数字可视化平台和程序接口的设计、以及多协议接入的准备
林扬航：源程序的设计，主要包括轻触按键的线程、LED阵列变化的线程等
张旭泽：资料收集、PPT展示、视频拍摄等

 

**3.** **时间规划**

7.22 项目功能的构思

7.23 OneNET模块、轻触按键模块和LED阵列模块的设计

7.24 不同模块间的整合与程序改进

7.25 视频拍摄、制作PPT、编写设计文档以及程序的开源

7.26 进行答辩展示

 

 
