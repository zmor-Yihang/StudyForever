################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Driver/common/source/%.obj: ../Driver/common/source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo '正在调用： C2000 Compiler'
	"D:/ProgramFiles/CCS/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/ProgramFiles/CCS/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="C:/Users/30495/Desktop/CCSPro/example0/User" --include_path="C:/Users/30495/Desktop/CCSPro/example0/Driver/common/include" --include_path="C:/Users/30495/Desktop/CCSPro/example0/Driver/headers/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Driver/common/source/$(basename $(<F)).d_raw" --obj_directory="Driver/common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Driver/common/source/%.obj: ../Driver/common/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo '正在调用： C2000 Compiler'
	"D:/ProgramFiles/CCS/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/ProgramFiles/CCS/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="C:/Users/30495/Desktop/CCSPro/example0/User" --include_path="C:/Users/30495/Desktop/CCSPro/example0/Driver/common/include" --include_path="C:/Users/30495/Desktop/CCSPro/example0/Driver/headers/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Driver/common/source/$(basename $(<F)).d_raw" --obj_directory="Driver/common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


