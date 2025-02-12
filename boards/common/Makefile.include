#####################################################################
##   ,------.                    ,--.                ,--.          ##
##   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    ##
##   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    ##
##   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    ##
##   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    ##
##                                             `---'               ##
##    Routines for use in Makefiles                                ##
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

CURRENT_DIR := ${CURDIR}

##
# List of directories
#
list_directories = $(abspath $(wildcard $1/*/.))

##
# Filelist support
#

# @brief Checks if a file exists
# @details Check if a file exists, otherwise prints file not found error
# @usage $(call file_exists, <filename>)
file_exists = $(if $(wildcard $1),$(or $(info Reading $1),1),$(error File not found: $1))

# @brief removes comments
# @details $(filterout ...) returns $1 if it does not start with '//', "" otherwise
_filelist_remove_comment = $(filter-out //%,$1)

# @brief returns include file
# @details $(filter ...) returns $1 if it starts with +include+, "" otherwise
#          the $(subst ...) then removes the +include+ part
_filelist_include = $(subst +include+,,$(filter +include+%,$1))

# @brief Read filelist
# @details Open <file> and return its contents
# @usage myvar=$(call read_filelist, <file>)
_filelist_readfile = $(foreach line,$(file < $1),	\
    $(if $(call _filelist_include,$(line)),$(call load_filelist,$(dir $1)$(call _filelist_include,$(line))),$(dir $1)$(line))							\
)

# @brief Load filelist
# @details Open <filelist>, and return its contents
# @usage myvar=$(call load_filelist, <file>)
#load_filelist = $(if $(call file_exists,$1),$(addprefix $(dir $1),$(file < $1)))
load_filelist = $(if $(call file_exists,$1),$(call _filelist_readfile,$1))


#TODO:
#+incdir+
#+define+
#// (comment)

##
# Get the board
#
# This assumes the command is called from the <...>/<board> directory

# @brief get second-to-last entry in list
get_penultimate = $(wordlist $(words $1),$(words $1), __x__ $1)

# @brief get the board
get_board = $(call get_penultimate,$(subst /, ,$(CURDIR)))

