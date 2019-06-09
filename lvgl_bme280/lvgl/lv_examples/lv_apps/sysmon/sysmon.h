/**
 * @file symon.h
 *
 */

#ifndef SYSMON_H
#define SYSMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../../lvgl/lvgl.h"
#include "../../../lv_ex_conf.h"
#endif
#if USE_LV_DEMO

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the system monitor
 */
void sysmon_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_SYSMON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SYSMON_H */
