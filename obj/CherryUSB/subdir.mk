################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CherryUSB/usb_dc_usbhs.c \
../CherryUSB/usbd_cdc.c \
../CherryUSB/usbd_core.c 

C_DEPS += \
./CherryUSB/usb_dc_usbhs.d \
./CherryUSB/usbd_cdc.d \
./CherryUSB/usbd_core.d 

OBJS += \
./CherryUSB/usb_dc_usbhs.o \
./CherryUSB/usbd_cdc.o \
./CherryUSB/usbd_core.o 



# Each subdirectory must supply rules for building sources it contributes
CherryUSB/%.o: ../CherryUSB/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"e:/RYMCU/ch32/RYDAP-HS/Debug" -I"e:/RYMCU/ch32/RYDAP-HS/Core" -I"e:/RYMCU/ch32/RYDAP-HS/User" -I"e:/RYMCU/ch32/RYDAP-HS/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
