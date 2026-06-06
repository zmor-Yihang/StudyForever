################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../Driver/source/DSP2833x_ADC_cal.asm \
../Driver/source/DSP2833x_CSMPasswords.asm \
../Driver/source/DSP2833x_CodeStartBranch.asm \
../Driver/source/DSP2833x_DBGIER.asm \
../Driver/source/DSP2833x_DisInt.asm \
../Driver/source/DSP2833x_usDelay.asm 

C_SRCS += \
../Driver/source/DSP2833x_Adc.c \
../Driver/source/DSP2833x_CpuTimers.c \
../Driver/source/DSP2833x_DMA.c \
../Driver/source/DSP2833x_DefaultIsr.c \
../Driver/source/DSP2833x_ECan.c \
../Driver/source/DSP2833x_ECap.c \
../Driver/source/DSP2833x_EPwm.c \
../Driver/source/DSP2833x_EQep.c \
../Driver/source/DSP2833x_GlobalVariableDefs.c \
../Driver/source/DSP2833x_Gpio.c \
../Driver/source/DSP2833x_I2C.c \
../Driver/source/DSP2833x_Mcbsp.c \
../Driver/source/DSP2833x_MemCopy.c \
../Driver/source/DSP2833x_PieCtrl.c \
../Driver/source/DSP2833x_PieVect.c \
../Driver/source/DSP2833x_Sci.c \
../Driver/source/DSP2833x_Spi.c \
../Driver/source/DSP2833x_SysCtrl.c \
../Driver/source/DSP2833x_Xintf.c 

C_DEPS += \
./Driver/source/DSP2833x_Adc.d \
./Driver/source/DSP2833x_CpuTimers.d \
./Driver/source/DSP2833x_DMA.d \
./Driver/source/DSP2833x_DefaultIsr.d \
./Driver/source/DSP2833x_ECan.d \
./Driver/source/DSP2833x_ECap.d \
./Driver/source/DSP2833x_EPwm.d \
./Driver/source/DSP2833x_EQep.d \
./Driver/source/DSP2833x_GlobalVariableDefs.d \
./Driver/source/DSP2833x_Gpio.d \
./Driver/source/DSP2833x_I2C.d \
./Driver/source/DSP2833x_Mcbsp.d \
./Driver/source/DSP2833x_MemCopy.d \
./Driver/source/DSP2833x_PieCtrl.d \
./Driver/source/DSP2833x_PieVect.d \
./Driver/source/DSP2833x_Sci.d \
./Driver/source/DSP2833x_Spi.d \
./Driver/source/DSP2833x_SysCtrl.d \
./Driver/source/DSP2833x_Xintf.d 

OBJS += \
./Driver/source/DSP2833x_ADC_cal.obj \
./Driver/source/DSP2833x_Adc.obj \
./Driver/source/DSP2833x_CSMPasswords.obj \
./Driver/source/DSP2833x_CodeStartBranch.obj \
./Driver/source/DSP2833x_CpuTimers.obj \
./Driver/source/DSP2833x_DBGIER.obj \
./Driver/source/DSP2833x_DMA.obj \
./Driver/source/DSP2833x_DefaultIsr.obj \
./Driver/source/DSP2833x_DisInt.obj \
./Driver/source/DSP2833x_ECan.obj \
./Driver/source/DSP2833x_ECap.obj \
./Driver/source/DSP2833x_EPwm.obj \
./Driver/source/DSP2833x_EQep.obj \
./Driver/source/DSP2833x_GlobalVariableDefs.obj \
./Driver/source/DSP2833x_Gpio.obj \
./Driver/source/DSP2833x_I2C.obj \
./Driver/source/DSP2833x_Mcbsp.obj \
./Driver/source/DSP2833x_MemCopy.obj \
./Driver/source/DSP2833x_PieCtrl.obj \
./Driver/source/DSP2833x_PieVect.obj \
./Driver/source/DSP2833x_Sci.obj \
./Driver/source/DSP2833x_Spi.obj \
./Driver/source/DSP2833x_SysCtrl.obj \
./Driver/source/DSP2833x_Xintf.obj \
./Driver/source/DSP2833x_usDelay.obj 

ASM_DEPS += \
./Driver/source/DSP2833x_ADC_cal.d \
./Driver/source/DSP2833x_CSMPasswords.d \
./Driver/source/DSP2833x_CodeStartBranch.d \
./Driver/source/DSP2833x_DBGIER.d \
./Driver/source/DSP2833x_DisInt.d \
./Driver/source/DSP2833x_usDelay.d 

