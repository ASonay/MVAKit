NAME=tmva_trainer
EXEC=$(NAME)

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

App: $(NAME).cpp $(OBJECTS)
	@echo "Comiling App..."
	$(CXX) $(CXXFLAGS) -o $(NAME) $(NAME).cpp $(OBJECTS) $(LIBS)
	@echo "-----------------------------------------------------"

clean:
	@rm -fv $(EXEC)

