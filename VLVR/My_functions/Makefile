OBJ_DIR = ./bin/
OBJ_SUF = .o
make_obj_path = $(addprefix $(OBJ_DIR), $(addsuffix $(OBJ_SUF), $(1)))

SRC_DIR = ./src/
SRC_SUF = .cpp
make_src_path = $(addprefix $(SRC_DIR), $(addsuffix $(SRC_SUF), $(1)))

MAKE_DIR = ./Makefiles/
MAKE_SUF = .mk
make_makefile_path = $(addprefix $(MAKE_DIR), $(addsuffix $(MAKE_SUF), $(1)))

H_DIR = ../include/

LIB_DIR = ../static_libs/
LIB_SUF = .a
make_lib_path = $(addprefix $(LIB_DIR), $(addprefix lib, $(addsuffix $(LIB_SUF), $(1))))

CXX = g++
CXX_FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline   	\
-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default  	\
-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy	\
-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                      	\
-Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith          	\
-Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits           	\
-Wwrite-strings -Werror=vla -D_EJUDGE_CLIENT_SIDE -D__USE_MINGW_ANSI_STDIO -D_DEBUG

TARGET_LIB = $(call make_lib_path, My_functions)

.PHONY: all prepare clean

SRCS = My_functions

all: $(call make_obj_path, $(SRCS)) | prepare
	@ar rcs $(TARGET_LIB) $(call make_obj_path, $(SRCS))
	@echo Compilation end

prepare:
	@mkdir -p $(OBJ_DIR) $(MAKE_DIR)

clean:
	@rm -rf $(OBJ_DIR) $(MAKE_DIR) $(TARGET_LIB)

.SECONDEXPANSION:

%$(MAKE_SUF): $(call make_src_path, $$(*F)) | prepare
	@rm -f $@
	@$(CXX) -MM $(CXX_FLAGS) $(addprefix -I, $(H_DIR)) $< > $@.$$$$;						\
	sed 's,\($(*F)\)\$(OBJ_SUF)[ :]*,$(call make_obj_path, \1) \./$@: ,g' < $@.$$$$ > $@;	\
	rm $@.$$$$

ifeq (, $(filter clean, $(MAKECMDGOALS)))
include $(call make_makefile_path, $(SRCS))
endif

make_object = $(call make_obj_path, $(1)) : $(call make_src_path, $(1));	\
@$(CXX) $(CXX_FLAGS) $$< $(addprefix -I, $(H_DIR)) -c -o $$@

$(foreach src, $(SRCS), $(eval $(call make_object, $(src))))
