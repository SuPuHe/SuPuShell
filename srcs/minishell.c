/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 10:19:13 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Prototypes for helper functions
static int  execute_builtin_command(t_input *command, t_shell *shell);
static int  execute_external_command(t_input *command, t_shell *shell);
static int  execute_command_node(t_ast_node *node, t_shell *shell);
static int  execute_pipe_node(t_ast_node *node, t_shell *shell);
static void execute_pipe_child_left(t_ast_node *node, t_shell *shell, int *pipefd);
static void execute_pipe_child_right(t_ast_node *node, t_shell *shell, int *pipefd);
static int  execute_subshell_node(t_ast_node *node, t_shell *shell);
static void execute_subshell_child(t_ast_node *node, t_shell *shell);
static int  execute_and_node(t_ast_node *node, t_shell *shell);
static int  execute_or_node(t_ast_node *node, t_shell *shell);
static bool is_builtin_command(const char *command);
int	execute_node(t_ast_node *node, t_shell *shell);

bool	apply_redirections(t_input *input);

bool	is_operator(char c);
t_ast_node	*parse_expression(t_list **tokens, t_shell *shell);
void	free_ast(t_ast_node *node);
void	free_token_list(t_list *tokens);

bool	handle_heredoc(t_input *input)
{
	char	*line;
	int		fd;
	char	*filename = "/tmp/.minishell_heredoc";

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (perror("heredoc file"), false);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, input->heredoc) == 0)
			break;
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	input->infile = cf_strdup(filename);
	return (true);
}

