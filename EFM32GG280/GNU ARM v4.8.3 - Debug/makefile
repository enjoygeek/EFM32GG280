################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include task/src/subdir.mk
-include src/subdir.mk
-include rf/src/subdir.mk
-include procotol/p645/src/subdir.mk
-include procotol/p101/src/subdir.mk
-include mid/src/subdir.mk
-include hal/src/subdir.mk
-include emlib/subdir.mk
-include FREERTOS/subdir.mk
-include CMSIS/efm32gg/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: EFM32GG280.axf

# Tool invocations
EFM32GG280.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "EFM32GG280.ld" -Xlinker --gc-sections -Xlinker -Map="EFM32GG280.map" --specs=nano.specs -o EFM32GG280.axf "./task/src/init.o" "./task/src/mytask.o" "./src/EFM32GG280.o" "./src/sysdef.o" "./rf/src/radio_hal.o" "./rf/src/rf_spi.o" "./rf/src/si446x_api_lib.o" "./rf/src/si446x_nirq.o" "./procotol/p645/src/p645parse.o" "./procotol/p101/src/list_request.o" "./procotol/p101/src/protocol_app_handler.o" "./mid/src/circlequeue.o" "./mid/src/database.o" "./mid/src/flashstorage.o" "./mid/src/mid.o" "./hal/src/hal_adc.o" "./hal/src/hal_cmu.o" "./hal/src/hal_dma.o" "./hal/src/hal_gpio.o" "./hal/src/hal_gps.o" "./hal/src/hal_math.o" "./hal/src/hal_mx25.o" "./hal/src/hal_r8025t.o" "./hal/src/hal_timer.o" "./hal/src/hal_usart.o" "./hal/src/hal_wdog.o" "./emlib/em_adc.o" "./emlib/em_cmu.o" "./emlib/em_dma.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_int.o" "./emlib/em_letimer.o" "./emlib/em_leuart.o" "./emlib/em_rtc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_wdog.o" "./FREERTOS/heap_1.o" "./FREERTOS/list.o" "./FREERTOS/port.o" "./FREERTOS/queue.o" "./FREERTOS/tasks.o" "./FREERTOS/timers.o" "./CMSIS/efm32gg/startup_gcc_efm32gg.o" "./CMSIS/efm32gg/system_efm32gg.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: EFM32GG280.hex'
	arm-none-eabi-objcopy -O ihex "EFM32GG280.axf" "EFM32GG280.hex"
	@echo ' '

	@echo 'Building bin file: EFM32GG280.bin'
	arm-none-eabi-objcopy -O binary "EFM32GG280.axf" "EFM32GG280.bin"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "EFM32GG280.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) EFM32GG280.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
