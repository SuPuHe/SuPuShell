# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/30 10:05:20 by omizin            #+#    #+#              #
#    Updated: 2025/08/19 16:48:33 by omizin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

RESET_COLOR	= \033[0m			# Reset to default color
YELLOW		= \033[1;33m		# Brighter yellow
BLUE		= \033[1;34m		# Bright blue
GREEN		= \033[1;32m		# Bright green
RED			= \033[1;31m		# Bright red
CYAN		= \033[1;36m		# Bright cyan

NAME = minishell

SUPULIB_DIR = SuPuLib
SUPULIB_REPO = https://github.com/SuPuHe/SuPuLib.git
SRCS_DIR = srcs
OBJS_DIR = objs
INCS_DIR = includes

INCLUDE = -I$(INCS_DIR) -I$(SUPULIB_DIR)/libft/includes -I$(SUPULIB_DIR)/ft_printf/includes -I$(SUPULIB_DIR)/get_next_line/includes
CC = cc
CFLAGS = -Wall -Wextra -Werror $(INCLUDE)
LDFLAGS = -lreadline

RM = rm -rf

SRCS = minishell.c helpers.c env_helpers_1.c signals.c commands_1.c \
commands_helpers.c commands_2.c small_helpers.c billy.c \
wildcards_1.c centralized_free.c cf_libft.c centralized_alloc.c path_helpers.c \
wildcards_2.c executor_main_2.c executor_main_1.c \
executor_other.c executor_pipe_1.c expander_utils.c expander.c parser_command.c \
parser_main.c parser_redirection_handlers_1.c string_builder_1.c ast.c \
parser_redirection_1.c parser_word_2.c parser_word_1.c \
tokenizer_handlers.c tokenizer_main_1.c tokenizer_main_2.c \
input_checker.c string_builder_2.c parser_redirection_2.c \
parser_redirection_handlers_2.c parser_redirection_3.c \
parser_redirection_4.c parser_redirection_5.c executor_pipe_2.c \
env_helpers_2.c commands_3.c minishell_helper.c


SRCS := $(addprefix $(SRCS_DIR)/, $(SRCS))

OBJS = $(SRCS:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.o)

all: $(SUPULIB_DIR)/SuPuLib.a $(NAME)

$(SUPULIB_DIR):
	@git clone $(SUPULIB_REPO) $(SUPULIB_DIR)
	@echo "$(GREEN)SuPuLib cloned successfully$(RESET_COLOR)"

$(SUPULIB_DIR)/SuPuLib.a: | $(SUPULIB_DIR)
	@$(MAKE) -C $(SUPULIB_DIR)

# Compile minishell
$(NAME): $(SUPULIB_DIR)/SuPuLib.a $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(SUPULIB_DIR)/SuPuLib.a -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled successfully$(RESET_COLOR)"

# Compile object files
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c | $(OBJS_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

bonus: all

clean:
	@$(RM) $(OBJS_DIR)
	@$(MAKE) -C $(SUPULIB_DIR) clean
	@echo "$(GREEN)Clean sucessfully$(RESET_COLOR)"

fclean: clean
	@$(RM) $(NAME)
	@$(MAKE) -C $(SUPULIB_DIR) fclean
	@echo "$(GREEN)Fclean sucessfully$(RESET_COLOR)"

re: fclean all

.PHONY: all clean fclean re bonus
