/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:06:41 by omizin            #+#    #+#             */
/*   Updated: 2025/07/11 17:57:14 by vpushkar         ###   ########.fr       */
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
\033[1m\033[36mSuPuShell\033[0m\
\033[1m\033[34m git:(\033[0m\
\033[1m\033[31mmaster\033[0m\
\033[1m\033[34m)\033[0m\
\033[1m\033[33m ✗ \033[0m"
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

// extern volatile sig_atomic_t	g_signal_interrupt;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_shell
{
	t_env	*env;
	int		last_exit_status;
}	t_shell;

typedef struct s_input
{
	const char	*line;
	int			i;
	t_shell		*shell;
	t_env		*env;
	char		**args;
	bool		syntax_ok;
	char		*infile;
	char		*outfile;
	bool		append;
	char		*heredoc;
}	t_input;


//free_functions
void	free_input(t_input *input);
void	free_env_list(t_env *env);
void	free_args(char **args);
void	free_at_exit(t_input *input, t_env **env);
//helpers
char	*substr_dup(const char *start, const char *end);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_strjoin_free(char *s1, const char *s2);
char	*ft_strcpy(char *dest, const char *src);
char	*ft_strcat(char *dest, const char *src);
//env heplerrs
void	update_or_add_env_var(t_env **env, char *key, char *val);
int		parse_export_argument(char *arg, char **key, char **val);
void	remove_env_var(t_env **env, const char *key);
t_env	*create_env(char **envp);
char	*get_env_value(t_env *env, const char *key);

// signals.c
void	setup_signal(void);
void	disable_echoctl(void);
//commands
void	do_pwd(void);
void	do_cd(char **commands, t_env **env);
void	do_export(char **argv, t_env **env);
void	do_unset(char **argv, t_env **env);
void	do_env(t_env *env);
//commands_helpers
void	goto_prev_dir(t_env **env);
char	**build_envp(t_env *env);
char	*ft_strsep(char **str, const char *sep);
char	*search_path(const char *cmd, t_env *env);
//commands_second_part
void	do_echo(char **argv);
void	run_external_command(char **argv, t_env *env, t_input *input);
//small_helpers
int		check_for_input(char *line);
int		ft_isspace(char c);
void	skip_spaces(t_input *input);
char	**append_arg(char **args, char *new_arg);

//billy
void	billy_print(void);

#endif
