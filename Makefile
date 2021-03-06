# version of this Makefile
MAKEFILE_VER=		0.3.7_debug


# make options
MAKEFLAGS+=			-s



# OBS: search for 'CUSTOM' text




# CUSTOM output
OUT_FILE=			b_tree_plus

# CUSTOM paths
INC_DIR=			inc
SRC_DIR=			src
LIB_DIR=			lib



# basic path
BUILD_DIR=			BUILD
RELEASE_DIR=		$(BUILD_DIR)/release
DEBUG_DIR=			$(BUILD_DIR)/debug



# compiler and linker
CC=					g++
LD=					g++

# rm
RM=					rm -f



# flags
RELEASE_FLAGS=		-O2
DEBUG_FLAGS=		-O0 -ggdb

CFLAGS=				-Wall -pipe
CC_FLAGS=			$(CFLAGS) -I$(INC_DIR)
LD_FLAGS=			-L$(LIB_DIR)



# commands
ifeq ($(MAKECMDGOALS), debug)
	OUT_DIR=		$(DEBUG_DIR)
	CC_COMMAND=		$(CC) $(DEBUG_FLAGS) $(CC_FLAGS)
	LD_COMMAND=		$(LD) $(DEBUG_FLAGS) $(LD_FLAGS)
else
	OUT_DIR=		$(RELEASE_DIR)
	CC_COMMAND=		$(CC) $(RELEASE_FLAGS) $(CC_FLAGS)
	LD_COMMAND=		$(LD) $(RELEASE_FLAGS) $(LD_FLAGS)
endif


# colors
NORMAL=				"\\033[0;39m"
GREEN=				"\\033[1;32m"
WHITE=				"\\033[1;37m"
YELLOW=				"\\033[1;33m"
RED=				"\\033[1;31m"
BLUE=				"\\033[1;34m"
CYAN=				"\\033[1;36m"
PINK=				"\\033[1;35m"

SPACE=				"\\033[70G"



# output
ECHO_DONE=			$(SPACE)$(GREEN)DONE$(NORMAL)
ECHO_SHIT=			$(SPACE)${RED}SHIT$(NORMAL)
CHECK=				&& echo $(ECHO_DONE) || (echo && echo $(ECHO_SHIT) && exit 1)




################################################################################
#--- main target
all:				run

compile:				pre INFO INFO_CC OBJECTS INFO_LD
	@echo -n ${GREEN}"   + "${WHITE} $(OUT_DIR)"/*.o"$(NORMAL) -o $(OUT_DIR)/$(OUT_FILE)
	$(LD_COMMAND) $(OUT_DIR)/*.o -o $(OUT_DIR)/$(OUT_FILE) $(CHECK)

run:				compile
	@echo
	@echo "Hint: run with './$(OUT_DIR)/$(OUT_FILE)'"
	@echo

debug:				compile
	@echo
	@echo "Hint: debug with 'ddd $(OUT_DIR)/$(OUT_FILE)'"
	@echo



################################################################################
#--- CUSTOM dependence files

OBJECTS:			MAIN

MAIN:				NODE $(OUT_DIR)/main.o
NODE:				INEXDATASTRUCTURE LEAF $(INC_DIR)/Node.h $(OUT_DIR)/Node.o
LEAF:				INEXDATASTRUCTURE $(INC_DIR)/Leaf.h $(OUT_DIR)/Leaf.o
INEXDATASTRUCTURE:	$(INC_DIR)/IndexDataStructure.h $(OUT_DIR)/IndexDataStructure.o



################################################################################
#--- redefine the implicit rule to compile
$(OUT_DIR)/%.o:	$(SRC_DIR)/%.cpp
	@echo -n ${GREEN}"   + "${WHITE}$(SRC_DIR)/$*.cpp$(NORMAL) -o $(OUT_DIR)/$*.o
	$(CC_COMMAND) -c $(SRC_DIR)/$*.cpp -o $(OUT_DIR)/$*.o $(CHECK)



################################################################################
pre:
	mkdir -p $(RELEASE_DIR)
	mkdir -p $(DEBUG_DIR)



################################################################################
clean:
	@echo
	@echo $(YELLOW)"CLEAN:"$(NORMAL)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(RELEASE_DIR)"/*.o" $(RELEASE_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(RELEASE_DIR)/*.o $(RELEASE_DIR)/$(OUT_FILE)$(CHECK)
	@echo -n ${RED}"   - "${WHITE}$(RM) $(DEBUG_DIR)"/*.o" $(DEBUG_DIR)/$(OUT_FILE)$(NORMAL)
	$(RM) $(DEBUG_DIR)/*.o $(DEBUG_DIR)/$(OUT_FILE)$(CHECK)
	@echo



################################################################################
INFO:
	@echo
	@echo $(YELLOW)"Variabili usate per la compilazione:"$(NORMAL)
	@echo CC_FLAGS=	$(CC_FLAGS)
	@echo LD_FLAGS=	$(LD_FLAGS)
	@echo
	@echo

INFO_CC:
	@echo
	@echo $(YELLOW)"Comando usato per la compilazione:"$(NORMAL)
	@echo    $(BLUE)$(CC_COMMAND) -c$(NORMAL)

INFO_LD:
	@echo
	@echo $(YELLOW)"Comando usato per il linking:"$(NORMAL)
	@echo    $(BLUE)$(LD_COMMAND)$(NORMAL)





# End
