################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/CRC32.c \
../Core/Src/ChaCha20.c \
../Core/Src/Frame.c \
../Core/Src/My_SDCard.c \
../Core/Src/add_scalar.c \
../Core/Src/fe.c \
../Core/Src/ge.c \
../Core/Src/key_exchange.c \
../Core/Src/keypair.c \
../Core/Src/main.c \
../Core/Src/sc.c \
../Core/Src/seed.c \
../Core/Src/sha3.c \
../Core/Src/sha512.c \
../Core/Src/sign.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/verify.c 

OBJS += \
./Core/Src/CRC32.o \
./Core/Src/ChaCha20.o \
./Core/Src/Frame.o \
./Core/Src/My_SDCard.o \
./Core/Src/add_scalar.o \
./Core/Src/fe.o \
./Core/Src/ge.o \
./Core/Src/key_exchange.o \
./Core/Src/keypair.o \
./Core/Src/main.o \
./Core/Src/sc.o \
./Core/Src/seed.o \
./Core/Src/sha3.o \
./Core/Src/sha512.o \
./Core/Src/sign.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/verify.o 

C_DEPS += \
./Core/Src/CRC32.d \
./Core/Src/ChaCha20.d \
./Core/Src/Frame.d \
./Core/Src/My_SDCard.d \
./Core/Src/add_scalar.d \
./Core/Src/fe.d \
./Core/Src/ge.d \
./Core/Src/key_exchange.d \
./Core/Src/keypair.d \
./Core/Src/main.d \
./Core/Src/sc.d \
./Core/Src/seed.d \
./Core/Src/sha3.d \
./Core/Src/sha512.d \
./Core/Src/sign.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/verify.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/CRC32.d ./Core/Src/CRC32.o ./Core/Src/ChaCha20.d ./Core/Src/ChaCha20.o ./Core/Src/Frame.d ./Core/Src/Frame.o ./Core/Src/My_SDCard.d ./Core/Src/My_SDCard.o ./Core/Src/add_scalar.d ./Core/Src/add_scalar.o ./Core/Src/fe.d ./Core/Src/fe.o ./Core/Src/ge.d ./Core/Src/ge.o ./Core/Src/key_exchange.d ./Core/Src/key_exchange.o ./Core/Src/keypair.d ./Core/Src/keypair.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/sc.d ./Core/Src/sc.o ./Core/Src/seed.d ./Core/Src/seed.o ./Core/Src/sha3.d ./Core/Src/sha3.o ./Core/Src/sha512.d ./Core/Src/sha512.o ./Core/Src/sign.d ./Core/Src/sign.o ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/verify.d ./Core/Src/verify.o

.PHONY: clean-Core-2f-Src

