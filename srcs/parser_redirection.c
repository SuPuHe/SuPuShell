/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:10:29 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/13 13:17:41 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_list	*collect_d_tokens_and_sb(t_list *start,
	t_string_builder *sb, bool *quoted)
{
	t_list	*cur;
	t_token	*tok;

	cur = start;
	*quoted = false;
	while (cur)
	{
		tok = (t_token *)cur->content;
		if (!tok)
			break ;
		if (tok->type != TOKEN_WORD
			&& tok->type != TOKEN_SINGLE_QUOTE_WORD
			&& tok->type != TOKEN_DOUBLE_QUOTE_WORD)
			break ;
		if (tok->has_space && cur != start)
			break ;
		if (tok->type == TOKEN_SINGLE_QUOTE_WORD
			|| tok->type == TOKEN_DOUBLE_QUOTE_WORD)
			*quoted = true;
		sb_append(sb, tok->value, false);
		cur = cur->next;
	}
	return (cur);
}

char	*collect_heredoc_delimiter(t_list **current_tokens, bool *quoted)
{
	t_list				*start;
	t_string_builder	*sb;

	if (!current_tokens || !*current_tokens || !(*current_tokens)->content)
		return (NULL);
	start = *current_tokens;
	sb = sb_create();
	if (!sb)
		return (NULL);
	*current_tokens = collect_d_tokens_and_sb(start, sb, quoted);
	return (sb_build_and_destroy(sb));
}

/**
 * @brief Expands a filename token, handling quotes and variables.
 *
 * Returns the expanded filename string for redirection, handling
 * single quotes and variable expansion.
 *
 * @param filename_token Pointer to the filename token.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Expanded filename string (must be freed).
 */
char	*expand_filename_token(t_token *filename_token,
	t_env *env, t_shell *shell)
{
	if (filename_token->type == TOKEN_SINGLE_QUOTE_WORD)
		return (cf_strdup(filename_token->value));
	else
		return (expand_string_variables(filename_token->value,
				env, shell, false));
}

static void	resize_heredoc_arrays(t_input *input)
{
	char	**new_delims;
	bool	*new_quoted;
	int		i;

	new_delims = cf_malloc(sizeof(char *) * (input->heredoc_count + 2));
	new_quoted = cf_malloc(sizeof(bool) * (input->heredoc_count + 2));
	i = 0;
	while (i < input->heredoc_count)
	{
		new_delims[i] = input->heredoc_delimiters[i];
		new_quoted[i] = input->heredoc_is_quoted[i];
		i++;
	}
	cf_free_one(input->heredoc_delimiters);
	cf_free_one(input->heredoc_is_quoted);
	input->heredoc_delimiters = new_delims;
	input->heredoc_is_quoted = new_quoted;
}

/**
 * @brief Handles heredoc redirection by storing the delimiter.
 *
 * Sets the heredoc field in the input structure to the delimiter value.
 *
 * @param input Pointer to the input structure.
 * @param filename_token Pointer to the heredoc delimiter token.
 */
void	handle_heredoc_redirection_with_delimiter(
	t_input *input, char *delimiter, bool quoted)
{
	if (!input->heredoc_delimiters)
	{
		input->heredoc_delimiters = cf_malloc(sizeof(char *) * 2);
		input->heredoc_is_quoted = cf_malloc(sizeof(bool) * 2);
		input->heredoc_count = 0;
	}
	else
	{
		resize_heredoc_arrays(input);
	}
	input->heredoc_delimiters[input->heredoc_count] = delimiter;
	input->heredoc_is_quoted[input->heredoc_count] = quoted;
	input->heredoc_count++;
	input->heredoc_delimiters[input->heredoc_count] = NULL;
}

/**
 * @brief Validates the presence of a token after a redirection operator.
 *
 * Advances the token list and checks if the next token exists and is valid.
 * If the next token is missing,
 * sets the syntax flag to false and returns false.
 *
 * @param current_tokens Pointer to the token list pointer.
 * @param input Pointer to the input structure to update the syntax flag.
 * @return true if the next token is valid, false otherwise.
 */
bool	validate_redirection_tokens(t_list **current_tokens, t_input *input)
{
	t_token	*tok;

	if (!*current_tokens || !(*current_tokens)->next)
		return (input->syntax_ok = false, false);
	tok = (t_token *)(*current_tokens)->next->content;
	if (tok->type == TOKEN_REDIR_OUT || tok->type == TOKEN_REDIR_IN
		|| tok->type == TOKEN_REDIR_APPEND || tok->type == TOKEN_HEREDOC
		|| !tok->value)
		return (input->syntax_ok = false, false);
	return (true);
}

