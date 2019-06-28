#!/usr/bin/make -f

################################################################################
# SETTINGS                                                                     #
################################################################################

NAME = libft.a
AR = ar
ARFLAGS = -rcs
AS = nasm
ASFLAGS += -f macho64
CC = gcc
CFLAGS += -Wall -Werror -Wextra
CFLAGS += -Ofast
CFLAGS += #-g -fsanitize=address
INC = -I inc
SRC_DIR = src
OBJ_DIR = obj

# [ BASE ]

SRC = \
	ft_memalloc\
	ft_memcmp\
	ft_memcpy\
	ft_memset\
	ft_strchr\
	ft_strcmp

# [ OTHER ]

SRC += \
	getopt/ft_getopt

OBJ = $(patsubst %, $(OBJ_DIR)/%.o, $(SRC))

################################################################################
# COLORS                                                                       #
################################################################################

NC = \033[0m
GREEN = \033[1;32m
RED = \033[1;31m
YELLOW = \033[1;33m

################################################################################
# RULES                                                                        #
################################################################################

all: $(NAME)

$(NAME): $(OBJ)
	@$(AR) $(ARFLAGS) $@ $(OBJ)
	@echo "$(GREEN)DONE$(NC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)$(NAME) removed$(NC)"

re: fclean all
