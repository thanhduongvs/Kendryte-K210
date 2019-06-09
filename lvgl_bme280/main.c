/* Copyright 2018 Canaan Inc.
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
#include <stdio.h>
#include "bsp.h"
#include "fpioa.h"
#include "lcd.h"
#include "sysctl.h"
#include "nt35310.h"
#include "board_config.h"
#include "ns2009.h"
#include "tscal.h"
#include "sleep.h"

#include "lvgl.h"

#include "img_init.h"
#include "lv_font_builtin.h"

#include "drv_bme280.h"
#include "drv_i2c.h"

uint32_t g_lcd_gram[LCD_X_MAX * LCD_Y_MAX / 2] __attribute__((aligned(128)));

//LV_FONT_DECLARE(lv_font_dejavu_30);        /*Full ASCII range*/
//LV_FONT_DECLARE(lv_font_symbol_30);   /*Continuous block of cyrillic characters*/


static void io_set_power(void)
{
#if BOARD_LICHEEDAN
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
#else
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
#endif
}

static void io_mux_init(void)
{
#if BOARD_LICHEEDAN
    /**
    *lcd_cs	    36
    *lcd_rst	37
    *lcd_dc	    38
    *lcd_wr 	39
    * */
    fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);
    fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(36, FUNC_SPI0_SS3);
    fpioa_set_function(39, FUNC_SPI0_SCLK);

    fpioa_set_function(30, FUNC_I2C0_SCLK);
    fpioa_set_function(31, FUNC_I2C0_SDA);

    sysctl_set_spi0_dvp_data(1);
#else
    fpioa_set_function(8, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(6, FUNC_SPI0_SS3);
    fpioa_set_function(7, FUNC_SPI0_SCLK);
    sysctl_set_spi0_dvp_data(1);
#endif
}

void sysmon_task(void * param)
{
}

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    lcd_draw_picture_by_half((uint16_t)x1, (uint16_t)y1, (uint16_t)(x2 - x1 + 1), (uint16_t)(y2 - y1 + 1), (uint16_t *)color_p);
    lv_flush_ready();
}

void box_init(void)
{
    /* set backgroung color */
    static lv_style_t style_shadow;
    lv_style_copy(&style_shadow, &lv_style_transp);
    style_shadow.body.shadow.width = 6;
    style_shadow.body.radius = 5;

    /*Create a simple base object*/
    lv_obj_t * boxTemperature;
    boxTemperature = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(boxTemperature, 150, 110);
    lv_obj_set_style(boxTemperature, &style_shadow);
    lv_obj_align(boxTemperature, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 5);

    lv_obj_t * boxHumidity;
    boxHumidity = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(boxHumidity, 150, 110);
    lv_obj_set_style(boxHumidity, &style_shadow);
    lv_obj_align(boxHumidity, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 5);

    lv_obj_t * boxPressure;
    boxPressure = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(boxPressure, 150, 110);
    lv_obj_set_style(boxPressure, &style_shadow);
    lv_obj_align(boxPressure, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5);

    lv_obj_t * boxCloudy;
    boxCloudy = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(boxCloudy, 150, 110);
    lv_obj_set_style(boxCloudy, &style_shadow);
    lv_obj_align(boxCloudy, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5);
}

void lable_init(void)
{
    
    /*Create label on the screen. By default it will inherit the style of the screen*/
    lv_obj_t * titleTem = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(titleTem, "Temperature\nMin: 20.5 C\nMax: 30.5 C");
    lv_obj_set_pos(titleTem, 10, 50);

    lv_obj_t * titleHum = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(titleHum, "Humidity\nMin: 65.5 %\nMax: 75.5 %");
    lv_obj_set_pos(titleHum, 170, 50);

    lv_obj_t * titlePre = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(titlePre, "Pressure\nMin: 990.5mPa\nMax: 1502.5mPa");
    lv_obj_set_pos(titlePre, 10, 170);

    lv_obj_t * titleWea = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(titleWea, "Weather\nStation");
    lv_obj_set_pos(titleWea, 180, 170);
}

