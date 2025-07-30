/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:29:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:05:30 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes the right child of a pipe in a child process.
 *
 * Sets up input redirection from the pipe, executes the right AST node,
 * and exits with the returned status.
 *
 * @param node AST node for the right side of the pipe.
 * @param shell Pointer to the shell structure.
 * @param pipefd Pipe file descriptors array.
 */
void	execute_pipe_child_right(t_ast_node *node, t_shell *shell, int *pipefd)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	status = execute_node(node->right, shell);
	rl_clear_history();
	cf_free_all();
	exit(status);
}

/**
 * @brief Executes the left child of a pipe in a child process.
 *
 * Sets up output redirection to the pipe, executes the left AST node,
 * and exits with the returned status.
 *
 * @param node AST node for the left side of the pipe.
 * @param shell Pointer to the shell structure.
 * @param pipefd Pipe file descriptors array.
 */
void	execute_pipe_child_left(t_ast_node *node, t_shell *shell, int *pipefd)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	status = execute_node(node->left, shell);
	rl_clear_history();
	cf_free_all();
	exit(status);
}

/**
 * @brief Executes a pipe AST node, connecting left and right children.
 *
 * Creates a pipe, forks child processes for left and right nodes,
 * and waits for both to finish. Returns the exit status.
 *
 * @param node AST node representing the pipe.
 * @param shell Pointer to the shell structure.
 * @return Exit status of the rightmost command in the pipe.
 */
int	execute_pipe_node(t_ast_node *node, t_shell *shell)
{
	int		pipefd[2];
	pid_t	left_pid;
	pid_t	right_pid;
	int		status;
	int		last_status;

	last_status = 0;
	pipe(pipefd);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	left_pid = fork();
	if (left_pid == 0)
		execute_pipe_child_left(node, shell, pipefd);
	right_pid = fork();
	if (right_pid == 0)
		execute_pipe_child_right(node, shell, pipefd);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(left_pid, &status, 0);
	waitpid(right_pid, &status, 0);
	if (WIFEXITED(status))
		last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		last_status = 128 + WTERMSIG(status);
	return (last_status);
}