//part of apply_redirections
bool	apply_input_redirections(t_input *input)
{
	int	fd;

	if (input->heredoc)
	{
		if (!handle_heredoc(input))
			return (false);
	}
	if (input->infile)
	{
		fd = open(input->infile, O_RDONLY);
		if (fd < 0)
		{
			perror(input->infile);
			return (false);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	return (true);
}

//part of apply_redirections
bool	apply_output_redirections(t_input *input)
{
	int	fd;
	int	flags;

	if (input->outfile)
	{
		flags = O_CREAT | O_WRONLY;
		if (input->append)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;
		fd = open(input->outfile, flags, 0644);
		if (fd < 0)
		{
			perror(input->outfile);
			return (false);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	return (true);
}

bool	apply_redirections(t_input *input)
{
	if (!apply_input_redirections(input))
		return (false);
	if (!apply_output_redirections(input))
		return (false);
	return (true);
}

t_string_builder	*sb_create(void)
{
	t_string_builder	*sb;

	sb = cf_malloc(sizeof(t_string_builder));
	if (!sb)
		return (NULL);
	sb->str = cf_strdup("");
	if (!sb->str)
	{
		cf_free_one(sb);
		return (NULL);
	}
	sb->len = 0;
	sb->capacity = 1;
	return (sb);
}

void	sb_append_char(t_string_builder *sb, char c)
{
	size_t	new_capacity;
	char	*new_str;

	if (sb->len + 1 >= sb->capacity)
	{
		new_capacity = sb->capacity * 2;
		new_str = cf_realloc(sb->str, new_capacity);
		if (!new_str)
			return ;
		sb->str = new_str;
		sb->capacity = new_capacity;
	}
	sb->str[sb->len++] = c;
	sb->str[sb->len] = '\0';
}

void	sb_append(t_string_builder *sb, const char *s)
{
	size_t	s_len;
	size_t	new_capacity;
	char	*new_str;

	s_len = ft_strlen(s);
	if (!s)
		return ;
	if (sb->len + s_len >= sb->capacity)
	{
		new_capacity = (sb->len + s_len) * 2;
		new_str = cf_realloc(sb->str, new_capacity);
		if (!new_str)
			return ;
		sb->str = new_str;
		sb->capacity = new_capacity;
	}
	ft_strcpy(sb->str + sb->len, s);
	sb->len += s_len;
}

char	*sb_build_and_destroy(t_string_builder *sb)
{
	char	*final_str;

	final_str = sb->str;
	if (!sb)
		return (NULL);
	cf_free_one(sb);
	return (final_str);
}

void	free_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->left)
		free_ast(node->left);
	if (node->right)
		free_ast(node->right);
	if (node->type == NODE_CMD && node->command)
	{
		cf_free_one(node->command);
	}
	cf_free_one(node);
}

void	free_token_list(t_list *tokens)
{
	t_list *current;
	t_token *token;
	while (tokens)
	{
		current = tokens;
		tokens = tokens->next;
		token = (t_token *)current->content;
		if (token && token->value)
			cf_free_one(token->value);
		if (token)
			cf_free_one(token);
		current->next = NULL;
		cf_free_one(current);
	}
}

t_token	*create_token(t_token_type type, const char *value)
{
	t_token	*token;

	token = cf_malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = cf_strdup(value);
	else
		token->value = NULL;
	return (token);
}

static char *extract_subsegment(const char *line, int *i, char delimiter)
{
	int	start;
	start = *i;
	while (line[*i] && line[*i] != delimiter)
		(*i)++;
	char *s = cf_substr(line, start, *i - start);
	return s;
}

char	*extract_non_quoted_word(const char *line, int *i)
{
	int	start;
	start = *i;
	while (line[*i] && !ft_isspace(line[*i]) && !is_operator(line[*i]) && line[*i] != '\'' && line[*i] != '"')
		(*i)++;
	char *s = cf_substr(line, start, *i - start);
	return s;
}

// Helper function to skip whitespace
static void	skip_whitespace(const char *line, int *i, bool *had_space)
{
	*had_space = false;
	while (ft_isspace(line[*i]))
	{
		*had_space = true;
		(*i)++;
	}
}

// Helper function to handle operator tokens
static t_token	*handle_operator_tokens(const char *line, int *i)
{
	if (ft_strncmp(&line[*i], "&&", 2) == 0)
	{
		*i += 2;
		return (create_token(TOKEN_AND, NULL));
	}
	else if (ft_strncmp(&line[*i], "||", 2) == 0)
	{
		*i += 2;
		return (create_token(TOKEN_OR, NULL));
	}
	else if (line[*i] == '|')
	{
		(*i)++;
		return (create_token(TOKEN_PIPE, NULL));
	}
	else if (line[*i] == '(')
	{
		(*i)++;
		return (create_token(TOKEN_LPAREN, NULL));
	}
	else if (line[*i] == ')')
	{
		(*i)++;
		return (create_token(TOKEN_RPAREN, NULL));
	}
	else if (line[*i] == '&')
	{
		(*i)++;
		return (create_token(TOKEN_AMPERSAND, NULL));
	}
	return (NULL);
}

// Helper function to handle redirection tokens
static t_token	*handle_redirection_tokens(const char *line, int *i)
{
	if (ft_strncmp(&line[*i], ">>", 2) == 0)
	{
		*i += 2;
		return (create_token(TOKEN_REDIR_APPEND, NULL));
	}
	else if (ft_strncmp(&line[*i], "<<", 2) == 0)
	{
		*i += 2;
		return (create_token(TOKEN_HEREDOC, NULL));
	}
	else if (line[*i] == '>')
	{
		(*i)++;
		return (create_token(TOKEN_REDIR_OUT, NULL));
	}
	else if (line[*i] == '<')
	{
		(*i)++;
		return (create_token(TOKEN_REDIR_IN, NULL));
	}
	return (NULL);
}

// Helper function to handle quote tokens
static t_token	*handle_quote_tokens(const char *line, int *i, t_list **tokens)
{
	char	*value;

	if (line[*i] == '\'')
	{
		(*i)++;
		value = extract_subsegment(line, i, '\'');
		if (line[*i] == '\'')
			(*i)++;
		else
		{
			free_token_list(*tokens);
			return (NULL);
		}
		return (create_token(TOKEN_SINGLE_QUOTE_WORD, value));
	}
	else if (line[*i] == '"')
	{
		(*i)++;
		value = extract_subsegment(line, i, '"');
		if (line[*i] == '"')
			(*i)++;
		else
		{
			free_token_list(*tokens);
			return (NULL);
		}
		return (create_token(TOKEN_DOUBLE_QUOTE_WORD, value));
	}
	return (NULL);
}

// Helper function to add token to list
static void	add_token_to_list(t_token *new_token, bool had_space, t_list **tokens)
{
	t_list	*node;

	if (new_token)
	{
		new_token->has_space = had_space;
		node = cf_lstnew(new_token);
		ft_lstadd_back(tokens, node);
	}
}

// Helper function to add end token
static void	add_end_token(t_list **tokens)
{
	t_token	*end_token;
	t_list	*end_node;

	end_token = create_token(TOKEN_END, NULL);
	end_token->has_space = false;
	end_node = cf_lstnew(end_token);
	ft_lstadd_back(tokens, end_node);
}

t_list	*tokenize(const char *line)
{
	t_list	*tokens;
	int		i;
	bool	had_space;
	t_token	*new_token;

	tokens = NULL;
	i = 0;
	while (line[i])
	{
		skip_whitespace(line, &i, &had_space);
		if (!line[i])
			break;

		new_token = handle_operator_tokens(line, &i);
		if (!new_token)
			new_token = handle_redirection_tokens(line, &i);
		if (!new_token)
			new_token = handle_quote_tokens(line, &i, &tokens);
		if (!new_token)
		{
			char *word_str = extract_non_quoted_word(line, &i);
			new_token = create_token(TOKEN_WORD, word_str);
		}
		add_token_to_list(new_token, had_space, &tokens);
	}
	add_end_token(&tokens);
	return (tokens);
}

//part of expand_string_variables
static void	expand_status(t_string_builder *sb, t_shell *shell, int *i)
{
	char	*status_str;

	status_str = ft_itoa(shell->last_exit_status);
	sb_append(sb, status_str);
	free(status_str);
	(*i)++;
}

//part of expand_string_variables
static void	expand_digit(t_string_builder *sb, const char *str, int *i)
{
	if (str[*i] == '0')
		sb_append(sb, "SuPuShell");
	(*i)++;
}

//part of expand_string_variables
static void	expand_env_var(t_string_builder *sb, const char *str, t_env *env, int *i)
{
	int		var_start;
	char	*var_name;
	char	*var_value;

	var_start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	var_name = cf_substr(str, var_start, *i - var_start);
	var_value = get_env_value(env, var_name);
	if (var_value)
		sb_append(sb, var_value);
	cf_free_one(var_name);
}

//part of expand_string_variables
static void	expand_other(t_string_builder *sb)
{
	sb_append_char(sb, '$');
}

//part of expand_string_variables
static void	expand_hub(t_expand_ctx *ctx)
{
	if (ctx->str[*ctx->i] == '?')
		expand_status(ctx->sb, ctx->shell, ctx->i);
	else if (ft_isdigit(ctx->str[*ctx->i]))
		expand_digit(ctx->sb, ctx->str, ctx->i);
	else if (ctx->str[*ctx->i] == '_' || ft_isalpha(ctx->str[*ctx->i]))
		expand_env_var(ctx->sb, ctx->str, ctx->env, ctx->i);
	else
		expand_other(ctx->sb);
}

//part of expand_string_variables
static void	expand_loop(const char *str, t_expand_ctx *ctx)
{
	int	*i;

	i = ctx->i;
	while (str[*i])
	{
		if (str[*i] == '$')
		{
			(*i)++;
			expand_hub(ctx);
		}
		else
		{
			sb_append_char(ctx->sb, str[*i]);
			(*i)++;
		}
	}
}

char	*expand_string_variables(const char *str, t_env *env, t_shell *shell)
{
	t_string_builder	*sb;
	int					i;
	t_expand_ctx		ctx;

	sb = sb_create();
	if (!sb)
		return (NULL);
	i = 0;
	ctx.sb = sb;
	ctx.str = str;
	ctx.env = env;
	ctx.shell = shell;
	ctx.i = &i;
	expand_loop(str, &ctx);
	return (sb_build_and_destroy(sb));
}

char	*expand_token_value(t_token *token, t_env *env, t_shell *shell)
{
	if (!token || !token->value)
		return (cf_strdup(""));

	if (token->type == TOKEN_SINGLE_QUOTE_WORD)
	{
		return (cf_strdup(token->value));
	}
	else if (token->type == TOKEN_DOUBLE_QUOTE_WORD)
	{
		return (expand_string_variables(token->value, env, shell));
	}
	else if (token->type == TOKEN_WORD)
	{
		return (expand_string_variables(token->value, env, shell));
	}
	return (cf_strdup(""));
}

bool	is_operator(char c)
{
	return (c == '|' || c == '&' || c == '(' || c == ')' || c == '<' || c == '>');
}

t_ast_node	*create_cmd_node(t_input *cmd)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_CMD;
	node->left = NULL;
	node->right = NULL;
	node->command = cmd;
	return (node);
}

t_ast_node	*create_binary_node(t_node_type type, t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->left = left;
	node->right = right;
	node->command = NULL;
	return (node);
}

t_ast_node	*create_subshell_node(t_ast_node *child)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_SUBSHELL;
	node->left = child;
	node->right = NULL;
	node->command = NULL;
	return (node);
}

