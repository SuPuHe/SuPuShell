/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:06:41 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 15:21:40 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "ft_printf.h"
# include "get_next_line.h"
# include "libft.h"

# include "cf.h"

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

# define SHELLNAME \
"\001\033[1m\033[32m\002➜  \001\033[0m\002" \
"\001\033[1m\033[36m\002SuPuShell\001\033[0m\002" \
"\001\033[1m\033[34m\002 git:(\001\033[0m\002" \
"\001\033[1m\033[31m\002master\001\033[0m\002" \
"\001\033[1m\033[34m\002)\001\033[0m\002" \
"\001\033[1m\033[33m\002 ✗ \001\033[0m\002"
//readline + history
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
//malloc / free / printf
# include <stdlib.h>
// open / read / write / close
# include <fcntl.h>
# include <unistd.h>
//fork / execve / wait
# include <sys/wait.h>
// dup2 / pipe
//signal / kill / sigaction
# include <signal.h>
// access / stat
# include <sys/stat.h>
// opendir / readdir
# include <dirent.h>
//getenv / export
# include <stdlib.h>
//tcgetattr / tcsetattr
# include <termios.h>
//tgetent / tgetstr / tputs (termcap)
# include <term.h>
//bool
# include <stdbool.h>

# include <dirent.h>

extern volatile sig_atomic_t	g_sigint_exit_status;

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
	int		should_exit;
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

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_SINGLE_QUOTE_WORD,
	TOKEN_DOUBLE_QUOTE_WORD,
	TOKEN_PIPE,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_AMPERSAND,
	TOKEN_END
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	bool			has_space;
}	t_token;

typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_AND,
	NODE_OR,
	NODE_SUBSHELL
}	t_node_type;

typedef struct s_ast_node
{
	t_node_type			type;
	struct s_ast_node	*left;
	struct s_ast_node	*right;
	t_input				*command;
}	t_ast_node;

typedef struct s_string_builder
{
	char	*str;
	size_t	len;
	size_t	capacity;
}	t_string_builder;

typedef struct s_expand_ctx
{
	t_string_builder	*sb;
	const char			*str;
	t_env				*env;
	t_shell				*shell;
	int					*i;
}	t_expand_ctx;

typedef struct s_input_check
{
	int	d_quot;
	int	s_quot;
	int	l_par;
	int	r_par;
}	t_input_check;

//free_functions
void	free_input(t_input *input);
void	free_env_list(t_env *env);
void	free_args(char **args);
void	free_at_exit(t_input *input, t_env **env);
void	free_ast(t_ast_node *node);
//helpers
char	*substr_dup(const char *start, const char *end);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_strjoin_free(char *s1, const char *s2);
char	*ft_strcpy(char *dest, const char *src);
char	*ft_strcat(char *dest, const char *src);
//env_helpers
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
//commands_helpers
void	goto_prev_dir(t_env **env);
char	**build_envp(t_env *env);
//path_helpers
char	*ft_strsep(char **str, const char *sep);
char	*search_path(const char *cmd, t_env *env);
//commands_second_part
void	do_echo(char **argv);
void	run_external_command(char **argv, t_env *env);
void	do_env(t_env *env);
//small_helpers
int		check_for_input(char *line);
int		ft_isspace(char c);
char	**append_arg(char **args, char *new_arg);
// wildcards
char	**expand_wildcards(const char *pattern);
void	free_expanded_wildcards(char **expanded);
// wildcars_second_part
int		matches_pattern(const char *str, const char *pattern);
int		count_matches(const char *pattern);
char	**create_result_array(int size);
char	**handle_no_wildcards(const char *pattern);
//billy
void	billy_print(void);

//ast.c
t_ast_node		*create_cmd_node(t_input *cmd);
t_ast_node		*create_binary_node(t_node_type type,
			t_ast_node *left, t_ast_node *right);
t_ast_node		*create_subshell_node(t_ast_node *child);
void			free_ast(t_ast_node *node);

//executor_main.c
bool	is_builtin_command(const char *command);
int		execute_external_command(t_input *command, t_shell *shell);
int		execute_builtin_command(t_input *command, t_shell *shell);
int		execute_command_node(t_ast_node *node, t_shell *shell);
int		execute_node(t_ast_node *node, t_shell *shell);

//executor_second_part.c
void	execute_external_child(t_input *command, t_shell *shell);

//executor_other.c
int		execute_and_node(t_ast_node *node, t_shell *shell);
int		execute_or_node(t_ast_node *node, t_shell *shell);
void	execute_subshell_child(t_ast_node *node, t_shell *shell);
int		execute_subshell_node(t_ast_node *node, t_shell *shell);

