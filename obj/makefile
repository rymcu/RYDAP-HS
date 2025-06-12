################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include DAP/Source/subdir.mk
-include Core/subdir.mk
-include CherryUSB/port/ch32/subdir.mk
-include CherryUSB/core/subdir.mk
-include CherryUSB/class/cdc/subdir.mk
-include CherryRB/subdir.mk
-include CherryDAP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
RYDAP-HS.hex \

SECONDARY_LIST += \
RYDAP-HS.lst \

SECONDARY_SIZE += \
RYDAP-HS.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: RYDAP-HS.elf secondary-outputs

# Tool invocations
RYDAP-HS.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "e:/RYMCU/github/RYDAP-HS/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"RYDAP-HS.map" --specs=nano.specs --specs=nosys.specs -o "RYDAP-HS.elf" $(OBJS) $(USER_OBJS) $(LIBS)
RYDAP-HS.hex: RYDAP-HS.elf
	@	riscv-none-embed-objcopy -O ihex "RYDAP-HS.elf" "RYDAP-HS.hex"
RYDAP-HS.lst: RYDAP-HS.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "RYDAP-HS.elf" > "RYDAP-HS.lst"
RYDAP-HS.siz: RYDAP-HS.elf
	riscv-none-embed-size --format=berkeley "RYDAP-HS.elf"

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) RYDAP-HS.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets