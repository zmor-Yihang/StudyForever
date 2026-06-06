################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
User/bsp/%.obj: ../User/bsp/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ProgramFiles/CCS/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/ProgramFiles/CCS/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="C:/Users/30495/Desktop/StudyForever/example0/User" --include_path="C:/Users/30495/Desktop/StudyForever/example0/Driver/common/include" --include_path="C:/Users/30495/Desktop/StudyForever/example0/Driver/headers/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="User/bsp/$(basename $(<F)).d_raw" --include_path="C:/Users/30495/Desktop/StudyForever/example0/Debug/syscfg" --obj_directory="User/bsp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


