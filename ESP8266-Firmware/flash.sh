#!/bin/bash
sudo /opt/Espressif/esp-open-sdk/esptool/esptool.py --port /dev/ttyUSB0 write_flash 0x00000 ./bin/eagle.flash.bin 0x40000 ./bin/eagle.irom0text.bin
