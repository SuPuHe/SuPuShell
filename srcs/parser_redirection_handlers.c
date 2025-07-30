/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_handlers.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 13:07:31 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:31:13 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Applies output redirection to the input structure.
 *
 * Sets the output file and append mode based on the redirection type.
 *
 * @param input Pointer to input structure.
 * @param redir_type Type of output redirection.
 * @param expanded_value Expanded output file name.
 */
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

/**
 * @brief Applies input redirection to the input structure.
 *
 * Sets the input file for the command.
 *
 * @param input Pointer to input structure.
 * @param expanded_value Expanded input file name.
 */
void	apply_input_redirection(t_input *input, char *expanded_value)
{
	if (input->infile)
		cf_free_one(input->infile);
	input->infile = expanded_value;
}

/**
 * @brief Applies all input and output redirections for a command.
 *
 * Calls input and output redirection handlers and returns true if
 * successful.
 *
 * @param input Pointer to input structure.
 * @return true if all redirections applied, false otherwise.
 */
bool	apply_redirections(t_input *input)
{
	if (!apply_input_redirections(input))
		return (false);
	if (!apply_output_redirections(input))
		return (false);
	return (true);
}

/**
 * @brief Applies all input redirections, including heredoc.
 *
 * Handles heredoc and opens input file for reading, duplicating
 * file descriptor to stdin.
 *
 * @param input Pointer to input structure.
 * @return true if successful, false otherwise.
 */
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

/**
 * @brief Applies all output redirections for a command.
 *
 * Opens output file for writing, sets append or truncate mode,
 * and duplicates file descriptor to stdout.
 *
 * @param input Pointer to input structure.
 * @return true if successful, false otherwise.
 */
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
