/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:33:08 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:18:46 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	handle_heredoc_child_part(t_input *input, char *filename, int i)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	heredoc_child_process(input, filename, i);
	rl_clear_history();
	cf_free_all();
	exit(0);
}

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
