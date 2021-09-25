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


# ---------------------------------------------------------------------------
#                   directories
# ---------------------------------------------------------------------------
SRC_DIR := src
#OBJ_DIR := obj
OBJ_DIR := $(PRJ_DIR)/$(PRJ_OBJ_DIR)
INC_DIR := inc ../common/inc ../hal/inc
LIB_DIR := lib /usr/local/lib /lib /usr/lib


# ---------------------------------------------------------------------------
#                   common settings
# ---------------------------------------------------------------------------
SRCS := $(wildcard $(SRC_DIR)/*$(SRC_SUFFIX))
OBJS  := $(patsubst $(SRC_DIR)/%$(SRC_SUFFIX),$(OBJ_DIR)/%$(OBJ_SUFFIX),$(SRCS)) 
INCS := $(addprefix $(INC_PREFIX), $(INC_DIR))
LIBS := $(addprefix $(LIB_PREFIX), $(LIB_DIR)) $(LIBFLAGS)
TEMPFILES  := core core.* *$(OBJ_SUFFIX) temp.* *.out typescript*


# ---------------------------------------------------------------------------
#                   make rule
# ---------------------------------------------------------------------------

.PHONY: all clean

all: CHECK_DIR $(OBJS)

clean:
	$(RM) $(OBJS)

CHECK_DIR:
	$(shell if [ ! -d $(OBJ_DIR) ];then mkdir $(OBJ_DIR);fi)

$(OBJS):$(OBJ_DIR)/%$(OBJ_SUFFIX):$(SRC_DIR)/%$(SRC_SUFFIX)
	$(CC) $(OPT_DEBUG) $(CFLAGS) $(INCS) $(OPT_OUT)$@ $<