static void	init_input_structure(t_input *input, t_env *env, t_shell *shell);
static bool	is_command_terminator(t_token *token);
static bool	validate_redirection_tokens(t_list **current_tokens, t_input *input);
static void	handle_heredoc_redirection(t_input *input, t_token *filename_token);
static char	*expand_filename_token(t_token *filename_token, t_env *env, t_shell *shell);
static void	apply_output_redirection(t_input *input, t_token_type redir_type, char *expanded_value);
static void	apply_input_redirection(t_input *input, char *expanded_value);
static bool	handle_redirection_token(t_input *input, t_list **current_tokens, t_env *env, t_shell *shell);
static bool	is_adjacent_word_token(t_token *token);
static char	*concatenate_adjacent_tokens(t_list **current_tokens, char *expanded_value, t_env *env, t_shell *shell);
static void	handle_wildcard_expansion(t_input *input, t_token *current_tok, char *expanded_value);
static void	handle_regular_word(t_input *input, char *expanded_value);
static void	handle_heredoc_fallback(t_input *input);
static bool	handle_word_token(t_input *input, t_list **current_tokens, t_env *env, t_shell *shell);



// Helper function to initialize input structure
static void	init_input_structure(t_input *input, t_env *env, t_shell *shell)
{
	ft_memset(input, 0, sizeof(t_input));
	input->env = env;
	input->syntax_ok = true;
	input->shell = shell;
}

