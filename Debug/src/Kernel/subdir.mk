################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel/Kernel.c \
../src/Kernel/utils.c 

OBJS += \
./src/Kernel/Kernel.o \
./src/Kernel/utils.o 

C_DEPS += \
./src/Kernel/Kernel.d \
./src/Kernel/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/Kernel/%.o: ../src/Kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/utnso/git/so-commons-library" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


