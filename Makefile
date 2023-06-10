NAME = minishell

CC = gcc

CFLAGS = -Wall -Wextra -Werror -g -I includes/ -I libft/includes/

LIBFT = -L libft -lft

HEADER = minishell.h

AST = 

# BUILTINS = cd echo env exit export pwd unset
BUILTINS = env

ENV = init_env get_env set_env

EXEC = 

SHELL_DATA = init_data

MAIN = main

PARSING = token

UTILS = free

SRC = $(addsuffix .c, $(addprefix src/ast/, $(AST))) \
	  $(addsuffix .c, $(addprefix src/builtins/, $(BUILTINS))) \
	  $(addsuffix .c, $(addprefix src/env/, $(ENV))) \
	  $(addsuffix .c, $(addprefix src/exec/, $(EXEC))) \
	  $(addsuffix .c, $(addprefix src/shell_data/, $(SHELL_DATA))) \
	  $(addsuffix .c, $(addprefix src/main/, $(MAIN))) \
	  $(addsuffix .c, $(addprefix src/parsing/, $(PARSING))) \
	  $(addsuffix .c, $(addprefix src/utils/, $(UTILS))) \

OBJ = $(SRC:c=o)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "\n"
	@make -C libft/
	@echo "\033[0;32mCompiling minishell..."
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBFT)
	@echo "\n\033[0mDone !"

%.o: %.c
	@printf "\033[0;33mGenerating minishell objects... %-33.33s\r" $@
	@${CC} ${CFLAGS} -c $< -o $@

clean:
	@echo "\033[0;31mCleaning libft..."
	@make clean -C libft/
	@echo "\nRemoving binaries..."
	@rm -f $(OBJ)
	@echo "\033[0m"

fclean:
	@echo "\033[0;31mCleaning libft..."
	@make fclean -C libft/
	@echo "\nDeleting objects..."
	@rm -f $(OBJ)
	@echo "\nDeleting executable..."
	@rm -f $(NAME)
	@echo "\033[0m"

re: fclean all

test: all
	./minishell

norm:
	norminette $(SRC) includes/$(HEADER)

.PHONY: clean fclean re test norm