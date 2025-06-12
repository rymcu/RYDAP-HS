################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CherryRB/chry_ringbuffer.c 

C_DEPS += \
./CherryRB/chry_ringbuffer.d 

OBJS += \
./CherryRB/chry_ringbuffer.o 



# Each subdirectory must supply rules for building sources it contributes
CherryRB/%.o: ../CherryRB/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"e:/RYMCU/ch32/RYDAP-HS/Debug" -I"e:/RYMCU/ch32/RYDAP-HS/Core" -I"e:/RYMCU/ch32/RYDAP-HS/User" -I"e:/RYMCU/ch32/RYDAP-HS/Peripheral/inc" -I"e:/RYMCU/ch32/RYDAP-HS/DAP/Include" -I"E:\RYMCU\ch32\RYDAP-HS\DAP\Config" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/class/cdc" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/core" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/port/ch32" -I"e:/RYMCU/ch32/RYDAP-HS/CherryUSB/common" -I"e:/RYMCU/ch32/RYDAP-HS/CherryRB" -I"e:/RYMCU/ch32/RYDAP-HS/CherryDAP" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