// Helper function to check if token is a command terminator
static bool	is_command_terminator(t_token *token)
{
	return (token->type == TOKEN_PIPE || token->type == TOKEN_AND ||
			token->type == TOKEN_OR || token->type == TOKEN_RPAREN ||
			token->type == TOKEN_END);
}

// Helper function to handle redirection tokens
// Helper function to validate redirection tokens
static bool	validate_redirection_tokens(t_list **current_tokens, t_input *input)
{
	*current_tokens = (*current_tokens)->next;
	if (!*current_tokens)
		return (input->syntax_ok = false, false);
	return (true);
}

// Helper function to handle heredoc redirection
static void	handle_heredoc_redirection(t_input *input, t_token *filename_token)
{
	if (input->heredoc)
		cf_free_one(input->heredoc);
	input->heredoc = cf_strdup(filename_token->value);
}

// Helper function to expand filename token
static char	*expand_filename_token(t_token *filename_token, t_env *env, t_shell *shell)
{
	if (filename_token->type == TOKEN_SINGLE_QUOTE_WORD)
		return (cf_strdup(filename_token->value));
	else
		return (expand_string_variables(filename_token->value, env, shell));
}

// Helper function to apply output redirection
static void	apply_output_redirection(t_input *input, t_token_type redir_type, char *expanded_value)
{
	if (input->outfile)
		cf_free_one(input->outfile);
	input->outfile = expanded_value;
	if (redir_type == TOKEN_REDIR_OUT)
		input->append = false;
	else if (redir_type == TOKEN_REDIR_APPEND)
		input->append = true;
}

// Helper function to apply input redirection
static void	apply_input_redirection(t_input *input, char *expanded_value)
{
	if (input->infile)
		cf_free_one(input->infile);
	input->infile = expanded_value;
}