/* label display value */
lv_obj_t * lblTem;
lv_obj_t * lblHum;
lv_obj_t * lblPre;

void value_init(void)
{   

    /*Concatenate the fonts into one*/
    lv_font_add(&lv_font_dejavu_30, &lv_font_symbol_30);

    /*Create a style and use the new font*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    /*Set the base font whcih is concatenated with the others*/
    style.text.font = &lv_font_dejavu_30;
    style.text.color = LV_COLOR_HEX(0x2170a5);

    lblTem = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(lblTem, 60, 15);
    lv_label_set_style(lblTem, &style);
    //lv_label_set_long_mode(lblTem, LV_LABEL_LONG_ROLL);
    lv_label_set_text(lblTem, "0.0 C");

    lblHum = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(lblHum, 210, 15);
    lv_label_set_style(lblHum, &style);
    lv_label_set_text(lblHum, "0.0 %");

    lblPre = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(lblPre, 60, 125);
    lv_label_set_style(lblPre, &style);
    lv_label_set_text(lblPre, "0.0 pa");

}

void value_update(float temperature, float humidity, float pressure)
{   
    char buffer [10];

    sprintf (buffer, "%.2f C", temperature);
    lv_label_set_text(lblTem, buffer);

    sprintf (buffer, "%.2f %%", humidity);
    lv_label_set_text(lblHum, buffer);

    sprintf (buffer, "%.2f Pa", pressure/1000);
    lv_label_set_text(lblPre, buffer);

}

void do_lvgl_init(void)
{
    lcd_init();
#if BOARD_LICHEEDAN
    lcd_set_direction(DIR_YX_LRUD);
#endif
    lcd_clear(WHITE);
    lcd_draw_string(0, 0, "LVGL Test ", RED);
    msleep(500);

    lv_task_t * refr_task;

    //lv_init
    lv_init();

    //dis
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv); //Basic initialization
    disp_drv.disp_flush = my_disp_flush;
    lv_disp_drv_register(&disp_drv);

    refr_task = lv_task_create(sysmon_task, 500, LV_TASK_PRIO_LOW, NULL);

    lv_image_init();

    box_init();

    lable_init();

    value_init();
    
}

int main(void)
{

    float pressure, temperature, humidity;
    uint64_t core_id = current_coreid();

    if (core_id == 0)
    {
        printf("lvgl test\n");
        io_mux_init();
        

        i2c_mux_init();

        io_set_power();

        plic_init();
        sysctl_enable_irq();
        i2c_master_init();
        
        bme280_init_params();
        bme280_init();

        do_lvgl_init();
        while (1)
        {
            lv_task_handler();
            lv_tick_inc(1);


            humidity = bme280_readFloatHumidity();
            pressure = bme280_readFloatPressure();
            temperature = bme280_readTempC();

            value_update(temperature, humidity, pressure);

            /*
            printf("Humidity: %.2f %%\n", humidity);
            printf("Pressure: %.2f Pa\n", pressure);
            printf("Temperature: %.2f C\n", temperature);
            printf("====================\n\n");
            */

            msleep(10);
            // ts_ns2009_poll();
            // msleep(ts_ns2009_pdata->interval);
        }
    }
    else
    {
        while (1)
        {
            // lv_tick_inc(1);
            // msleep(1);
        }
    }
    while (1);
}



/*
cd lvgl_bme280
mkdir build && cd build
cmake ../../.. -DPROJ=lvgl_bme280
make -j8

sudo chmod 777 /dev/ttyUSB0
kflash -B dan -t lvgl_bme280.bin
ctrl + ] to exit serial
*/

/*
cd bme280
mkdir build && cd build
cmake ../../.. -DPROJ=bme280
make -j8

sudo chmod 777 /dev/ttyUSB0
kflash -B dan -t bme280.bin
ctrl + ] to exit serial
*/