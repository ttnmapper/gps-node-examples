# stm32flash

stm32flash (https://sourceforge.net/projects/stm32flash/) with some fixes, including:
* support for manually erasing more than 255 flash pages
* support for writing/reading EEPROM on STM32L0 and STM32L1
* support for erasing necessary flash pages only with -E option

From here,
https://github.com/unwireddevices/stm32flash

I added the RAK811 STM32L device that I have in dev_table as it did not support page erase 

To program flash memory:
stm32flash -b 230400 -E -w file.hex COMxx

To program EEPROM memory:
stm32flash -b 230400 -S 0x08080000 -w file.bin COMxx

P.S. Default UART baud rate is 115200 bps. Speeds up to 921600 bps were tested with CP2102 based USB-UART adapter. Even higher 3000000 bps speed should be possible with FT232R, althought it is not guaranteed by AN3155 that speed over 115200 bps will work.
