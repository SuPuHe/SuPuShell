/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:06:41 by omizin            #+#    #+#             */
/*   Updated: 2025/07/08 13:50:58 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "ft_printf.h"
# include "get_next_line.h"
# include "libft.h"

# define RESET   "\033[0m"
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE    "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN    "\033[36m"
# define WHITE   "\033[37m"
# define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
# define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
# define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
# define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
# define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
# define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
# define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
# define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

//# define SHELLNAME "\033[1m\033[32m➜  \033[0m\033[1m\033[36mSuPuShell \033[0m\033[1m\033[34mgit:(\033[0m\033[1m\033[31mmaster\033[0m\033[1m\033[34m)\033[0m\033[1m\033[33m ✗ \033[0m"
# define SHELLNAME \
"\033[1m\033[32m➜  \033[0m\
\033[1m\033[36mSuPuShell  \033[0m \
\033[1m\033[34m git:( \033[0m \
\033[1m\033[31m  master \033[0m \
\033[1m\033[34m ) \033[0m \
\033[1m\033[33m  ✗  \033[0m"
//readline + history
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
//malloc / free / printf
# include <stdio.h>
# include <stdlib.h>
// open / read / write / close
# include <fcntl.h>
# include <unistd.h>
//fork / execve / wait
# include <unistd.h>
# include <sys/wait.h>
// dup2 / pipe
# include <unistd.h>
# include <stdio.h>
//signal / kill / sigaction
# include <signal.h>
# include <unistd.h>
# include <stdio.h>
// access / stat
# include <unistd.h>
# include <sys/stat.h>
# include <stdio.h>
// opendir / readdir
# include <dirent.h>
# include <stdio.h>
//getcwd / chdir
# include <unistd.h>
# include <stdio.h>
// isatty / ttyname
# include <stdio.h>
# include <unistd.h>
//getenv / export
# include <stdio.h>
# include <stdlib.h>
//tcgetattr / tcsetattr
# include <termios.h>
# include <stdio.h>
# include <unistd.h>
//tgetent / tgetstr / tputs (termcap)
# include <term.h>
# include <unistd.h>
//bool
# include <stdbool.h>

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_input
{
	const char	*line;
	int			i;
	t_env		*env;
	char		**args;
	bool		syntax_ok;
}	t_input;

//helpers
void	free_env_list(t_env *env);
void	free_args(char **args);
char	**split_args(const char *input);
char	*substr_dup(const char *start, const char *end);
int		ft_strcmp(const char *s1, const char *s2);
//env heplerrs
void	update_or_add_env_var(t_env **env, char *key, char *val);
int		parse_export_argument(char *arg, char **key, char **val);
void	remove_env_var(t_env **env, const char *key);
t_env	*create_env(char **envp);
char	*ft_strjoin_free(char *s1, const char *s2);

// signals.c
void	setup_signal(void);
void	disable_echoctl(void);

#endif
