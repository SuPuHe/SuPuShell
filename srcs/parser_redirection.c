/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:10:29 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:44:46 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to expand filename token
char	*expand_filename_token(t_token *filename_token,
	t_env *env, t_shell *shell)
{
	if (filename_token->type == TOKEN_SINGLE_QUOTE_WORD)
		return (cf_strdup(filename_token->value));
	else
		return (expand_string_variables(filename_token->value, env, shell));
}

// Helper function to handle heredoc redirection
void	handle_heredoc_redirection(t_input *input, t_token *filename_token)
{
	if (input->heredoc)
		cf_free_one(input->heredoc);
	input->heredoc = cf_strdup(filename_token->value);
}

// Helper function to handle redirection tokens
// Helper function to validate redirection tokens
bool	validate_redirection_tokens(t_list **current_tokens, t_input *input)
{
	*current_tokens = (*current_tokens)->next;
	if (!*current_tokens)
		return (input->syntax_ok = false, false);
	return (true);
}

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