static bool	handle_redirection_token(t_input *input, t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_token		*current_tok;
	t_token		*filename_token;
	t_token_type	redir_type;
	char		*expanded_value;

	current_tok = (t_token*)(*current_tokens)->content;
	redir_type = current_tok->type;

	if (!validate_redirection_tokens(current_tokens, input))
		return (false);

	filename_token = (t_token*)(*current_tokens)->content;

	if (redir_type == TOKEN_HEREDOC)
		handle_heredoc_redirection(input, filename_token);
	else
	{
		expanded_value = expand_filename_token(filename_token, env, shell);
		if (redir_type == TOKEN_REDIR_OUT || redir_type == TOKEN_REDIR_APPEND)
			apply_output_redirection(input, redir_type, expanded_value);
		else if (redir_type == TOKEN_REDIR_IN)
			apply_input_redirection(input, expanded_value);
	}
	*current_tokens = (*current_tokens)->next;
	return (true);
}

// Helper function to check if token is an adjacent word token
static bool	is_adjacent_word_token(t_token *token)
{
	return (token->type == TOKEN_SINGLE_QUOTE_WORD ||
			token->type == TOKEN_DOUBLE_QUOTE_WORD ||
			token->type == TOKEN_WORD);
}

// Helper function to concatenate adjacent tokens
static char	*concatenate_adjacent_tokens(t_list **current_tokens, char *expanded_value, t_env *env, t_shell *shell)
{
	t_list	*next_token;
	char	*next_expanded;
	char	*combined;

	next_token = (*current_tokens)->next;
	while (next_token && !((t_token*)next_token->content)->has_space &&
			is_adjacent_word_token((t_token*)next_token->content))
	{
		next_expanded = expand_token_value((t_token*)next_token->content, env, shell);
		if (next_expanded)
		{
			combined = ft_strjoin_free(expanded_value, next_expanded);
			cf_free_one(expanded_value);
			cf_free_one(next_expanded);
			expanded_value = combined;
			*current_tokens = next_token;
			next_token = next_token->next;
		}
		else
			break;
	}
	return (expanded_value);
}

// Helper function to handle wildcard expansion
static void	handle_wildcard_expansion(t_input *input, t_token *current_tok, char *expanded_value)
{
	char	**wildcards;
	int		j;

	if (current_tok->type == TOKEN_WORD &&
		(ft_strchr(expanded_value, '*') || ft_strchr(expanded_value, '?')))
	{
		wildcards = expand_wildcards(expanded_value);
		if (wildcards)
		{
			for (j = 0; wildcards[j]; j++)
				input->args = append_arg(input->args, wildcards[j]);
			free_expanded_wildcards(wildcards);
		}
		else
			input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
	}
	else
		handle_regular_word(input, expanded_value);
}

// Helper function to handle regular word
static void	handle_regular_word(t_input *input, char *expanded_value)
{
	if (ft_strlen(expanded_value) > 0)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
	}
	else
		cf_free_one(expanded_value);
}

static bool	handle_word_token(t_input *input, t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_token	*current_tok;
	char	*expanded_value;

	current_tok = (t_token*)(*current_tokens)->content;
	expanded_value = expand_token_value(current_tok, env, shell);
	if (!expanded_value)
		return (input->syntax_ok = false, false);

	expanded_value = concatenate_adjacent_tokens(current_tokens, expanded_value, env, shell);
	handle_wildcard_expansion(input, current_tok, expanded_value);

	*current_tokens = (*current_tokens)->next;
	return (true);
}

// Helper function to handle heredoc fallback
static void	handle_heredoc_fallback(t_input *input)
{
	if ((!input->args || !input->args[0]) && input->heredoc)
	{
		input->args = cf_malloc(sizeof(char *) * 2);
		if (!input->args)
		{
			input->syntax_ok = false;
			return ;
		}
		input->args[0] = cf_strdup(":");
		input->args[1] = NULL;
	}
}

