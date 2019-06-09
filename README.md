# Setup Environment for Kendryte K210 on Ubuntu

1. Tải 2 file **kendryte-toolchain** và **kendryte-openocd** theo link này [https://kendryte.com/downloads/](https://kendryte.com/downloads/)
2. Cài gói thư viện usb `sudo apt-get install libusb-dev -y`

## Cài đặt toolchain

- Tải toolchain theo đường dẫn này: [https://kendryte.com/downloads/](https://kendryte.com/downloads/)
- Giải nén vào thư mục **/opt/**
- *$* `sudo tar -xvf kendryte-toolchain-ubuntu-amd64-8.2.0-20190213.tar.gz -C /opt/`
- Mở file **.bashrc** bằng **Visual Studio Code**:
- *$* `code ~/.bashrc` rồi thềm dòng bên dưới ở cuối file
- `export PATH=$PATH:/opt/kendryte-toolchain/bin`
- *$* `source  ~/.bashrc` update lại file **.bashrc**
- *$* `riscv64-unknown-elf-gcc -v` kiểm tra version

## Cài đặt OpenOCD

- Tải toolchain theo đường dẫn này: [https://kendryte.com/downloads/](https://kendryte.com/downloads/)
- Tạo thư mục với tên **kendryte**:
- *$* `mkdir kendryte`
- Giải nén file *OpenOCD* tải về vào thư mục **kendryte** mới tạo:
- *$* `tar -xvf kendryte-openocd-0.1.3-ubuntu64.tar.gz -C ~/working/kendryte/`
- Kiểm tra version:
- *$* `cd kendryte`
- *$* `/bin/openocd -v`

## Cài đặt Flash tool

- Cài đặt kflash bằng lệnh:
- *$* `sudo pip3 install kflash`
- Xem thêm thông tin và cách sử dụng: [https://github.com/kendryte/kflash.py](https://github.com/kendryte/kflash.py)

## Cài đặt SDK

- Standalone SDK *(nên đặt trong thư mục kendryte đã tạo)*:
- *kendryte$* `git clone https://github.com/kendryte/kendryte-standalone-sdk.git`
- FreeRTOS SDK *(nên đặt trong thư mục kendryte đã tạo)*:
- *kendryte$* `git clone https://github.com/kendryte/kendryte-freertos-sdk.git`

## Build code

- Build code mẫu với kendryte-standalone-sdk:
- `cd kendryte-standalone-sdk/`
- Tạo thư mục  để build code:
- `mkdir build && cd build`
- Build code:
- `cmake .. -DPROJ=hello_world`
- `make -j8`
- Nạp code:
- `sudo chmod 777 /dev/ttyUSB0`
- `kflash -B dan hello_world.bin` lệnh này chỉ nạp code
- `kflash -B dan -t hello_world.bin` lệnh này nạp code xong sẽ mở cổng serial để debug
