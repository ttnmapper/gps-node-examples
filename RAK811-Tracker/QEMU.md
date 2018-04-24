# Running the RAK board in qeum
Clone and build this
https://github.com/Ebiroll/qemu-xtensa-esp32
Instructions are available in readme.md


# Start qemu

./qemu-system-arm   -serial file:uart1.log  -d unimp  -serial tcp::12344,server,nowait -serial tcp::12345,server,nowait -monitor stdio -machine rak811  -cpu cortex-m3
 -S -s  -pflash .pioenvs/rak811/firmware.bin


# Start debubgger
esport PATH=$PATH:~/.platformio/packages/toolchain-gccarmnoneeabi/bin/

/home/olas/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gdb  .pioenvs/rak811/firmware.elf -ex ' target remote:1234'


    (gdb) target extended-remote localhost:1234
    Press Ctrl-X then o
    (gdb) layout next
    (gdb) b SystemInit
    (gdb) monitor info mtree
    (gdb) b main
    (gdb) b HAL_Init

# Status
It starts and gets giong, emulation is not ass good as for stm32f2xx
The new files compared to the pebble emulation is called stm32l1xx

[compare makefile build with this](./parallell_run.md)

It would be nice to get debug printouts, need to investigate
   UART_WaitOnFlagUntilTimeout


# Layout

   (qemu) info mtree
   (gdb) monitor info mtree

    08000000-083fffff (prio 0, R-): f2xx.flash
    20000000-20007fff (prio 0, RW): armv7m.sram
    22000000-23ffffff (prio 0, RW): bitband
    40000000-4000009f (prio 0, RW): tim
    40000400-4000049f (prio 0, RW): tim
    40000800-4000089f (prio 0, RW): tim
    40000c00-40000c9f (prio 0, RW): tim
    40001000-4000109f (prio 0, RW): tim
    40001400-4000149f (prio 0, RW): tim
    40001800-4000189f (prio 0, RW): tim
    40001c00-40001c9f (prio 0, RW): tim
    40002000-4000209f (prio 0, RW): tim
    40002400-400027ff (prio 0, RW): dummy
    40002800-40002bfe (prio 0, RW): rtc
    40002c00-40002fff (prio 0, RW): dummy
    40003000-400033ff (prio 0, RW): dummy
    40003400-400037ff (prio 0, RW): dummy
    40003800-40003bfe (prio 0, RW): spi
    40003cd0-400040ce (prio 0, RW): spi
    40004000-400043ff (prio 0, RW): dummy
    40004400-400047fe (prio 0, RW): uart
    40004800-40004bfe (prio 0, RW): uart
    40004c00-40004ffe (prio 0, RW): uart
    40005000-400053fe (prio 0, RW): uart
    40005400-400057fe (prio 0, RW): i2c
    40005800-40005bfe (prio 0, RW): i2c
    40005c00-40005ffe (prio 0, RW): i2c
    40006000-400063ff (prio 0, RW): dummy
    40006c00-40006fff (prio 0, RW): dummy
    40007000-40007007 (prio 0, RW): pwr
    40007400-400077ff (prio 0, RW): dummy
    40007800-40007bff (prio 0, RW): dummy
    40008000-4000ffff (prio 0, RW): dummy
    40010000-400103ff (prio 0, RW): dummy
    40010400-400107ff (prio 0, RW): dummy
    40010400-400107fe (prio 0, RW): exti
    40011800-40011fff (prio 0, RW): dummy
    40012000-400123ff (prio 0, RW): adc
    40012c00-40012fff (prio 0, RW): dummy
    40013000-400133fe (prio 0, RW): spi
    40013800-40013bfe (prio 0, RW): uart
    40013800-40013bfe (prio 0, RW): syscfg
    40014000-4001409f (prio 0, RW): tim
    40014400-4001449f (prio 0, RW): tim
    40014800-4001489f (prio 0, RW): tim
    40020000-400203ff (prio 0, RW): gpio
    40020400-400207ff (prio 0, RW): gpio
    40020800-40020bff (prio 0, RW): gpio
    40020c00-40020fff (prio 0, RW): gpio
    40021000-400213ff (prio 0, RW): gpio
    40021400-400217ff (prio 0, RW): gpio
    40021800-40021bff (prio 0, RW): gpio
    40021c00-40021fff (prio 0, RW): gpio
    40022000-400223ff (prio 0, RW): gpio
    40023000-400233ff (prio 0, RW): crc
    40023800-40023bff (prio 0, RW): rcc
    40026000-400263ff (prio 0, RW): dma
    40026400-400267ff (prio 0, RW): dma
    42000000-43ffffff (prio 0, RW): bitband
    e000e000-e000efff (prio 0, RW): nvic
    e000e000-e000efff (prio 0, RW): nvic_sysregs




