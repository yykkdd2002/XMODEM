# ---------------------------------------------------------------------------
#                   commands
# ---------------------------------------------------------------------------
CC := gcc
LINK := gcc
RM := rm -rf
MV := mv
TAR := tar
MKDIR := mkdir


# ---------------------------------------------------------------------------
#                   settings
# ---------------------------------------------------------------------------
SRC_SUFFIX := .c
OBJ_SUFFIX := .o
LIB_SUFFIX := .a
BIN_SUFFIX := .exe
DLL_SUFFIX := .so

INC_PREFIX := -I 
LIB_PREFIX := -L

OPT_C := -c 
OPT_OUT := -o 
OPT_LINKOUT := -o 
OPT_DEBUG := -g 

CFLAGS := $(OPT_C)
LIBFLAGS := -Debug

BIN_NAME=xmodem_sim

# ---------------------------------------------------------------------------
#                   directories
# ---------------------------------------------------------------------------
PRJ_DIR := $(PWD)
PRJ_SRC_DIR := src
PRJ_OBJ_DIR := obj
PRJ_INC_DIR := inc  ../../components/common/inc ../../components/lib/inc ../../components/hal/inc
PRJ_BIN_DIR := bin
PRJ_COMPONENTS_DIR := ../../components/lib ../../components/common
PRJ_LIB_DIR := ./lib /usr/local/lib /lib /usr/lib


# ---------------------------------------------------------------------------
#                   common settings
# ---------------------------------------------------------------------------
PRJ_SRCS := $(wildcard $(PRJ_SRC_DIR)/*$(SRC_SUFFIX))
PRJ_OBJS := $(patsubst $(PRJ_SRC_DIR)/%$(SRC_SUFFIX),$(PRJ_OBJ_DIR)/%$(OBJ_SUFFIX),$(PRJ_SRCS)) 
PRJ_INCS := $(addprefix $(INC_PREFIX),$(PRJ_INC_DIR))
PRJ_LIBS := $(addprefix $(LIB_PREFIX), $(PRJ_LIB_DIR)) $(LIBFLAGS)
TEMPFILES  := core core.* *$(OBJ_SUFFIX) temp.* *.out typescript*
FINAL_OBJS := $(wildcard $(PRJ_OBJ_DIR)/*$(OBJ_SUFFIX))
export PRJ_DIR PRJ_OBJ_DIR

# ---------------------------------------------------------------------------
#                   make rule
# ---------------------------------------------------------------------------

.PHONY: all clean

all: DIR_CHECK TARGET

DIR_CHECK:
	$(shell if [ ! -d $(PRJ_OBJ_DIR) ];then mkdir $(PRJ_OBJ_DIR);fi)
	$(shell if [ ! -d $(PRJ_BIN_DIR) ];then mkdir $(PRJ_BIN_DIR);fi)

TARGET:GEN_OBJ GEN_BIN

GEN_BIN:
	$(CC) $(OPT_LINKOUT) $(PRJ_BIN_DIR)/$(BIN_NAME) $(PRJ_OBJ_DIR)/*$(OBJ_SUFFIX) -pthread

GEN_OBJ:$(PRJ_COMPONENTS_DIR) $(PRJ_OBJS)

$(PRJ_COMPONENTS_DIR):DIR_ECHO
	make -C $@
DIR_ECHO:
	@echo $(PRJ_COMPONENTS_DIR)
	@echo begin compile

$(PRJ_OBJS):$(PRJ_OBJ_DIR)/%$(OBJ_SUFFIX):$(PRJ_SRC_DIR)/%$(SRC_SUFFIX)
	$(CC) $(OPT_DEBUG) $(PRJ_INCS) $(CFLAGS) $(OPT_OUT) $@ $<

clean:
	$(RM) $(FINAL_OBJS)
