# Hướng dẫn

Đặt thư mục **lvgl_bmme280** nằm trong thư mục **/kendryte-standalone-sdk/src**

## Build code

- `cd lvgl_bme280`
- `mkdir build && cd build`
- `cmake ../../.. -DPROJ=lvgl_bme280`
- `make -j8`

## Nạp code

- `sudo chmod 777 /dev/ttyUSB0`
- `kflash -B dan -t lvgl_bme280.bin`
- Gõ <kbd> Ctrl</kbd> + <kbd>]</kbd> để thoát khỏi terminal
