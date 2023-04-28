################################################################
#                    Code files and ROM name                   #
################################################################

CODECFILES  = main.c scheduler.c graphics.c audio.c controller.c rcp.c helper.c engine.c
CODEHFILES  =
LIBRARYFILES = libdatatypes_math.o \
               libdatatypes_engine.o \
               libgame_levels.o

DEBUGFILES = debug.c usb.c

ROMNAME = platform.n64

REGISTRATION = "PLATFORM64" B PF I

PADDING = 8


################################################################
#                       Compiler Settings                      #
################################################################

CC  = gcc
LD  = ld
MAKEROM = mild
COMPFLAGS = -Wall -Werror


################################################################
#                         Make Commands                        #
################################################################

ifeq ($(DEBUG_MODE), 1)
    TARGETS = $(ROMNAME:.n64=_d.n64)
else
    TARGETS = $(ROMNAME)
endif

default: $(TARGETS)
    

################################################################
#     Don't touch below unless you know what you're doing!     #
################################################################

CODESEGMENT = codesegment.o
OBJECTS     = $(CODESEGMENT)
APP         = $(TARGETS:.n64=.out)
#LINKLIBS    = $(LIBRARYFILES:lib%=%)

################################################################
#                     ROM debug mode check                     #
################################################################

ifeq ($(DEBUG_MODE), 0)
    CODEOBJECTS     = $(CODECFILES:.c=.o) $(LIBRARYFILES)
    OPTIMIZER       = -O2
    COMPFLAGS       = 
    LCDEFS          = -MD $(COMPFLAGS) -D_FINALROM -DNDEBUG -DF3DEX_GBI_2 -DNOT_SPEC
    N64LIB          = -lgultra_rom
    MAKEROMFLAGS    = 
else
    CODEOBJECTS     = $(CODECFILES:.c=.o) $(LIBRARYFILES) $(DEBUGFILES:.c=.o)
    OPTIMIZER       = -g
    LCDEFS          = -MD $(COMPFLAGS) -DDEBUG -DF3DEX_GBI_2 -DNOT_SPEC
    N64LIB          = -lgultra_d
    MAKEROMFLAGS    = -d
endif


################################################################
#                        Linker Settings                       #
################################################################

LCINCS  = -I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include
LCOPTS  = -G 0
LDIRT   = $(APP)
#LDLINKLIBS := $(foreach inclib,$(basename $(LINKLIBS)),-l$(inclib))
LDFLAGS = $(MKDEPOPT) -L$(ROOT)/usr/lib -L$(ROOT)/usr/lib/PR $(N64LIB) -L. --start-group $(LDLINKLIBS) --end-group -L$(GCCDIR)/mipse/lib -lkmc


################################################################
#                          Dependencies                        #
################################################################

DEPS := $(CODEOBJECTS:.o=.d)

-include $(DEPS) $(ROOT)/usr/include/make/PRdefs


################################################################
#                          Compilation                         #
################################################################

$(CODESEGMENT): $(CODEOBJECTS)
    $(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS): $(OBJECTS)
    $(MAKEROM) spec $(MAKEROMFLAGS) -r $(TARGETS) -e $(APP) -s $(PADDING)
    @makemask $(TARGETS)
ifeq ($(ALLOWREGISTER), 1)
    @nrdc $(TARGETS) $(REGISTRATION)
endif
