################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel/Kernel.c 

OBJS += \
./src/Kernel/Kernel.o 

C_DEPS += \
./src/Kernel/Kernel.d 


# Each subdirectory must supply rules for building sources it contributes
src/Kernel/%.o: ../src/Kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


