################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../auxInotify.c \
../logging.c \
../serializaciones.c \
../tiempo.c \
../utilsCliente.c \
../utilsServidor.c 

OBJS += \
./auxInotify.o \
./logging.o \
./serializaciones.o \
./tiempo.o \
./utilsCliente.o \
./utilsServidor.o 

C_DEPS += \
./auxInotify.d \
./logging.d \
./serializaciones.d \
./tiempo.d \
./utilsCliente.d \
./utilsServidor.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


