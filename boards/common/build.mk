#####################################################################
##   ,------.                    ,--.                ,--.          ##
##   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    ##
##   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    ##
##   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    ##
##   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    ##
##                                             `---'               ##
##    Common Build Makefile for virtual development boards         ##
##                                                                 ##
#####################################################################
##                                                                 ##
##    Copyright (C) 2024 Roa Logic BV - www.roalogic.com           ##
##    Copyright (C) 2024 richard.herveille@roalogic.com            ##
##                                                                 ##
##     Redistribution and use in source and binary forms, with     ##
##   or without modification, are permitted provided that the      ##
##   following conditions are met:                                 ##
##   1. Redistributions of source code must retain the above       ##
##      copyright notice, this list of conditions and the          ##
##      following disclaimer.                                      ##
##   2. Redistributions in binary form must reproduce the above    ##
##      copyright notice, this list of conditions and the          ##
##      following disclaimer in the documentation and/or other     ##
##      materials provided with the distribution.                  ##
##   3. Neither the name of the copyright holder nor the names     ##
##      of its contributors may be used to endorse or promote      ##
##      products derived from this software without specific       ##
##      prior written permission.                                  ##
##                                                                 ##
##     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND      ##
##   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,   ##
##   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF      ##
##   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE      ##
##   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR         ##
##   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  ##
##   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT  ##
##   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;  ##
##   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)      ##
##   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     ##
##   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE     ##
##   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS       ##
##   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  ##
##                                                                 ##
#####################################################################

SAVE_CWD:=$(CWD)
CWD:=$(dir $(lastword $(MAKEFILE_LIST)))
BOARDS_DIR:=$(CWD)../
SRC_DIR:=$(CWD)../../src/
CWD:=$(SAVE_CWD)

include $(BOARDS_DIR)common/utils.mk

board:=$(call get_board)
wrapper:=$(board)_verilator_wrapper

#!! directly expanded variables for CWD
ifeq ($(strip $(filelist)),)
  RTL_VERILOG:=
else
  RTL_VERILOG:=$(call load_filelist, $(filelist))
endif
INCDIRS:=
TB_CXX:=

#include common source files
#must be after RTL_VERILOG, INCDIRS, and TB_CXX definitions
include $(SRC_DIR)common/Makefile.include


#####################################################################
## Verilator Environment
#####################################################################

#Object directory, design is verilated here
OBJDIR:=obj_dir

#Set VERILATOR_ROOT if not already set
VERILATOR_ROOT ?= $(shell bash -c 'verilator -V | grep VERILATOR_ROOT | tail -1 | sed -e " s/^.*=\s*//"')
ifeq ($(VERILATOR_ROOT),)
  VERILATOR_ROOT = $(shell bash -c 'verilator -V | grep VERILATOR_ROOT | head -1 | sed -e " s/^.*=\s*//"')
endif
VERILATOR_FLAGS ?= -Wall -Wno-PINCONNECTEMPTY
VERILATE_FLAGS ?= -CFLAGS -DVL_NO_LEGACY -CFLAGS -O3 -Wno-lint -structs-packed --trace 

#These files need to be included in the compile
VERILATOR_CXX     = $(addprefix $(VERILATOR_ROOT)/include/, verilated_threads.cpp verilated.cpp verilated_vcd_c.cpp verilated_dpi.cpp)
VERILATOR_INCLUDE = $(addprefix -I, $(VERILATOR_ROOT)/include $(VERILATOR_ROOT)/include/vltstd)

#CXX Variables
CXX ?= g++
#CXXFLAGS ?= -fcoroutines
CXXFLAGS += $(shell wx-config --cxxflags)
CXXFLAGS += -g
CXXFLAGS += -O3

CPPSTD ?= c++20
CPPLIB ?= stdc++

LDLIBS = -lm -pthread
LDFLAGS += $(shell wx-config --libs)

ifdef PLI
ifneq ($(PLI),"")
  PLI_OPTS = -pli $(PLI)
endif
endif


#################################################################
## Build app
#################################################################

vdb_Dpi.h = $(OBJDIR)/vdb__Dpi.h


$(board): $(OBJDIR)/V$(wrapper)__ALL.o $(OBJDIR)/vdb__Dpi.h $(VERILATOR_CXX) $(TB_CXX)
	@echo "--- Building $@"
	$(CXX) $(CXXFLAGS) -std=$(CPPSTD) 			\
	$(VERILATOR_INCLUDE) $<	$(TB_CXX) $(VERILATOR_CXX)	\
	-I./$(OBJDIR) 						\
	$(addprefix -I, $(INCDIRS))				\
	$(LDFLAGS) $(LDLIBS)					\
	-o $@


#####################################################################
## Rules
#####################################################################

V%.mk: $(RTL_VERILOG)
	@echo "--- Verilating $*"
	mkdir -p $(OBJDIR)
	verilator $(VERILATOR_FLAGS) $(VERILATE_FLAGS)		\
	-Mdir $(@D) --cc $(RTL_VERILOG) --top-module $(*F)	\
	$(foreach d,$(DEFINES),+define+$d)			\
	$(foreach d,$(INCDIRS),+incdir+$d)			\
	$(foreach l,$(wildcard $(LIBDIRS)),-y $l)

%__ALL.cpp: %.mk
	#must rebuild here, otherwise prerequisites for _ALL.o not met
	@echo "--- Building $(@F)"
	make -C $(@D) -f $(*F).mk

%__ALL.o: %__ALL.cpp
	echo

#This builds a Makefile, which is then called to generate the vdb__Dpi.h file
#This is needed, because $(wildcard ...) expension must happen after the .h files are generated
Makefile.vdb:
	echo "VERILATED_HEADERS=\$$(notdir \$$(filter-out %__024root.h %__Syms.h %__pch.h,\$$(wildcard $(OBJDIR)/*.h)))" > $@
	echo "$(OBJDIR)/vdb__Dpi.h:" >> $@
	echo "	echo \$$(foreach f,\$$(VERILATED_HEADERS),\"#include \\\"\$$f\\\"\\\n\") >> \$$@" >> $@
	echo "	echo \"using designName = V$(wrapper);\" >> \$$@" >> $@


%vdb__Dpi.h: Makefile.vdb $(OBJDIR)/V$(wrapper)__ALL.o
	$(MAKE) -f $^ $@


#################################################################
## Clean
#################################################################
clean:
	rm -rf Makefile.vdb $(OBJDIR)/*

