#include "img_init.h"
#include "lvgl/lvgl.h"
#include <stdio.h>
#include <errno.h>

LV_IMG_DECLARE(img_temperature);
LV_IMG_DECLARE(img_humidity);
LV_IMG_DECLARE(img_pressure);
LV_IMG_DECLARE(img_cloudy);

void lv_image_init(void)
{
    lv_obj_t * image_temperature = lv_img_create(lv_scr_act(), NULL); 
    lv_img_set_src(image_temperature, &img_temperature);
    lv_obj_set_pos(image_temperature, 10, 5);
    lv_obj_set_drag(image_temperature, true);

    lv_obj_t * image_humidity = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(image_humidity, &img_humidity);
    lv_obj_set_pos(image_humidity, 170, 5);
    lv_obj_set_drag(image_humidity, true);

    lv_obj_t * image_pressure = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(image_pressure, &img_pressure);
    lv_obj_set_pos(image_pressure, 10, 125);
    lv_obj_set_drag(image_pressure, true);

    lv_obj_t * image_cloudy = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(image_cloudy, &img_cloudy);
    lv_obj_set_pos(image_cloudy, 170, 125);
    lv_obj_set_drag(image_cloudy, true);
}
