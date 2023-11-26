all : check

TARGET:=i2c_oled

CFLAGS+=-DSTDOUT_UART

include ../../ch32v003fun/ch32v003fun.mk

check : cv_check
flash : cv_flash
clean : cv_clean

