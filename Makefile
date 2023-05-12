################################################################################
# Project: Téma 21 - Paralelizace - Maximální tok v síti                       #
# Course:  GAL - Graph Algorithms - FIT BUT                                     #
# Year:    2022/2023                                                           #
# Authors:                                                                     #
#          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz               #
#          Dostál Michal (xdosta51) - xdosta51@stud.fit.vutbr.cz               #
################################################################################

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
TARGET = GAL
CC = g++
CFLAGS = -fopenmp -std=c++11 -Wall -Wextra -pedantic -I$(INCDIR)

# paths
SRCDIR = $(PROJECT_ROOT)src/
BINDIR = $(PROJECT_ROOT)bin/
INCDIR = $(PROJECT_ROOT)include/

# compilation mode (debug, release)
ifeq ($(MODE),debug)
	CFLAGS += -g
	OBJDIR = $(PROJECT_ROOT)obj/debug/
else
	CFLAGS += -o3
	OBJDIR = $(PROJECT_ROOT)obj/
endif

SOURCES  := $(wildcard $(SRCDIR)*.cpp)
INCLUDES := $(wildcard $(SRCDIR)*.hpp)
OBJECTS  := $(SOURCES:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

rm = rm -f

.PHONY: all
all: $(BINDIR) $(OBJDIR) $(BINDIR)$(TARGET)

$(BINDIR):
	@mkdir -p $@

$(OBJDIR):
	@mkdir -p $@

# linker
$(BINDIR)$(TARGET): $(OBJECTS)
	@$(CC) $(CFLAGS) $(OBJECTS) -o $@
	@echo "Linking complete!"

# compiler
$(OBJECTS): $(OBJDIR)%.o : $(SRCDIR)%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!" | sed 's:$(SRCDIR)::'

.PHONY: run
run: all
	@$(BINDIR)$(TARGET)

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@$(rm) -r $(OBJDIR)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)$(TARGET)
	@$(rm) -r $(BINDIR)
	@echo "Executable removed!"

############################### END OF MAKEFILE ###############################
