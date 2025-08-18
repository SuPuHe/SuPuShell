/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_handlers_1.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 13:07:31 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 12:29:57 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// parser_redirection_handlers_1.c

void	add_outfile(t_input *input, char *filename, bool append)
{
	char	**new_arr;
	bool	*new_flags;
	int		i;

	new_arr = cf_malloc(sizeof(char *) * (input->outfiles_count + 1));
	if (!new_arr)
		return;
	new_flags = cf_malloc(sizeof(bool) * (input->outfiles_count + 1));
	if (!new_flags)
	{
		cf_free_one(new_arr);
		return;
	}
	i = 0;
	while (i < input->outfiles_count)
	{
		new_arr[i] = input->all_outfiles[i];
		new_flags[i] = input->all_outfiles_append_flags[i];
		i++;
	}
	if (input->all_outfiles)
		cf_free_one(input->all_outfiles);
	if (input->all_outfiles_append_flags)
		cf_free_one(input->all_outfiles_append_flags);
	new_arr[input->outfiles_count] = filename;
	new_flags[input->outfiles_count] = append;
	input->all_outfiles = new_arr;
	input->all_outfiles_append_flags = new_flags;
	input->outfiles_count++;
}

/**
 * @brief Applies output redirection to the input structure.
 *
 * Sets the output file and append mode based on the redirection type.
 * Creates the file immediately to match bash behavior.
 *
 * @param input Pointer to input structure.
 * @param redir_type Type of output redirection.
 * @param expanded_value Expanded output file name.
 */
// In parser_redirection_handlers_1.c
// parser_redirection_handlers_1.c

// parser_redirection_handlers_1.c

void	apply_output_redirection(t_input *input,
	t_token_type redir_type, char *expanded_value)
{
	bool	append;

	append = (redir_type == TOKEN_REDIR_APPEND);

	// Free the previous outfile before adding a new one
	if (input->outfile)
		cf_free_one(input->outfile);

	// Add the filename and also make a copy for `input->outfile`
	add_outfile(input, cf_strdup(expanded_value), append);
	input->outfile = expanded_value;
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

	if (input->heredoc_count > 0)
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
