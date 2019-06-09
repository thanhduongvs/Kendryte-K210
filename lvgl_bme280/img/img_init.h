#ifndef _LV_IMAGES_H_
#define _LV_IMAGES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../../lvgl/lvgl.h"
#include "../../../lv_ex_conf.h"
#endif

void lv_image_init(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _LV_IMAGES_H_ */