#
# Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited.
# Proprietary and confidential.
# Written by Steve Tickle <Steve@BlockWorks.co>, November 2015.
#


#
# Make sure we do something sensible if no target is specified.
#
.DEFAULT_GOAL := all
.SUFFIXES: .c .h .o


#########################################################################################################

#
# Tools
#
SHELL     		= sh
CPPC      		= arm-linux-gnueabihf-g++
CC        	    = arm-linux-gnueabihf-gcc
LD        		= arm-linux-gnueabihf-gcc
OBJCOPY   		= arm-linux-gnueabihf-objcopy
RM        		= rm
MKDIR     		= mkdir
SIZE      		= size
HEX2BIN   		= hex2bin
ECHO      		= echo
ECHO_RED  		= echo -e "\033[0;31m"
ECHO_GREEN		= echo -e "\033[0;32m"


#
# Build flags.
#
CFLAGS 			= -O3 -g --std=c99 -Wno-unused-result -D_GNU_SOURCE -mfpu=neon -std=c++17
LDFLAGS			= -g


#########################################################################################################


include $(PROJECT_BASE)/Build.mk
include $(PROJECT_BASE)/../Common/Build.mk
include $(SOURCE_LIBRARY)/Implementations/CircularBuffer/Build.mk
include $(SOURCE_LIBRARY)/Implementations/ARMLinuxTimestamp/Build.mk
include $(SOURCE_LIBRARY)/Implementations/LinuxSharedMemory/Build.mk
include $(SOURCE_LIBRARY)/Implementations/ErrorHandling/Build.mk
include $(SOURCE_LIBRARY)/Implementations/StdioDebugText/Build.mk


#
#
#
LIBS		+= -lc -lgcc -lrt


#########################################################################################################


#
# Include the dependency rules for each target iff already generated.
# If they haven't been generated, then we have to build anyway.
#
-include $(notdir $(patsubst %.o,%.d,$(OBJS)) )


#########################################################################################################


#
# Default target.
#
all: Main.elf


#
# Remove all the build artifacts.
#
clean:
	@ $(ECHO) Cleaning.
	-@ $(RM) -f *.d
	-@ $(RM) -f *.o
	-@ $(RM) -f *.a
	-@ $(RM) -f *.bin
	-@ $(RM) -f *.hex
	-@ $(RM) -f *.elf


#
# Link the objects into a binary.
#
Main.elf: $(OBJS)
	@ $(LD) $(LDFLAGS) -Xlinker -Map=$@.map -o $@ $^ $(LIBS)
	@ $(SIZE) -d $@


#########################################################################################################


#
# Compile a file and then generate its dependencies.
#
%.o: %.c
	@ $(ECHO) Building $(basename $<)
	@ $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@ -MMD -MP -MF $(notdir $(patsubst %.o,%.d,$@) )


