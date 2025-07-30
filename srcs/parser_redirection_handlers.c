/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_handlers.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 13:07:31 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:45:08 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to apply output redirection
void	apply_output_redirection(t_input *input,
	t_token_type redir_type, char *expanded_value)
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
void	apply_input_redirection(t_input *input, char *expanded_value)
{
	if (input->infile)
		cf_free_one(input->infile);
	input->infile = expanded_value;
}

bool	apply_redirections(t_input *input)
{
	if (!apply_input_redirections(input))
		return (false);
	if (!apply_output_redirections(input))
		return (false);
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
