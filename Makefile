#####################################################################
##   ,------.                    ,--.                ,--.          ##
##   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    ##
##   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    ##
##   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    ##
##   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    ##
##                                             `---'               ##
##    Build Makefile for virtual development board                 ##
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

PWD:=$(dir $(lastword MAKEFILE_LIST))

include $(PWD)boards/common/build/Makefile.include

#get the vendors
list_vendors=$(filter-out $1 common, $(subst /, ,$(call list_directories,$1)))
vendors=$(call list_vendors,boards)

#get boards per vendor
boards_list=$(foreach vendor,$(vendors),$(call list_directories,boards/$(vendor)))
boards=$(filter-out boards $(vendors),$(subst /, ,$(boards_list)))


#####################################################################
## Boards                
#####################################################################
.PHONY: $(boards) $(boards)_clean

#Call the makefile in the <vendor>/<board>/<build> directory
$(boards):
	$(MAKE) -C "$(filter %/$@/, $(boards_list))build" $@ 	\
		CALLING_FROM=$(abspath $(CURDIR))		\
		board=$@ filelist=$(filelist)

$(boards)_clean: %_clean :
	$(MAKE) -C "$(filter %/$*/, $(boards_list))build" clean


#####################################################################
## Demo
#####################################################################
.PHONY: demo

demo:
	echo "Building demo"
	$(MAKE) de10lite filelist=demo/filelist.f


#####################################################################
## Help
#####################################################################
.PHONY: help

help:
	@echo "usage: make <board> filelist=<filelist.f>"
	@echo "boards:"
	$(foreach board,$(boards),@echo "- $(board)")
	@echo "filelist points to a .f file with the design's verilog RTL files"
	@echo "make <board>_clean  : clean <board> build directory"
	@echo "make help           : this help message"


