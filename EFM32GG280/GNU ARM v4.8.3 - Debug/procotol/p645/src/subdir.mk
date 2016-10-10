################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../procotol/p645/src/p645parse.c 

OBJS += \
./procotol/p645/src/p645parse.o 

C_DEPS += \
./procotol/p645/src/p645parse.d 


# Each subdirectory must supply rules for building sources it contributes
procotol/p645/src/p645parse.o: ../procotol/p645/src/p645parse.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32GG280F512=1' '-DDEBUG=1' -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/emlib/inc" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32GG/Include" -I"D:\WS\EFM\EFM32GG280\FREERTOS\include" -I"D:\WS\EFM\EFM32GG280\hal\inc" -I"D:\WS\EFM\EFM32GG280\rf\inc" -I"D:\WS\EFM\EFM32GG280\src" -I"D:\WS\EFM\EFM32GG280\task\inc" -I"D:\WS\EFM\EFM32GG280\mid\inc" -I"D:\WS\EFM\EFM32GG280\procotol\p101\inc" -I"D:\WS\EFM\EFM32GG280\procotol\p645\inc" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"procotol/p645/src/p645parse.d" -MT"procotol/p645/src/p645parse.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


