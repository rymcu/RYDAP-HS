################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DAP/Source/DAP.c \
../DAP/Source/DAP_vendor.c \
../DAP/Source/JTAG_DP.c \
../DAP/Source/SWO.c \
../DAP/Source/SW_DP.c \
../DAP/Source/UART.c \
../DAP/Source/swd_host.c 

C_DEPS += \
./DAP/Source/DAP.d \
./DAP/Source/DAP_vendor.d \
./DAP/Source/JTAG_DP.d \
./DAP/Source/SWO.d \
./DAP/Source/SW_DP.d \
./DAP/Source/UART.d \
./DAP/Source/swd_host.d 

OBJS += \
./DAP/Source/DAP.o \
./DAP/Source/DAP_vendor.o \
./DAP/Source/JTAG_DP.o \
./DAP/Source/SWO.o \
./DAP/Source/SW_DP.o \
./DAP/Source/UART.o \
./DAP/Source/swd_host.o 



# Each subdirectory must supply rules for building sources it contributes
DAP/Source/%.o: ../DAP/Source/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"e:/RYMCU/ch32/RYDAP-HS/Debug" -I"e:/RYMCU/ch32/RYDAP-HS/Core" -I"e:/RYMCU/ch32/RYDAP-HS/User" -I"e:/RYMCU/ch32/RYDAP-HS/Peripheral/inc" -I"e:/RYMCU/ch32/RYDAP-HS/DAP/Include" -I"E:\RYMCU\ch32\RYDAP-HS\DAP\Config" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/class/cdc" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/core" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/port/ch32" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/common" -I"e:/RYMCU/ch32/RYDAP-HS/CherryRB" -I"e:/RYMCU/ch32/RYDAP-HS/CherryDAP" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
