/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:48:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 13:39:28 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles a heredoc redirection token.
 *
 * Collects the heredoc delimiter and determines if it is quoted.
 * If no valid delimiter is found, marks syntax as invalid.
 * Otherwise, sets up heredoc handling with the collected delimiter.
 *
 * @param input Input structure holding parsing state.
 * @param current_tokens Pointer to the current token list iterator.
 * @return true if heredoc was successfully handled, false on syntax error.
 */
static bool	handle_heredoc_token_case(t_input *input,
	t_list **current_tokens)
{
	char	*delimiter;
	bool	quoted;

	delimiter = collect_heredoc_delimiter(current_tokens, &quoted);
	if (!delimiter)
	{
		input->syntax_ok = false;
		return (false);
	}
	handle_heredoc_redirection_with_delimiter(input, delimiter, quoted);
	return (true);
}

/**
 * @brief Handles input/output file redirection tokens.
 *
 * Expands the filename token, updates infile or outfiles depending
 * on the redirection type, and advances the token iterator.
 *
 * @param input Input structure holding parsing state.
 * @param current_tokens Pointer to the current token list iterator.
 * @param env Environment variable list for expansion.
 * @param redir_type Type of redirection token (>, >>, <).
 */
static void	handle_file_redirection_case(t_input *input,
			t_list **current_tokens, t_env *env,
			t_token_type redir_type)
{
	char	*expanded_value;

	expanded_value = get_expanded_filename(input, current_tokens, env);
	if (!expanded_value)
		return ;
	if (redir_type == TOKEN_REDIR_OUT || redir_type == TOKEN_REDIR_APPEND)
	{
		add_outfile(input, expanded_value, redir_type == TOKEN_REDIR_APPEND);
		input->outfile = expanded_value;
	}
	else if (redir_type == TOKEN_REDIR_IN)
	{
		if (input->infile)
			cf_free_one(input->infile);
		input->infile = expanded_value;
	}
	*current_tokens = (*current_tokens)->next;
}

/**
 * @brief Dispatches handling of a redirection token.
 *
 * Validates the redirection token sequence, determines the type,
 * and calls the appropriate handler (heredoc or file redirection).
 *
 * @param input Input structure holding parsing state.
 * @param current_tokens Pointer to the current token list iterator.
 * @param env Environment variable list used for expansions.
 * @return true if redirection was valid and processed, false on error.
 */
bool	handle_redirection_token(t_input *input,
		t_list **current_tokens, t_env *env)
{
	t_token			*current_tok;
	t_token_type	redir_type;

	if (!validate_redirection_tokens(current_tokens, input))
		return (false);
	current_tok = (t_token *)(*current_tokens)->content;
	redir_type = current_tok->type;
	*current_tokens = (*current_tokens)->next;
	if (redir_type == TOKEN_HEREDOC)
		return (handle_heredoc_token_case(input, current_tokens));
	handle_file_redirection_case(input, current_tokens, env, redir_type);
	return (input->syntax_ok);
}

/**
 * @brief Processes a single line inside the heredoc child process.
 *
 * Expands variables if the heredoc delimiter is unquoted.
 * Writes the processed line (expanded or raw) to the temporary heredoc file.
 *
 * @param input Input structure holding heredoc state.
 * @param line Line of input from the user.
 * @param fd File descriptor of the heredoc temporary file.
 * @param index Index of the current heredoc in progress.
 */
static void	heredoc_child_process_line(t_input *input, char *line,
				int fd, int index)
{
	char	*expanded;

	if (!input->heredoc_is_quoted[index])
	{
		expanded = expand_string_variables(line, input->env,
				input->shell, false);
		write(fd, expanded, ft_strlen(expanded));
		write(fd, "\n", 1);
		cf_free_one(expanded);
	}
	else
	{
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
	}
}

/**
 * @brief Handles heredoc input in a child process.
 *
 * Opens a temporary file for the heredoc, reads lines from stdin until
 * the delimiter is encountered, processes each line (with or without
 * variable expansion), and writes to the file.
 *
 * On file open failure, exits immediately with error.
 *
 * @param input Input structure holding heredoc state and delimiters.
 * @param filename Temporary file to write heredoc content.
 * @param index Index of the current heredoc being processed.
 */
void	heredoc_child_process(t_input *input, char *filename, int index)
{
	char	*line;
	int		fd;

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
		heredoc_child_process_line(input, line, fd, index);
		free(line);
	}
	close(fd);
}
