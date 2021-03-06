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
CPPC      		= arm-linux-gnueabi-g++
CC        	    = arm-linux-gnueabi-gcc
LD        		= arm-linux-gnueabi-gcc
OBJCOPY   		= arm-linux-gnueabi-objcopy
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
CFLAGS 			= -g -fno-common  -march=armv7-a -mlittle-endian -mtune=cortex-a7 -fno-stack-protector -nostdlib
LDFLAGS			= -g -r -fno-common  -Wl,--build-id=none -Wl,-eEntryPoint -nostdlib -L/usr/lib/arm-none-eabi -T ../Main.ld -Xlinker -Map=Main.map


#
# System includes.
#
ALLOY_BASE		= ../../../BlockWorks/Alloy
INCLUDES  		+= -I$(ALLOY_BASE)/Source/Module

#########################################################################################################


include $(PROJECT_BASE)/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/MemoryPool/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/EventQueue/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/UARTTransmitter/Build.mk
include $(SOURCE_LIBRARY)/Implementations/ErrorHandling/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/UARTReceiver/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/Debugger/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/EventQueue/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/BlockedEvents/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/FileStorage/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/PersistentStorage/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/HashTable/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/I2CMaster/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/KeyValueStore/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/LogStorage/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/PredicatedEvents/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/UnitTest/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/Utilities/Build.mk
include $(SOURCE_LIBRARY)/Implementations/AlloyDebugText/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/Safety/Build.mk
include $(SOURCE_LIBRARY)/Implementations/TextFormatter/Build.mk
#include $(SOURCE_LIBRARY)/Implementations/BlockDevice/Build.mk
include $(SOURCE_LIBRARY)/Implementations/AlloyBoardSupport/Build.mk
include $(SOURCE_LIBRARY)/Implementations/AlloyChannel/Build.mk
include $(SOURCE_LIBRARY)/Implementations/AlloyThreadLocalStorage/Build.mk
include $(SOURCE_LIBRARY)/Implementations/BitStreamCompressor/Build.mk
include $(SOURCE_LIBRARY)/Implementations/BitArray/Build.mk
include $(SOURCE_LIBRARY)/Implementations/AlloyDigitalInputProcessor/Build.mk
include $(SOURCE_LIBRARY)/Implementations/AlloyDigitalOutputProcessor/Build.mk
include $(SOURCE_LIBRARY)/Implementations/AlloyHighResolutionTimestamp/Build.mk


#
#
#
LIBS		+= -lgcc -lc


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
	@ $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@ $(CC) -MM $(CFLAGS) $(INCLUDES) $< -o $(notdir $(patsubst %.o,%.d,$@) )