t_input	parse_command_from_tokens(t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_input	input;
	t_token	*current_tok;

	init_input_structure(&input, env, shell);

	while (*current_tokens && !is_command_terminator((t_token*)(*current_tokens)->content))
	{
		current_tok = (t_token*)(*current_tokens)->content;
		if (current_tok->type >= TOKEN_REDIR_IN && current_tok->type <= TOKEN_HEREDOC)
		{
			if (!handle_redirection_token(&input, current_tokens, env, shell))
				return (input);
		}
		else if (current_tok->type == TOKEN_WORD ||
			current_tok->type == TOKEN_SINGLE_QUOTE_WORD ||
			current_tok->type == TOKEN_DOUBLE_QUOTE_WORD)
		{
			if (!handle_word_token(&input, current_tokens, env, shell))
				return (input);
		}
		else
		{
			input.syntax_ok = false;
			return (input);
		}
	}
	handle_heredoc_fallback(&input);
	return (input);
}

// part of parse_primary
static t_ast_node	*parse_primary_subshell(t_list **tokens, t_shell *shell)
{
	t_ast_node	*sub_expr;
	t_ast_node	*node;

	*tokens = (*tokens)->next;
	sub_expr = parse_expression(tokens, shell);
	if (!sub_expr)
		return (NULL);
	if (!*tokens || ((t_token*)(*tokens)->content)->type != TOKEN_RPAREN)
	{
		shell->last_exit_status = 2;
		free_ast(sub_expr);
		return (NULL);
	}
	*tokens = (*tokens)->next;
	node = create_subshell_node(sub_expr);
	return (node);
}

// part of parse_primary
static t_ast_node	*parse_primary_command(t_list **tokens, t_shell *shell)
{
	t_input		*cmd_data;
	t_ast_node	*node;

	cmd_data = cf_malloc(sizeof(t_input));
	if (!cmd_data)
		return (NULL);
	ft_memset(cmd_data, 0, sizeof(t_input));
	*cmd_data = parse_command_from_tokens(tokens, shell->env, shell);
	if (!cmd_data->syntax_ok)
	{
		cf_free_one(cmd_data);
		shell->last_exit_status = 2;
		return (NULL);
	}
	node = create_cmd_node(cmd_data);
	return (node);
}

t_ast_node	*parse_primary(t_list **tokens, t_shell *shell)
{
	t_token		*current_token;

	if (!*tokens)
		return (NULL);
	current_token = (t_token*)(*tokens)->content;
	if (current_token->type == TOKEN_LPAREN)
		return (parse_primary_subshell(tokens, shell));
	else if (current_token->type == TOKEN_WORD ||
			current_token->type == TOKEN_REDIR_IN ||
			current_token->type == TOKEN_REDIR_OUT ||
			current_token->type == TOKEN_REDIR_APPEND ||
			current_token->type == TOKEN_HEREDOC)
		return (parse_primary_command(tokens, shell));
	else if (current_token->type == TOKEN_AMPERSAND)
	{
		shell->last_exit_status = 2;
		ft_printf("SuPuShell: syntax error near unexpected token '&'\n");
		return (NULL);
	}
	shell->last_exit_status = 0;
	return (NULL);
}

t_ast_node	*parse_pipeline(t_list **tokens, t_shell *shell)
{
	t_ast_node	*left = parse_primary(tokens, shell);
	if (!left)
		return NULL;
	while ((*tokens)->content && ((t_token*)(*tokens)->content)->type == TOKEN_PIPE)
	{
		*tokens = (*tokens)->next;
		t_ast_node *right = parse_primary(tokens, shell);
		if (!right)
			return (NULL);
		left = create_binary_node(NODE_PIPE, left, right);
	}
	return (left);
}

t_ast_node	*parse_and_or(t_list **tokens, t_shell *shell)
{
	t_ast_node	*left = parse_pipeline(tokens, shell);
	if (!left)
		return (NULL);
	while ((*tokens)->content && (((t_token*)(*tokens)->content)->type == TOKEN_AND || ((t_token*)(*tokens)->content)->type == TOKEN_OR))
	{
		t_token_type op_type = ((t_token*)(*tokens)->content)->type;
		*tokens = (*tokens)->next;
		t_ast_node *right = parse_pipeline(tokens, shell);
		if (!right)
			return NULL;
		t_node_type node_type;
		if (op_type == TOKEN_AND)
			node_type = NODE_AND;
		else
			node_type = NODE_OR;
		left = create_binary_node(node_type, left, right);
	}
	return (left);
}

