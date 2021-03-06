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
CPPC      		= g++
CC        	    = gcc
LD        		= g++
OBJCOPY   		= objcopy
RM        		= rm
MKDIR     		= mkdir
SIZE      		= size
HEX2BIN   		= hex2bin
ECHO      		= echo
ECHO_RED  		= echo -e "\033[0;31m"
ECHO_GREEN		= echo -e "\033[0;32m"
OBJDUMP   		= objdump


#
# Build flags.
#
CFLAGS 			= -O3 -flto -g --std=c99   -Wno-unused-result 
CPPFLAGS 		= -O3 -flto -g --std=c++17 -Wno-unused-result 
LDFLAGS			= -g 


#########################################################################################################


include $(PROJECT_BASE)/Build.mk
include $(PROJECT_BASE)/../Common/Build.mk
include $(SOURCE_LIBRARY)/Implementations/ErrorHandling/Build.mk
include $(SOURCE_LIBRARY)/Implementations/StdioDebugText/Build.mk


#
#
#
LIBS		+= -lc -lgcc -lrt -lpthread 



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
	@ $(OBJDUMP) -d -S -l $@ > $@.s
	@ $(SIZE) -d $@


#########################################################################################################


#
# Compile a file and then generate its dependencies.
#
%.o: %.c
	@ $(ECHO) Building $(basename $(notdir $<))
	@ $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@ -MMD -MP -MF $(notdir $(patsubst %.o,%.d,$@) )

#
# Compile a file and then generate its dependencies.
#
%.o: %.cpp
	@ $(ECHO) Building $(basename $(notdir $<))
	@ $(CPPC) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@ -MMD -MP -MF $(notdir $(patsubst %.o,%.d,$@) )



