APP1=tmva_train
APP2=tmva_eval

#-----------configure ROOT-----------#
ifdef ROOTSYS
include $(ROOTSYS)/etc/Makefile.arch
ROOTINC     :=$(shell $(ROOTSYS)/bin/root-config --incdir)
ROOTLIBDIR  :=$(shell $(ROOTSYS)/bin/root-config --libdir)
ROOTLDFLAGS :=$(shell $(ROOTSYS)/bin/root-config --ldflags)
ROOTCFLAGS  :=$(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS    := $(shell  $(ROOTSYS)/bin/root-config --libs)
ROOTLINK = $(ROOTLIBS) $(ROOTCFLAGS) $(ROOTLDFLAGS) -I$(ROOTINC)
else
@echo "NO ROOTSYS!"
endif
#--------configure linker------------#
LINKER   = gcc
# linking flags here
LFLAGS   = -Wall -I. -lm
LDFLAGS  = -shared
#--------configure compiler----------#
CXXFLAGS  += -O2 -Wall -Wno-write-strings
LIBS     = $(ROOTLIBS) -lm -lz -lutil -lnsl -lpthread -lTMVA

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all: App1 App2

App1: $(APP1).cxx $(OBJECTS)
	@echo "Compiling "$(APP1)"..."
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$(APP1) $(APP1).cxx $(OBJECTS) $(LIBS)
	@echo "-----------------------------------------------------"

App2: $(APP2).cxx $(OBJECTS)
	@echo "Compiling "$(APP2)"..."
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$(APP2) $(APP2).cxx $(OBJECTS) $(LIBS)
	@echo "-----------------------------------------------------"

clean:
	@rm -fv $(BINDIR)/$(APP1)
	@rm -fv $(BINDIR)/$(APP2)

