/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:29:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:20:20 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helpers for execute_node: handle right child process of a pipe
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

// Helpers for execute_node: handle left child process of a pipe
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

// Helpers for execute_node: handle pipe AST nodes
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
