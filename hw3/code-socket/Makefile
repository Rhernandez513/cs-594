## [M1: point 1]
#  Explain following in here...
# ...
#  This line reads the current working 
# directory and assigning it to the variable CUR_DIR
CUR_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

## [M2: point 1]
#  Explain following in here
#  ...
# The first line specifies the compilier as the GNU Compiler 
# Collection (GCC) and assigns it to the variable CC
#
# the second line specifies the flags to be used by the compiler
# in this case, the flags are -g -O2 -Werror -Wall which are
# -g for debugging, -O2 for optimization, -Werror for warnings
# and -WAll for all warnings
#
# the third line specifies the flags to be used by the linker
# in this case, there are no flags
CC = gcc
CFLAGS += -g -O2 -Werror -Wall
LDFLAGS +=

## [M3: point 1]
#  Explain following in here
#  ...
# The first line specifies the directory for dependencies
# and assigns it to the variable DEPS_DIR
# in this case the directory is .deps in the current directory
# 
# the second line specifies the flags to be used by the compiler
# which in this case are -MD -MF $(DEPS_DIR)/$*.d -MP
# -MD is for dependency generation
# -MF is for specifies the files that are dependencies 
# as a list here with the extension ".d"
# -MP is for specifying phony targets for each dependency
DEPS_DIR  := $(CUR_DIR)/.deps$(LIB_SUFFIX)
DEPCFLAGS = -MD -MF $(DEPS_DIR)/$*.d -MP

## [M5: point 1]
#  Explain following in here
#  ...
# The first line specifies the source files to be compiled
# and assigns it to the variable SRC_FILES
# in this case, the source files are all the files with the
# .c extension in the current directory
SRC_FILES = $(wildcard *.c)

## [M6: point 1]
#  Explain following in here
#  ...
# The following line specifies the executable files 
# which are the source files without the .c extension
# string substitution is used to remove the .c extension
EXE_FILES = $(SRC_FILES:.c=)

## [M7: point 1]
#  Explain following in here
#  ...
# The first line specifies the target to be built
# and assigns it to the variable all
# in this case, the target is all
# and the target action is to print all the
# executable files to the console
all: $(EXE_FILES)
	echo $(EXE_FILES)

## [M8: point 1]
#  Explain following in here
#  ...
# The first line gets all of the dependencies
# in the deps_dir with the extension .c and
# the percent signs are wildcards, so it gets
# 
# The second line is the command to the compiler
# we've already discussed "CC" "CFLAGS" and
# "DEPCFLAGS" above, but the -c flag is for
# compiling without linking, the $@ refers to
# the target files ".o" in this case and
# the $< refers to the source files ".c" in
# this case
%/%.c:%.c $(DEPS_DIR)
	$(CC) $(CFLAGS) $(DEPCFLAGS) -c $@ $<

## [M9: point 1]
#  Explain following in here
#  ...
# The first line specifies the target to be built
# and assigns it to the variable clean
# in this case, the target is clean
# and the target is to remove all the executable files
clean:
	rm -f $(EXE_FILES)

## [M10: point 1]
#  Explain following in here
#  ...
# The first line specifies the target to be built
# in this case, the target is .PHONY
# and the target is to run the all command
# and then then clean command 
.PHONY: all clean
