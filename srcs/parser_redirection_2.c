/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:33:08 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 18:04:06 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles the child process for a single heredoc.
 *
 * Sets default signal handling for SIGINT and SIGQUIT, runs the
 * heredoc input routine, clears the readline history, frees all
 * tracked memory, and exits the child process.
 *
 * @param input Pointer to the t_input structure of the current command.
 * @param filename Temporary file to store heredoc input.
 * @param i Index of the current heredoc in the command's heredoc array.
 */
static bool	handle_heredoc_child_part(t_input *input, char *filename, int i)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	heredoc_child_process(input, filename, i);
	rl_clear_history();
	cf_free_all();
	exit(0);
}

/**
 * @brief Handles the parent-side logic after a heredoc child finishes.
 *
 * Checks the child's exit status. If the child was terminated by
 * SIGINT, it removes the temporary heredoc file and sets the global
 * signal exit status. Otherwise, sets the input file to the heredoc
 * temporary file.
 *
 * @param input Pointer to the t_input structure of the current command.
 * @param filename Temporary file created for the heredoc.
 * @param status Exit status of the child process handling the heredoc.
 * @return true if heredoc was handled successfully, false if it was
 *			interrupted by SIGINT.
 */
static bool	handle_heredoc_parent_part(t_input *input,
		char *filename, int status)
{
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
	return (true);
}

/**
 * @brief Processes all heredocs for a command in a loop.
 *
 * Iterates through all heredoc delimiters in the input structure,
 * creates a temporary filename for each, forks a child process to
 * handle heredoc input, and waits for the child to finish. Parent
 * handles the result and continues to the next heredoc.
 *
 * @param input Pointer to the t_input structure containing heredoc
 *				delimiters and related metadata.
 * @return true if all heredocs were processed successfully, false if
 *				any heredoc failed.
 */
static bool	handle_heredoc_loop_part(t_input *input)
{
	int		i;
	char	*num_str;
	char	*filename;
	pid_t	pid;
	int		status;

	i = 0;
	while (i < input->heredoc_count)
	{
		num_str = cf_itoa(i);
		filename = cf_strjoin("/tmp/.minishell_heredoc_", num_str);
		cf_free_one(num_str);
		pid = fork();
		if (pid == 0)
			handle_heredoc_child_part(input, filename, i);
		else
		{
			waitpid(pid, &status, 0);
			if (!handle_heredoc_parent_part(input, filename, status))
				return (false);
		}
		i++;
	}
	return (true);
}

/**
 * @brief Processes all heredocs for the given input structure.
 *
 * Checks if the heredocs were already processed, and if not, runs
 * the loop that handles each heredoc. Marks heredocs as processed
 * on success.
 *
 * @param input Pointer to the t_input structure containing heredoc
 *				information.
 * @return true if all heredocs were processed successfully or already
 *				processed, false if an error occurred during processing.
 */
bool	handle_heredoc(t_input *input)
{
	if (input->heredoc_processed)
		return (true);
	if (!handle_heredoc_loop_part(input))
		return (false);
	input->heredoc_processed = true;
	return (true);
}

/**
 * @brief Frees all allocated heredoc delimiters and related arrays.
 *
 * Iterates through the heredoc_delimiters array, frees each string,
 * then frees the arrays themselves. Resets pointers and heredoc count
 * in the input structure to indicate no heredocs are stored.
 *
 * @param input Pointer to the t_input structure containing heredoc
 *				delimiters and metadata.
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
