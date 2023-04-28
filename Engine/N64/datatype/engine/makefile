################################################################
#                  Code files and library name                 #
################################################################

CODECFILES  = color.c transform2d.c
CODEHFILES  =

LIBNAME = libdatatypes_engine.a


################################################################
#     Don't touch below unless you know what you're doing!     #
################################################################

################################################################
#                       Compiler Settings                      #
################################################################

CC  = gcc
LD  = ld
AR  = ar
COMPFLAGS = -Wall -Werror


################################################################
#                     ROM debug mode check                     #
################################################################

ifeq ($(DEBUG_MODE), 0)
    CODEOBJECTS = $(CODECFILES:.c=.o)
    OPTIMIZER       = -O2
    COMPFLAGS       = 
    LCDEFS          = -MD $(COMPFLAGS) -D_FINALROM -DNDEBUG -DF3DEX_GBI_2 -DNOT_SPEC
else
    CODEOBJECTS = $(CODECFILES:.c=.o)
    OPTIMIZER       = -g
    LCDEFS          = -MD $(COMPFLAGS) -DDEBUG -DF3DEX_GBI_2 -DNOT_SPEC
endif


################################################################
#                         Make Commands                        #
################################################################

TARGETS = $(LIBNAME)
OBJECTS = $(CODEOBJECTS)

default: $(TARGETS)


################################################################
#                        Linker Settings                       #
################################################################


################################################################
#                          Dependencies                        #
################################################################

DEPS := $(CODEOBJECTS:.o=.d)

-include $(DEPS) $(ROOT)/usr/include/make/celfdefs


################################################################
#                          Compilation                         #
################################################################

CCINC =	-I. -I$(ROOT)/usr/include
CCDEF = $(LCDEFS) -D_WIN32 -D_LANGUAGE_C -D_MIPS_SZLONG=32 -D_MIPS_SZINT=32
CELFRULES = $(ROOT)/usr/include/make/celfrules

$(TARGETS): $(CODEOBJECTS)
	$(AR) -cr $(TARGETS) $(CODEOBJECTS)
    
.c.o:
	$(CC) $(OPTIMIZER) -c $(CCINC) $(CCDEF) $<
