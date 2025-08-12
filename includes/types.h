/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:12:44 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/12 16:42:15 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# include <stdbool.h>

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
	char		**all_outfiles;
	bool		*all_outfiles_append_flags;
	int			outfiles_count;
	char		*infile;
	char		*outfile;
	bool		append;
	char		**heredoc_delimiters;
	int			heredoc_count;
	bool		heredoc_processed;
	bool		*heredoc_is_quoted;
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
	bool				quotes;
}	t_expand_ctx;

typedef struct s_input_check
{
	int	d_quot;
	int	s_quot;
	int	l_par;
	int	r_par;
}	t_input_check;

typedef struct s_cf_node
{
	void				*ptr;
	struct s_cf_node	*next;
}	t_cf_node;

#endif