/**
 * @brief Processes a redirection token and updates the input structure.
 *
 * This function handles redirection tokens such as input redirection (`<`),
 * output redirection (`>`), append redirection (`>>`), and heredoc (`<<`).
 * It validates the token sequence, expands filenames, and updates the
 * input structure accordingly.
 *
 * If the redirection token is invalid or missing a filename, an error
 * message is displayed, and the syntax flag is set to false.
 *
 * @param input Pointer to the input structure to update.
 * @param current_tokens Pointer to the current position in the token list.
 * @param env Pointer to the environment structure for variable expansion.
 * @param shell Pointer to the shell structure for additional context.
 * @return true if the redirection was successfully processed,
 * false otherwise.
 */
bool	handle_redirection_token(t_input *input,
	t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_token			*current_tok;
	t_token_type	redir_type;
	char			*delimiter;
	bool			quoted;
	t_token			*filename_token;
	char			*expanded_value;

	if (!validate_redirection_tokens(current_tokens, input))
		return (false);
	current_tok = (t_token *)(*current_tokens)->content;
	redir_type = current_tok->type;
	*current_tokens = (*current_tokens)->next;

	if (redir_type == TOKEN_HEREDOC)
	{
		delimiter = collect_heredoc_delimiter(current_tokens, &quoted);
		if (!delimiter)
			return (input->syntax_ok = false, false);
		handle_heredoc_redirection_with_delimiter(input, delimiter, quoted);
		return (true);
	}
	filename_token = (t_token *)(*current_tokens)->content;
	if (!filename_token || !filename_token->value)
		return (input->syntax_ok = false, false);
	expanded_value = expand_filename_token(filename_token, env, shell);
	if (redir_type == TOKEN_REDIR_OUT || redir_type == TOKEN_REDIR_APPEND)
		apply_output_redirection(input, redir_type, expanded_value);
	else if (redir_type == TOKEN_REDIR_IN)
		apply_input_redirection(input, expanded_value);
	*current_tokens = (*current_tokens)->next;
	return (true);
}

void	heredoc_child_process(t_input *input, char *filename, int index)
{
	char	*line;
	int		fd;
	char	*expanded;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("heredoc file");
		rl_clear_history();
		cf_free_all();
		exit(1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, input->heredoc_delimiters[index]) == 0)
		{
			free(line);
			break ;
		}
		if (!input->heredoc_is_quoted[index])
		{
			expanded = expand_string_variables(line, input->env, input->shell, false);
			write(fd, expanded, ft_strlen(expanded));
			write(fd, "\n", 1);
			cf_free_one(expanded);
		}
		else
		{
			write(fd, line, ft_strlen(line));
			write(fd, "\n", 1);
		}
		free(line);
	}
	close(fd);
}

/**
 * @brief Handles heredoc input, writing lines to a temporary file.
 *
 * Reads lines from the user until the heredoc delimiter is found,
 * writes them to a temp file, and sets the input file for the command.
 *
 * @param input Pointer to the input structure.
 * @return true on success, false on error.
 */
bool	handle_heredoc(t_input *input)
{
	int		i;
	char	*num_str;
	char	*filename;
	pid_t	pid;
	int		status;

	if (input->heredoc_processed)
		return (true);
	i = 0;
	while (i < input->heredoc_count)
	{
		num_str = cf_itoa(i);
		filename = cf_strjoin("/tmp/.minishell_heredoc_", num_str);
		cf_free_one(num_str);
		pid = fork();
		if (pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			heredoc_child_process(input, filename, i);
			rl_clear_history();
			cf_free_all();
			exit(0);
		}
		else
		{
			waitpid(pid, &status, 0);
			if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
			{
				unlink(filename);
				cf_free_one(filename);
				g_sigint_exit_status = 130;
				return (false);
			}
			if (input->infile)
				cf_free_one(input->infile);
			input->infile = filename;
		}
		i++;
	}
	input->heredoc_processed = true;
	return (true);
}

/**
 * @brief Frees heredoc delimiters array.
 *
 * Frees all delimiter strings and the array itself.
 *
 * @param input Pointer to the input structure.
 */
void	free_heredoc_delimiters(t_input *input)
{
	int	i;

	if (input->heredoc_delimiters)
	{
		i = 0;
		while (i < input->heredoc_count)
		{
			cf_free_one(input->heredoc_delimiters[i]);
			i++;
		}
		cf_free_one(input->heredoc_delimiters);
		input->heredoc_delimiters = NULL;
	}
	if (input->heredoc_is_quoted)
	{
		cf_free_one(input->heredoc_is_quoted);
		input->heredoc_is_quoted = NULL;
	}
	input->heredoc_count = 0;
}