t_ast_node	*parse_expression(t_list **tokens, t_shell *shell)
{
	return (parse_and_or(tokens, shell));
}

t_ast_node	*parse(const char *line, t_shell *shell)
{
	t_list		*tokens;
	t_ast_node	*ast;

	tokens = tokenize(line);
	if (!tokens)
		return (NULL);
	ast = parse_expression(&tokens, shell);

	if (ast && ((t_token*)tokens->content)->type == TOKEN_END)
		;
	else
	{
		free_ast(ast);
		ast = NULL;
	}
	if (tokens)
		free_token_list(tokens);
	tokens = NULL;
	return (ast);
}

// Helpers for execute_node: handle built-in shell commands
static int	execute_builtin_command(t_input *command, t_shell *shell)
{
	int	last_status;

	last_status = 0;
	if (ft_strncmp(command->args[0], "cd", 3) == 0)
		do_cd(command->args, &shell->env);
	else if (ft_strncmp(command->args[0], "export", 7) == 0)
		do_export(command->args, &shell->env);
	else if (ft_strncmp(command->args[0], "unset", 6) == 0)
		do_unset(command->args, &shell->env);
	else if (ft_strncmp(command->args[0], "exit", 5) == 0 ||
			ft_strncmp(command->args[0], "q", 2) == 0)
	{
		shell->should_exit = 1;
		return (0);
	}
	else if (ft_strncmp(command->args[0], "echo", 5) == 0)
		do_echo(command->args);
	else if (ft_strncmp(command->args[0], "env", 4) == 0)
		do_env(shell->env);
	else if (ft_strncmp(command->args[0], "pwd", 4) == 0)
		do_pwd();
	else if (ft_strncmp(command->args[0], ":", 2) == 0)
		last_status = 0;
	return (last_status);
}

// Helpers for execute_node: handle external (non-built-in) commands
static int	execute_external_command(t_input *command, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		last_status;

	last_status = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		run_external_command(command->args, shell->env);
		rl_clear_history();
		cf_free_all();
		exit(127);
	}
	else if (pid < 0)
	{
		perror("fork");
		last_status = 1;
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
	}
	return (last_status);
}

// Helpers for execute_node: handle command AST nodes (built-in or external)
static int	execute_command_node(t_ast_node *node, t_shell *shell)
{
	int		last_status;

	last_status = 0;
	if (!apply_redirections(node->command))
		last_status = 1;
	else if (node->command->args && node->command->args[0])
	{
		if (is_builtin_command(node->command->args[0]))
			last_status = execute_builtin_command(node->command, shell);
		else
			last_status = execute_external_command(node->command, shell);
	}
	return (last_status);
}

// Helpers for execute_node: handle left child process of a pipe
static void	execute_pipe_child_left(t_ast_node *node, t_shell *shell, int *pipefd)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	status = execute_node(node->left, shell);
	rl_clear_history();
	cf_free_all();
	exit(status);
}

// Helpers for execute_node: handle right child process of a pipe
static void	execute_pipe_child_right(t_ast_node *node, t_shell *shell, int *pipefd)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	status = execute_node(node->right, shell);
	rl_clear_history();
	cf_free_all();
	exit(status);
}

// Helpers for execute_node: handle pipe AST nodes
static int	execute_pipe_node(t_ast_node *node, t_shell *shell)
{
	int		pipefd[2];
	pid_t	left_pid;
	pid_t	right_pid;
	int		status;
	int		last_status;

	last_status = 0;
	pipe(pipefd);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	left_pid = fork();
	if (left_pid == 0)
		execute_pipe_child_left(node, shell, pipefd);
	right_pid = fork();
	if (right_pid == 0)
		execute_pipe_child_right(node, shell, pipefd);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(left_pid, &status, 0);
	waitpid(right_pid, &status, 0);
	if (WIFEXITED(status))
		last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		last_status = 128 + WTERMSIG(status);
	return (last_status);
}