//executor_pipe.c
void	execute_pipe_child_right(t_ast_node *node, t_shell *shell, int *pipefd);
void	execute_pipe_child_left(t_ast_node *node, t_shell *shell, int *pipefd);
int		execute_pipe_node(t_ast_node *node, t_shell *shell);

//expander_utils.c
void	expand_digit(t_string_builder *sb, const char *str, int *i);
void	expand_other(t_string_builder *sb);
void	expand_env_var(t_string_builder *sb,
			const char *str, t_env *env, int *i);
bool	is_operator(char c);

//expander.c
void	expand_status(t_string_builder *sb, t_shell *shell, int *i);
void	expand_loop(const char *str, t_expand_ctx *ctx);
void	expand_hub(t_expand_ctx *ctx);
char	*expand_string_variables(const char *str, t_env *env, t_shell *shell);
char	*expand_token_value(t_token *token, t_env *env, t_shell *shell);

//parser_command.c
bool		is_command_terminator(t_token *token);
void		init_input_structure(t_input *input, t_env *env, t_shell *shell);
t_ast_node	*parse_primary_subshell(t_list **tokens, t_shell *shell);
t_ast_node	*parse_primary_command(t_list **tokens, t_shell *shell);
t_input		parse_command_from_tokens(t_list **current_tokens, t_env *env, t_shell *shell);

//parser_main.c
t_ast_node	*parse_primary(t_list **tokens, t_shell *shell);
t_ast_node	*parse_pipeline(t_list **tokens, t_shell *shell);
t_ast_node	*parse_and_or(t_list **tokens, t_shell *shell);
t_ast_node	*parse_expression(t_list **tokens, t_shell *shell);
t_ast_node	*parse(const char *line, t_shell *shell);

//parser_redirection_handlers.c
void	apply_output_redirection(t_input *input,
			t_token_type redir_type, char *expanded_value);
void	apply_input_redirection(t_input *input, char *expanded_value);
bool	apply_redirections(t_input *input);
bool	apply_input_redirections(t_input *input);
bool	apply_output_redirections(t_input *input);

//parser_redirection.c
char	*expand_filename_token(t_token *filename_token,
			t_env *env, t_shell *shell);
void	handle_heredoc_redirection(t_input *input, t_token *filename_token);
bool	validate_redirection_tokens(t_list **current_tokens, t_input *input);
bool	handle_redirection_token(t_input *input,
			t_list **current_tokens, t_env *env, t_shell *shell);
bool	handle_heredoc(t_input *input);

//parser_word_second_part.c
bool	is_adjacent_word_token(t_token *token);

//parser_word.c
void	handle_heredoc_fallback(t_input *input);
void	handle_regular_word(t_input *input, char *expanded_value);
void	handle_wildcard_expansion(t_input *input,
			t_token *current_tok, char *expanded_value);
char	*concatenate_adjacent_tokens(t_list **current_tokens,
			char *expanded_value, t_env *env, t_shell *shell);
bool	handle_word_token(t_input *input,
			t_list **current_tokens, t_env *env, t_shell *shell);

//string_builder.c
t_string_builder	*sb_create(void);
void				sb_append_char(t_string_builder *sb, char c);
void				sb_append(t_string_builder *sb, const char *s);
char				*sb_build_and_destroy(t_string_builder *sb);

//tokenizer_handlers.c
char	*extract_non_quoted_word(const char *line, int *i);
char	*extract_subsegment(const char *line, int *i, char delimiter);
t_token	*handle_quote_tokens(const char *line, int *i, t_list **tokens);
t_token	*handle_redirection_tokens(const char *line, int *i);
t_token	*handle_operator_tokens(const char *line, int *i);

//tokenizer_main_second_part.c
t_token	*process_next_token(const char *line,
			int *i, t_list **tokens_list, bool *had_space);
void	add_end_token(t_list **tokens);

//tokenizer_main.c
void	skip_whitespace(const char *line, int *i, bool *had_space);
void	add_token_to_list(t_token *new_token, bool had_space, t_list **tokens);
void	free_token_list(t_list *tokens);
t_token	*create_token(t_token_type type, const char *value);
t_list	*tokenize(const char *line);

//input_checker.c
void	handle_quote_char(char c,
			t_input_check *check, bool *in_squote, bool *in_dquote);
bool	handle_non_quote_syntax(char c,
			t_input_check *check, bool in_squote, bool in_dquote);
bool	process_line_for_syntax_errors(const char *line,
			t_input_check *check, bool *in_squote, bool *in_dquote);
int		check_for_input(char *line);


#endif