# From technical manual
```
0xA0000000 - 0xA0000FFF FSMC
0x50060000 - 0x500603FF AES Section 23.12.13: AES
0x40026400 - 0x400267FF DMA2 Section 11.4.7: DMA register
0x40026000 - 0x400263FF DMA1 Section 11.4.7: DMA register
0x40023C00 - 0x40023FFF FLASH Section 3.9.10: Register map
0x40023800 - 0x40023BFF RCC 
0x40023000 - 0x400233FF CRC Section 4.4.4: CRC register
0x40021C00 - 0x40021FFF GPIOG
0x40021800 - 0x40021BFF GPIOF
0x40021400 - 0x400217FF GPIOH
0x40021000 - 0x400213FF GPIOE
0x40020C00 - 0x40020FFF GPIOD
0x40020800 - 0x40020BFF GPIOC
0x40020400 - 0x400207FF GPIOB
0x40020000 - 0x400203FF GPIOA
0x40013800 - 0x40013BFF USART1 APB2
0x40013000 - 0x400133FF SPI1 Section 28.5.10: SPI register
0x40012C00 - 0x40012FFF SDIO Section 29.9.16: SDIO
0x40012400 - 0x400127FF ADC Section 12.15.21: ADC
0x40011000 - 0x400113FF TIM11 Section 14.4.17: TIMx
0x40010C00 - 0x40010FFF TIM10 Section 14.4.17: TIMx
0x40010800 - 0x40010BFF TIM9 Section 14.4.17: TIMx
0x40010400 - 0x400107FF EXTI Section 10.3.7: EXTI register
0x40010000 - 0x400103FF SYSCFG Section 8.5.7: SYSCFG
0x40007C00 - 0x40007C03 COMP APB1
0x40007C04 - 0x40007C5B RI Section 8.5.7: SYSCFG
0x40007C5C - 0x40007FFF OPAMP Section 15.4.4: OPAMP
0x40007400 - 0x400077FF DAC Section 13.5.15: DAC
0x40007000 - 0x400073FF PWR Section 5.4.3: PWR register
0x40006000 - 0x400063FF USB device FS SRAM (512 bytes Section 24.5.4: USB register)
0x40005C00 - 0x40005FFF USB device FS
0x40005800 - 0x40005BFF I2C2 Section 26.6.10: I2C register
map on page 694 
0x40005400 - 0x400057FF I2C1
0x40005000 - 0x400053FF USART5
0x40004C00 - 0x40004FFF USART4
0x40004800 - 0x40004BFF USART3
0x40004400 - 0x400047FF USART2
0x40003C00 - 0x40003FFF SPI3 Section 28.5.10: SPI register
0x40003000 - 0x400033FF IWDG Section 21.4.5: IWDG
0x40002C00 - 0x40002FFF WWDG Section 22.6.4: WWDG
0x40002800 - 0x40002BFF RTC Section 20.6.21: RTC
0x40002400 - 0x400027FF LCD Section 16.5.6: LCD register
0x40001400 - 0x400017FF TIM7 Section 19.4.9: TIM6 and
0x40001000 - 0x400013FF TIM6 page 507
0x40000C00 - 0x40000FFF TIM5 (32-bits)
0x40000800 - 0x40000BFF TIM4
0x40000400 - 0x400007FF TIM3
0x40000000 - 0x400003FF TIM2
```


# Analysis

Next step is getting the UART emulation to output data


    Now we dont get stuck in stm3211xx_hal_rcc.c
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)              

    rbit   r2, r3 
    ldr    r2, [pc, #200]  ; (0x80040b8 <HAL_RCC_OscConfig+1428>) 
RCC_FLAG_LSERDY=#200


    HAL_RCC_OscConfig (RCC_OscInitStruct=RCC_OscInitStruct@entry=0x20003f5c)
    at src/boards/mcu/stm32/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc.c:620
    (gdb) p/x $r2
    $6 = 0x400000
    (gdb) p/x $r3
    $7 = 0x200
    (gdb)
 

In technical manual, 
    0x40023800 - 0x40023BFF    RCC

 
QEMU's ARM core expects the firmware to be loaded into a region starting at address 0x00000000. STM32's flash program space is at 0x08000000 and what appears at 0x00000000 depends on the configuration of the BOOT pins.

Long story short, I wanted to use the same linker script that I'm using for the real hardware, so I added an address translation function and an alias region that makes a reference at 0x08000000 - ... to 0x00000000 - .... See kernel_load_translate_fn() and the flash_alias region in the init function in stm32f2xx.c.

Using arm-non-eabi-gdb
After you've started qemu-system-arm with -s (and optionally -S), you can attach to the virtual STM32 as follows:

    $ arm-non-eabi-gdb your_firmware.elf
    (gdb) target remote :1234
	Remote debugging using :1234
	0x08000a00 in Reset_Handler ()
	(gdb) 
All the QEMU monitor commands are also available via GDB by prefixing it with monitor like so:

	(gdb) monitor info mtree
	memory
	0000000000000000-7ffffffffffffffe (prio 0, RW): system
        0000000000000000-00000000000fffff (prio 0, R-): armv7m.flash
	...
	
	(gdb) monitor system_reset
	...