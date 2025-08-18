/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_handlers_1.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 13:07:31 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 13:36:25 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands the arrays for storing outfiles and append flags.
 *
 * Allocates new arrays with size `outfiles_count + 1`, copies old content,
 * and frees the previous arrays.
 *
 * @param input Input structure containing outfile data.
 * @param new_arr Pointer to the new array of filenames.
 * @param new_flags Pointer to the new array of append flags.
 * @return true if allocation succeeded, false otherwise.
 */
static bool	expand_outfiles_arrays(t_input *input,
			char ***new_arr, bool **new_flags)
{
	int	i;

	*new_arr = cf_malloc(sizeof(char *) * (input->outfiles_count + 1));
	if (!*new_arr)
		return (false);
	*new_flags = cf_malloc(sizeof(bool) * (input->outfiles_count + 1));
	if (!*new_flags)
	{
		cf_free_one(*new_arr);
		return (false);
	}
	i = 0;
	while (i < input->outfiles_count)
	{
		(*new_arr)[i] = input->all_outfiles[i];
		(*new_flags)[i] = input->all_outfiles_append_flags[i];
		i++;
	}
	if (input->all_outfiles)
		cf_free_one(input->all_outfiles);
	if (input->all_outfiles_append_flags)
		cf_free_one(input->all_outfiles_append_flags);
	return (true);
}

/**
 * @brief Adds a new outfile to the input structure.
 *
 * Expands arrays of outfiles and append flags, copies existing ones,
 * and inserts the new filename with its append mode.
 *
 * @param input Input structure containing outfile data.
 * @param filename Output filename to add.
 * @param append Whether the file should be opened in append mode.
 */
void	add_outfile(t_input *input, char *filename, bool append)
{
	char	**new_arr;
	bool	*new_flags;

	if (!expand_outfiles_arrays(input, &new_arr, &new_flags))
		return ;
	new_arr[input->outfiles_count] = filename;
	new_flags[input->outfiles_count] = append;
	input->all_outfiles = new_arr;
	input->all_outfiles_append_flags = new_flags;
	input->outfiles_count++;
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
