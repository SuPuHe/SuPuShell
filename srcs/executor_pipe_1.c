/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:29:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:41:05 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	run_child_process(int fd_in, int fd_out,
				t_ast_node *node, t_shell *shell)
{
	int	status;

	if (fd_in != STDIN_FILENO)
		dup2(fd_in, STDIN_FILENO);
	if (fd_out != STDOUT_FILENO)
		dup2(fd_out, STDOUT_FILENO);
	status = execute_node(node, shell);
	rl_clear_history();
	cf_free_all();
	exit(status);
}

static void	close_fd_if_needed(int fd)
{
	if (fd != STDIN_FILENO)
		close(fd);
}

static int	setup_pipe_fork(t_ast_node *node, int pipe_in_fd,
				t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	else if (pid == 0)
	{
		close(pipefd[0]);
		run_child_process(pipe_in_fd, pipefd[1], node->left, shell);
	}
	close(pipefd[1]);
	close_fd_if_needed(pipe_in_fd);
	return (pipefd[0]);
}

static int	execute_last_pipe_node(t_ast_node *node, int pipe_in_fd,
				t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		close_fd_if_needed(pipe_in_fd);
		return (1);
	}
	else if (pid == 0)
		run_child_process(pipe_in_fd, STDOUT_FILENO, node, shell);
	close_fd_if_needed(pipe_in_fd);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	execute_pipe_node(t_ast_node *node, t_shell *shell)
{
	int		pipe_in_fd;

	if (!handle_heredocs_in_pipeline(node))
		return (1);
	pipe_in_fd = STDIN_FILENO;
	while (node && node->type == NODE_PIPE)
	{
		pipe_in_fd = setup_pipe_fork(node, pipe_in_fd, shell);
		if (pipe_in_fd == -1)
			return (1);
		node = node->right;
	}
	return (execute_last_pipe_node(node, pipe_in_fd, shell));
}
