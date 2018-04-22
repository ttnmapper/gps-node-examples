# Parallell run

Here I compare the software built by platformio compared to the Makefile build which should be more similar to the Original RAK source.


clone https://github.com/RAKWireless/RAK811_BreakBoard
cd RAK811_BreakBoard
wget https://raw.githubusercontent.com/oguiter/RAK811_BreakBoard/master/Makefile
make


# Start qemu
./qemu-system-arm  -S -s   -serial file:uart1.log  -d unimp  -monitor stdio -machine rak811  -cpu cortex-m3  -pflash Debug/classA.bin


# Start debugger
arm-none-eabi-gdb  Debug/classA.axf -ex ' target remote:1234'




The biggest difference is that platformio has commented out the call to __libc_init_array, There also _init is called.


bl     0x8012874 <_init>  


Here is the code,



```

0x80123dc <__libc_init_array>           push   {r4, r5, r6, lr}                                                                                       │
   │0x80123de <__libc_init_array+2>         movs   r5, #0                                                                                                 │
   │0x80123e0 <__libc_init_array+4>         ldr    r6, [pc, #48]   ; (0x8012414 <__libc_init_array+56>)                                                   │
   │0x80123e2 <__libc_init_array+6>         ldr    r4, [pc, #52]   ; (0x8012418 <__libc_init_array+60>)                                                   │
   │0x80123e4 <__libc_init_array+8>         subs   r4, r4, r6                                                                                             │
   │0x80123e6 <__libc_init_array+10>        asrs   r4, r4, #2                                                                                             │
   │0x80123e8 <__libc_init_array+12>        cmp    r5, r4                                                                                                 │
   │0x80123ea <__libc_init_array+14>        bne.n  0x8012400 <__libc_init_array+36>                                                                       │
   │0x80123ec <__libc_init_array+16>        bl     0x8012874 <_init>                                                                                      │
   │0x80123f0 <__libc_init_array+20>        movs   r5, #0                                                                                                 │
   │0x80123f2 <__libc_init_array+22>        ldr    r6, [pc, #40]   ; (0x801241c <__libc_init_array+64>)                                                   │
   │0x80123f4 <__libc_init_array+24>        ldr    r4, [pc, #40]   ; (0x8012420 <__libc_init_array+68>)                                                   │
   │0x80123f6 <__libc_init_array+26>        subs   r4, r4, r6                                                                                             │
   │0x80123f8 <__libc_init_array+28>        asrs   r4, r4, #2                                                                                             │
   │0x80123fa <__libc_init_array+30>        cmp    r5, r4

```