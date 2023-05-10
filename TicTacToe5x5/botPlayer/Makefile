# SHELL ALLIASSES
RM := rm -rf

# LOCAL FUNCTIONS
define print_cc
	$(if $(Q), @echo "[CC]        $(1)")
endef

define print_bin
	$(if $(Q), @echo "[BIN]       $(1)")
endef

define print_rm
    $(if $(Q), @echo "[RM]        $(1)")
endef

# make V=1 --> verbose, otherwise silence
ifeq ("$(origin V)", "command line")
	Q :=
else
	Q ?= @
endif

# PROJECT STRUCTURE
SDIR := ./src
IDIR := ./include
ADIR := ./app

# FILES
SRC := $(wildcard $(SDIR)/*.cpp)
ASRC := $(SRC) $(wildcard $(ADIR)/*.cpp)
AOBJ := $(ASRC:%.cpp=%.o)
OBJ := $(AOBJ)

DEPS := $(OBJ:%.o=%.d)

LIB :=

# EXEC
EXEC := main.out

# By default use g++
CXX ?= g++

CXXSTD := -std=c++17

DEP_FLAGS := -MMD -MP

H_INC := $(foreach d, $(IDIR), -I$d)
L_INC := $(foreach l, $(LIB), -l$l)

ifeq ($(CXX),clang++)
	CXXFLAGS += -Weverything -pedantic -Wno-c++98-compat
else
	CXXFLAGS += -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic
endif

ifeq ("$(origin O)", "command line")
	OPT := -O$(O)
else
	OPT := -O3
endif

ifeq ("$(origin G)", "command line")
	GGDB := -ggdb$(G)
else
	GGDB :=
endif

CXXFLAGS += $(CXXSTD) $(OPT) $(GGDB) $(DEP_FLAGS)

all: $(EXEC)


$(EXEC): $(AOBJ)
	$(call print_bin,$@)
	$(Q)$(CXX) $(CXXFLAGS) $(H_INC) $(AOBJ) -o $@ $(L_INC)


%.o:%.cpp %.d
	$(call print_cc,$<)
	$(Q)$(CXX) $(CXXFLAGS) $(H_INC) -c $< -o $@

clean:
	$(call print_rm,EXEC)
	$(Q)$(RM) $(EXEC)
	$(call print_rm,OBJ)
	$(Q)$(RM) $(OBJ)
	$(call print_rm,DEPS)
	$(Q)$(RM) $(DEPS)

$(DEPS):


include $(wildcard $(DEPS))