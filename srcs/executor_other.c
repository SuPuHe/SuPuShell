/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_other.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:30:54 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:05:30 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes an AND (&&) AST node.
 *
 * Executes the left node; if it succeeds, executes the right node.
 * Returns the exit status of the last executed node.
 *
 * @param node AST node representing the AND operation.
 * @param shell Pointer to the shell structure.
 * @return Exit status of the last executed node.
 */
int	execute_and_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;

	last_status = execute_node(node->left, shell);
	if (last_status == 0)
		last_status = execute_node(node->right, shell);
	return (last_status);
}

/**
 * @brief Executes an OR (||) AST node.
 *
 * Executes the left node; if it fails, executes the right node.
 * Returns the exit status of the last executed node.
 *
 * @param node AST node representing the OR operation.
 * @param shell Pointer to the shell structure.
 * @return Exit status of the last executed node.
 */
int	execute_or_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;

	last_status = execute_node(node->left, shell);
	if (last_status != 0)
		last_status = execute_node(node->right, shell);
	return (last_status);
}

/**
 * @brief Executes a subshell AST node in a child process.
 *
 * Runs the left child node in a new process, handling signals and
 * cleaning up before exit.
 *
 * @param node AST node representing the subshell.
 * @param shell Pointer to the shell structure.
 */
void	execute_subshell_child(t_ast_node *node, t_shell *shell)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	status = execute_node(node->left, shell);
	rl_clear_history();
	cf_free_all();
	exit(status);
}

/**
 * @brief Executes a subshell AST node by forking a new process.
 *
 * Forks a child process to run the subshell, waits for completion,
 * and returns the exit status.
 *
 * @param node AST node representing the subshell.
 * @param shell Pointer to the shell structure.
 * @return Exit status of the subshell.
 */
int	execute_subshell_node(t_ast_node *node, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		last_status;

	last_status = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == 0)
		execute_subshell_child(node, shell);
	else if (pid < 0)
	{
		perror("fork subshell");
		last_status = 1;
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
	}
	return (last_status);
}
