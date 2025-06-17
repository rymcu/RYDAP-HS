################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v30x_it.c \
../User/main.c \
../User/system_ch32v30x.c \
../User/usb2uart.c 

C_DEPS += \
./User/ch32v30x_it.d \
./User/main.d \
./User/system_ch32v30x.d \
./User/usb2uart.d 

OBJS += \
./User/ch32v30x_it.o \
./User/main.o \
./User/system_ch32v30x.o \
./User/usb2uart.o 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"e:/RYMCU/github/RYDAP-HS/Debug" -I"e:/RYMCU/github/RYDAP-HS/Core" -I"e:/RYMCU/github/RYDAP-HS/User" -I"e:/RYMCU/github/RYDAP-HS/Peripheral/inc" -I"e:/RYMCU/github/RYDAP-HS/DAP/Include" -I"E:\RYMCU\ch32\RYDAP-HS\DAP\Config" -I"e:/RYMCU/github/RYDAP-HS/CherryUSB/class/cdc" -I"e:/RYMCU/github/RYDAP-HS/CherryUSB/core" -I"e:/RYMCU/github/RYDAP-HS/CherryUSB/port/ch32" -I"e:/RYMCU/github/RYDAP-HS/CherryUSB/common" -I"e:/RYMCU/github/RYDAP-HS/CherryRB" -I"e:/RYMCU/github/RYDAP-HS/CherryDAP" -I"e:/RYMCU/github/RYDAP-HS/fatfs" -I"e:/RYMCU/github/RYDAP-HS/Offline" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
