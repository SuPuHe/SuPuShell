/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:48:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:23:59 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static void	handle_file_redirection_case(t_input *input,
				t_list **current_tokens, t_env *env,
				t_token_type redir_type)
{
	t_token	*filename_token;
	char	*expanded_value;

	filename_token = (t_token *)(*current_tokens)->content;
	if (!filename_token || !filename_token->value)
	{
		input->syntax_ok = false;
		return ;
	}
	expanded_value = expand_filename_token(filename_token, env, input->shell);
	if (redir_type == TOKEN_REDIR_OUT || redir_type == TOKEN_REDIR_APPEND)
		apply_output_redirection(input, redir_type, expanded_value);
	else if (redir_type == TOKEN_REDIR_IN)
		apply_input_redirection(input, expanded_value);
	*current_tokens = (*current_tokens)->next;
}

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

