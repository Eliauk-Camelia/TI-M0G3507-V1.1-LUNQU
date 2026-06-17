################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Hardware/%.o: ../Hardware/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU" -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU/Debug" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"Hardware/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


