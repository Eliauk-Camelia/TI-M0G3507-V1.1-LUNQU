################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS

GEN_OPTS__FLAG := @"./device.opt" 
GEN_CMDS__FLAG := -Wl,-l"./device_linker.cmd" 

ORDERED_OBJS += \
"./empty.o" \
"./ti_msp_dl_config.o" \
"./startup_mspm0g350x_ticlang.o" \
"./Hardware/MPU6050.o" \
"./Hardware/board.o" \
"./Hardware/bsp_iic.o" \
"./Hardware/encoder.o" \
"./Hardware/grey.o" \
"./Hardware/inv_mpu.o" \
"./Hardware/inv_mpu_dmp_motion_driver.o" \
"./Hardware/key.o" \
"./Hardware/led.o" \
"./Hardware/motor.o" \
"./Hardware/oled.o" \
$(GEN_CMDS__FLAG) \
-Wl,-ldevice.cmd.genlibs \
-Wl,-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Hardware/subdir_vars.mk
-include subdir_rules.mk
-include Hardware/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
TI-M0G3507-V1.1-LUNQU.out 

EXE_OUTPUTS__QUOTED += \
"TI-M0G3507-V1.1-LUNQU.out" 

BIN_OUTPUTS += \
TI-M0G3507-V1.1-LUNQU.hex 

BIN_OUTPUTS__QUOTED += \
"TI-M0G3507-V1.1-LUNQU.hex" 


# All Target
all: $(OBJS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "TI-M0G3507-V1.1-LUNQU.out" secondary-outputs

# Tool invocations
TI-M0G3507-V1.1-LUNQU.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"/opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -gdwarf-3 -Wall -Wl,-m"TI-M0G3507-V1.1-LUNQU.map" -Wl,-i"/home/arch/ti/mspm0_sdk_2_10_00_04/source" -Wl,-i"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU" -Wl,-i"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU/Debug/syscfg" -Wl,-i"/opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/lib" -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="TI-M0G3507-V1.1-LUNQU_linkInfo.xml" -Wl,--rom_model -o "TI-M0G3507-V1.1-LUNQU.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

TI-M0G3507-V1.1-LUNQU.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"/opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmhex" --memwidth=8 --romwidth=8 --diag_wrap=off --intel -o "TI-M0G3507-V1.1-LUNQU.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "empty.o" "ti_msp_dl_config.o" "startup_mspm0g350x_ticlang.o" "Hardware/MPU6050.o" "Hardware/board.o" "Hardware/bsp_iic.o" "Hardware/encoder.o" "Hardware/grey.o" "Hardware/inv_mpu.o" "Hardware/inv_mpu_dmp_motion_driver.o" "Hardware/key.o" "Hardware/led.o" "Hardware/motor.o" "Hardware/oled.o" 
	-$(RM) "empty.d" "ti_msp_dl_config.d" "startup_mspm0g350x_ticlang.d" "Hardware/MPU6050.d" "Hardware/board.d" "Hardware/bsp_iic.d" "Hardware/encoder.d" "Hardware/grey.d" "Hardware/inv_mpu.d" "Hardware/inv_mpu_dmp_motion_driver.d" "Hardware/key.d" "Hardware/led.d" "Hardware/motor.d" "Hardware/oled.d" 
	-@echo 'Finished clean'
	-@echo ' '

secondary-outputs: $(BIN_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