// Helpers for execute_node: handle subshell child process
static void	execute_subshell_child(t_ast_node *node, t_shell *shell)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	status = execute_node(node->left, shell);
	rl_clear_history();
	cf_free_all();
	exit(status);
}

// Helpers for execute_node: handle subshell AST nodes
static int	execute_subshell_node(t_ast_node *node, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		last_status;

	last_status = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == 0)
		execute_subshell_child(node, shell);
	else if (pid < 0)
	{
		perror("fork subshell");
		last_status = 1;
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
	}
	return (last_status);
}

// Helpers for execute_node: check if command is a built-in
static bool	is_builtin_command(const char *command)
{
	return (ft_strncmp(command, "cd", 3) == 0
		|| ft_strncmp(command, "export", 7) == 0
		|| ft_strncmp(command, "unset", 6) == 0
		|| ft_strncmp(command, "exit", 5) == 0
		|| ft_strncmp(command, "q", 2) == 0
		|| ft_strncmp(command, "echo", 5) == 0
		|| ft_strncmp(command, "env", 4) == 0
		|| ft_strncmp(command, "pwd", 4) == 0
		|| ft_strncmp(command, ":", 2) == 0);
}

// Helpers for execute_node: handle AND (&&) AST nodes
static int	execute_and_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;

	last_status = execute_node(node->left, shell);
	if (last_status == 0)
		last_status = execute_node(node->right, shell);
	return (last_status);
}

// Helpers for execute_node: handle OR (||) AST nodes
static int	execute_or_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;

	last_status = execute_node(node->left, shell);
	if (last_status != 0)
		last_status = execute_node(node->right, shell);
	return (last_status);
}

// Main dispatcher for AST execution
int	execute_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;
	int	original_stdin;
	int	original_stdout;

	last_status = 0;
	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	if (!node)
		return (1);
	if (node->type == NODE_CMD)
		last_status = execute_command_node(node, shell);
	else if (node->type == NODE_PIPE)
		last_status = execute_pipe_node(node, shell);
	else if (node->type == NODE_AND)
		last_status = execute_and_node(node, shell);
	else if (node->type == NODE_OR)
		last_status = execute_or_node(node, shell);
	else if (node->type == NODE_SUBSHELL)
		last_status = execute_subshell_node(node, shell);
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
	shell->last_exit_status = last_status;
	return (last_status);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_shell	shell;

	(void)argc;
	(void)argv;
	billy_print();
	shell.env = create_env(envp);
	shell.last_exit_status = 0;
	shell.should_exit = 0;
	disable_echoctl();
	int interactive = 0;
	t_ast_node	*ast = NULL;
	while (1)
	{
		setup_signal();
		if (isatty(STDIN_FILENO))
		{
			line = readline(SHELLNAME);
			interactive = 1;
		}
		else
			line = get_next_line(STDIN_FILENO);
		if (g_sigint_exit_status == 1)
		{
			shell.last_exit_status = 1;
			g_sigint_exit_status = 0;
		}
		if (line)
			ast = parse(line, &shell);
		if (!line)
		{
			rl_clear_history();
			cf_free_all();
			break ;
		}
		if (*line && interactive)
			add_history(line);
		if (!check_for_input(line))
		{
			free(line);
			continue ;
		}
		if (ast)
		{
			execute_node(ast, &shell);
			free_ast(ast);
			ast = NULL;
		}
		free(line);
		if (shell.should_exit)
		{
			rl_clear_history();
			cf_free_all();
			break ;
		}
	}
	shell.env = NULL;
	if (ast)
		free_ast(ast);
	ast = NULL;
	cf_free_all();
	return (0);
}

//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
// ➜  SuPuShell git:(master) ✗ export FILE=test.txt
// ➜  SuPuShell git:(master) ✗ echo hi > $FILE
// ➜  SuPuShell git:(master) ✗ echo hi $FILE > $FILE
// ➜  SuPuShell git:(master) ✗ echo hi '$FILE' >> $FILE
// ➜  SuPuShell git:(master) ✗ echo hi '$FILE' >> '$FILE'
// ➜  SuPuShell git:(master) ✗ cat << $FILE
