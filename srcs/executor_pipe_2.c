/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:33:10 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 16:45:47 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Add a child process PID to the pipeline list.
 *
 * This function stores the given PID in the list of process IDs
 * managed by the pipeline structure and increments the counter.
 *
 * @param pl Pointer to the PID list structure.
 * @param pid Process ID to add.
 */
void	pidlist_add(t_pid_list *pl, pid_t pid)
{
	pl->p[pl->n++] = pid;
}

/**
 * @brief Wait for all processes in a pipeline and get last exit status.
 *
 * This function waits for all child processes stored in the pipeline list.
 * It returns the exit status of the last process in the pipeline.
 *
 * @param pl Pointer to the pipeline process list (t_pid_list).
 * @return int Exit status of the last process in the pipeline.
 */
int	wait_pipeline(t_pid_list *pl)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 1;
	while (i < pl->n)
	{
		if (waitpid(pl->p[i], &status, 0) > 0
			&& pl->p[i] == pl->last)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	return (last_status);
}

/**
 * @brief Recursively processes all heredocs in a pipeline.
 *
 * Traverses the AST pipeline nodes and executes any heredocs found in
 * command nodes. Returns false if a heredoc processing fails.
 *
 * @param node Pointer to the AST node (command or pipe).
 * @return true if all heredocs were processed successfully, false otherwise.
 */
bool	handle_heredocs_in_pipeline(t_ast_node *node)
{
	if (!node)
		return (true);
	if (node->type == NODE_CMD && node->command->heredoc_count > 0)
	{
		if (!handle_heredoc(node->command))
			return (false);
	}
	else if (node->type == NODE_PIPE)
	{
		if (!handle_heredocs_in_pipeline(node->left))
			return (false);
		if (!handle_heredocs_in_pipeline(node->right))
			return (false);
	}
	return (true);
}
