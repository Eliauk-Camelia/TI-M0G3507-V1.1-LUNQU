################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU" -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU/Debug" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-2091494939: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/home/arch/ti/sysconfig_1.26.2/sysconfig_cli.sh" -s "/home/arch/ti/mspm0_sdk_2_10_00_04/.metadata/product.json" --script "/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU/empty.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-2091494939 ../empty.syscfg
device.opt: build-2091494939
device.cmd.genlibs: build-2091494939
ti_msp_dl_config.c: build-2091494939
ti_msp_dl_config.h: build-2091494939

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU" -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU/Debug" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: /home/arch/ti/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/opt/ccstudio/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU" -I"/home/arch/work-space/ti-space/TI-M0G3507-V1.1-LUNQU/Debug" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/home/arch/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