OBJS__QUOTED += \
"Driver\source\DSP2833x_ADC_cal.obj" \
"Driver\source\DSP2833x_Adc.obj" \
"Driver\source\DSP2833x_CSMPasswords.obj" \
"Driver\source\DSP2833x_CodeStartBranch.obj" \
"Driver\source\DSP2833x_CpuTimers.obj" \
"Driver\source\DSP2833x_DBGIER.obj" \
"Driver\source\DSP2833x_DMA.obj" \
"Driver\source\DSP2833x_DefaultIsr.obj" \
"Driver\source\DSP2833x_DisInt.obj" \
"Driver\source\DSP2833x_ECan.obj" \
"Driver\source\DSP2833x_ECap.obj" \
"Driver\source\DSP2833x_EPwm.obj" \
"Driver\source\DSP2833x_EQep.obj" \
"Driver\source\DSP2833x_GlobalVariableDefs.obj" \
"Driver\source\DSP2833x_Gpio.obj" \
"Driver\source\DSP2833x_I2C.obj" \
"Driver\source\DSP2833x_Mcbsp.obj" \
"Driver\source\DSP2833x_MemCopy.obj" \
"Driver\source\DSP2833x_PieCtrl.obj" \
"Driver\source\DSP2833x_PieVect.obj" \
"Driver\source\DSP2833x_Sci.obj" \
"Driver\source\DSP2833x_Spi.obj" \
"Driver\source\DSP2833x_SysCtrl.obj" \
"Driver\source\DSP2833x_Xintf.obj" \
"Driver\source\DSP2833x_usDelay.obj" 

C_DEPS__QUOTED += \
"Driver\source\DSP2833x_Adc.d" \
"Driver\source\DSP2833x_CpuTimers.d" \
"Driver\source\DSP2833x_DMA.d" \
"Driver\source\DSP2833x_DefaultIsr.d" \
"Driver\source\DSP2833x_ECan.d" \
"Driver\source\DSP2833x_ECap.d" \
"Driver\source\DSP2833x_EPwm.d" \
"Driver\source\DSP2833x_EQep.d" \
"Driver\source\DSP2833x_GlobalVariableDefs.d" \
"Driver\source\DSP2833x_Gpio.d" \
"Driver\source\DSP2833x_I2C.d" \
"Driver\source\DSP2833x_Mcbsp.d" \
"Driver\source\DSP2833x_MemCopy.d" \
"Driver\source\DSP2833x_PieCtrl.d" \
"Driver\source\DSP2833x_PieVect.d" \
"Driver\source\DSP2833x_Sci.d" \
"Driver\source\DSP2833x_Spi.d" \
"Driver\source\DSP2833x_SysCtrl.d" \
"Driver\source\DSP2833x_Xintf.d" 

ASM_DEPS__QUOTED += \
"Driver\source\DSP2833x_ADC_cal.d" \
"Driver\source\DSP2833x_CSMPasswords.d" \
"Driver\source\DSP2833x_CodeStartBranch.d" \
"Driver\source\DSP2833x_DBGIER.d" \
"Driver\source\DSP2833x_DisInt.d" \
"Driver\source\DSP2833x_usDelay.d" 

ASM_SRCS__QUOTED += \
"../Driver/source/DSP2833x_ADC_cal.asm" \
"../Driver/source/DSP2833x_CSMPasswords.asm" \
"../Driver/source/DSP2833x_CodeStartBranch.asm" \
"../Driver/source/DSP2833x_DBGIER.asm" \
"../Driver/source/DSP2833x_DisInt.asm" \
"../Driver/source/DSP2833x_usDelay.asm" 

C_SRCS__QUOTED += \
"../Driver/source/DSP2833x_Adc.c" \
"../Driver/source/DSP2833x_CpuTimers.c" \
"../Driver/source/DSP2833x_DMA.c" \
"../Driver/source/DSP2833x_DefaultIsr.c" \
"../Driver/source/DSP2833x_ECan.c" \
"../Driver/source/DSP2833x_ECap.c" \
"../Driver/source/DSP2833x_EPwm.c" \
"../Driver/source/DSP2833x_EQep.c" \
"../Driver/source/DSP2833x_GlobalVariableDefs.c" \
"../Driver/source/DSP2833x_Gpio.c" \
"../Driver/source/DSP2833x_I2C.c" \
"../Driver/source/DSP2833x_Mcbsp.c" \
"../Driver/source/DSP2833x_MemCopy.c" \
"../Driver/source/DSP2833x_PieCtrl.c" \
"../Driver/source/DSP2833x_PieVect.c" \
"../Driver/source/DSP2833x_Sci.c" \
"../Driver/source/DSP2833x_Spi.c" \
"../Driver/source/DSP2833x_SysCtrl.c" \
"../Driver/source/DSP2833x_Xintf.c" 


