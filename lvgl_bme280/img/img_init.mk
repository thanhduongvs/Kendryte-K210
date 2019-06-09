CSRCS += img_init.c
CSRCS += img_temperature.c
CSRCS += img_humidity.c
CSRCS += img_pressure.c
CSRCS += img_cloudy.c

DEPPATH += --dep-path $(LVGL_DIR)/img
VPATH += :$(LVGL_DIR)/img

CFLAGS += "-I$(LVGL_DIR)/img"