/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:06:41 by omizin            #+#    #+#             */
/*   Updated: 2025/08/20 11:35:53 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "ft_printf.h"
# include "get_next_line.h"
# include "libft.h"
# include "cf.h"

# include "executor.h"
# include "parser.h"
# include "tokenizer.h"
# include "expander.h"
# include "utils.h"
# include "commands.h"
# include "types.h"
# include "colors.h"
# include "helpers.h"
# include "wildcards.h"
# include "string_builder.h"
# include "ast.h"
# include "input_checker.h"
# include "signals.h"

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <dirent.h>
# include <termios.h>
# include <term.h>
# include <stdbool.h>

#endif
