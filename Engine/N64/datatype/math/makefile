################################################################
#                    Code files and ROM name                   #
################################################################

CODECFILES  = quaternion.c vector2d.c vector3d.c
CODEHFILES  =

LIBNAME = datatypes_math.a


################################################################
#                       Compiler Settings                      #
################################################################

CC  = gcc
LD  = ld
AR  = ar
COMPFLAGS = -Wall -Werror


################################################################
#     Don't touch below unless you know what you're doing!     #
################################################################

################################################################
#                     ROM debug mode check                     #
################################################################

ifeq ($(DEBUG_MODE), 0)
    CODEOBJECTS     = $(CODECFILES:.c=.o)
    OPTIMIZER       = -O2
    COMPFLAGS       = 
    LCDEFS          = -MD $(COMPFLAGS) -D_FINALROM -DNDEBUG -DF3DEX_GBI_2 -DNOT_SPEC
else
    CODEOBJECTS     = $(CODECFILES:.c=.o)
    OPTIMIZER       = -g
    LCDEFS          = -MD $(COMPFLAGS) -DDEBUG -DF3DEX_GBI_2 -DNOT_SPEC
endif


################################################################
#                        Linker Settings                       #
################################################################

LCINCS  = -I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include 
LCOPTS  = -G 0
LDIRT   = $(APP)
LDFLAGS = $(MKDEPOPT) -L$(ROOT)/usr/lib -L$(ROOT)/usr/lib/PR $(N64LIB) -L$(GCCDIR)/mipse/lib -lkmc


################################################################
#                          Compilation                         #
################################################################

GCCFLAG	= -c -I$(ROOT)/usr/include -D_MIPS_SZLONG=32 -D_MIPS_SZINT=32
CFLAGS = $(LCDEFS) $(LCINCS) $(LCOPTS) $(GCCFLAG) $(OPTIMIZER)
CELFRULES = $(ROOT)/usr/include/make/celfrules
include $(ROOT)/usr/include/make/celfdefs

TARGETS = $(LIBNAME)
OBJECTS = $(CODEOBJECTS)

default: $(TARGETS)

$(TARGETS): $(CODEOBJECTS)
	$(AR) -cr $(TARGETS) $(OBJECTS)
    
.c.o:
	$(CC) -c $(CFLAGS) $<


################################################################
#                          Dependencies                        #
################################################################

DEPS := $(wildcard $(CODEOBJECTS:.o=.d)*.d)

-include $(DEPS)
