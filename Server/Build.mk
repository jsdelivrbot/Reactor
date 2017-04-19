
#
# Source directories
#
VPATH		+=	$(PROJECT_BASE)/


#
# Include directories.
#
INCLUDES	+=	-I$(PROJECT_BASE)/ \
				-I/usr/include/libusb-1.0


#
# Objects to build.
#
OBJS		+=	Main.o \
				FTDIDataSource.o
    		  		

#
# Libraries.
#
LIBS		+= -lusb-1.0 -lpthread

