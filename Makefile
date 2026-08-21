# Portable build for the recovered extSDDiP source.
#
# This mirrors the first compile command in historical_source/compilar.txt
# byte-for-byte (same source list, same flags, same link order); only the
# CPLEX installation location is turned into a variable instead of being
# hard-coded to an author's machine. See docs/build_and_run.md.
#
# Usage:
#   make CPLEX_ROOT=/opt/ibm/ILOG/CPLEX_Studio128
#
# The historical build used IBM ILOG CPLEX Studio 12.8 on 64-bit Linux.
# Other 12.x installations with the standard cplex/concert directory layout
# should also work; override CPLEX_ARCH/CPLEX_LIBFORMAT if yours differs.

CPLEX_ROOT ?=
CPLEX_ARCH ?= x86-64_linux
CPLEX_LIBFORMAT ?= static_pic

SRC_DIR := src
BUILD_DIR := build
BIN := $(BUILD_DIR)/extSDDiP

# Same 12 translation units as historical_source/compilar.txt's first
# command; Pre_Solving_Tree.cpp is an incomplete legacy helper that the
# historical build also excludes (see docs/recovered_code_validation.md).
SOURCES := $(SRC_DIR)/Main.cpp $(SRC_DIR)/ReadData.cpp $(SRC_DIR)/GlobalVariables.cpp \
           $(SRC_DIR)/Pre_Solving.cpp $(SRC_DIR)/NodalModel.cpp $(SRC_DIR)/LinearModel.cpp \
           $(SRC_DIR)/LagrangianModel.cpp $(SRC_DIR)/DualLagrangianModel.cpp \
           $(SRC_DIR)/CutGeneration.cpp $(SRC_DIR)/GlobalModel.cpp \
           $(SRC_DIR)/UpperBounds.cpp $(SRC_DIR)/TreeInequalities.cpp

CXX := g++
CXXFLAGS := -DIL_STD -w -std=c++11

CPLEX_INCLUDE_DIRS ?= -I$(CPLEX_ROOT)/cplex/include -I$(CPLEX_ROOT)/cplex/include/ilcplex -I$(CPLEX_ROOT)/concert/include
CPLEX_LIB_DIRS ?= -L$(CPLEX_ROOT)/cplex/lib/$(CPLEX_ARCH)/$(CPLEX_LIBFORMAT) -L$(CPLEX_ROOT)/concert/lib/$(CPLEX_ARCH)/$(CPLEX_LIBFORMAT)
CPLEX_LIBS := -lilocplex -lconcert -lcplex -lm -lpthread -ldl

.PHONY: all clean
all: $(BIN)

$(BIN): $(SOURCES) $(SRC_DIR)/Header.h
	@if [ -z "$(CPLEX_ROOT)" ]; then \
	  echo "CPLEX_ROOT is not set. Usage: make CPLEX_ROOT=/path/to/CPLEX_Studio128"; \
	  exit 1; \
	fi
	mkdir -p $(BUILD_DIR)
	$(CXX) $(SOURCES) -o $(BIN) $(CPLEX_INCLUDE_DIRS) $(CPLEX_LIB_DIRS) $(CPLEX_LIBS) $(CXXFLAGS)

clean:
	rm -rf $(BUILD_DIR)
