/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:10:29 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:10:26 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		return (expand_string_variables(filename_token->value, env, shell));
}

/**
 * @brief Handles heredoc redirection by storing the delimiter.
 *
 * Sets the heredoc field in the input structure to the delimiter value.
 *
 * @param input Pointer to the input structure.
 * @param filename_token Pointer to the heredoc delimiter token.
 */
void	handle_heredoc_redirection(t_input *input, t_token *filename_token)
{
	if (input->heredoc)
		cf_free_one(input->heredoc);
	input->heredoc = cf_strdup(filename_token->value);
}

/**
 * @brief Validates redirection tokens and advances the token list.
 *
 * Moves to the next token and checks for syntax errors.
 *
 * @param current_tokens Pointer to the token list pointer.
 * @param input Pointer to the input structure.
 * @return true if valid, false otherwise.
 */
bool	validate_redirection_tokens(t_list **current_tokens, t_input *input)
{
	*current_tokens = (*current_tokens)->next;
	if (!*current_tokens)
		return (input->syntax_ok = false, false);
	return (true);
}

/**
 * @brief Handles a redirection token, updating the input structure.
 *
 * Processes heredoc, input, and output redirections, expanding filenames
 * and updating the input structure accordingly.
 *
 * @param input Pointer to the input structure.
 * @param current_tokens Pointer to the token list pointer.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return true if successful, false otherwise.
 */
bool	handle_redirection_token(t_input *input,
	t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_token			*current_tok;
	t_token			*filename_token;
	t_token_type	redir_type;
	char			*expanded_value;

	current_tok = (t_token *)(*current_tokens)->content;
	redir_type = current_tok->type;
	if (!validate_redirection_tokens(current_tokens, input))
		return (false);
	filename_token = (t_token *)(*current_tokens)->content;
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
	char	*line;
	int		fd;
	char	*filename;

	filename = "/tmp/.minishell_heredoc";
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (perror("heredoc file"), false);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, input->heredoc) == 0)
			break ;
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	input->infile = cf_strdup(filename);
	return (true);
}
