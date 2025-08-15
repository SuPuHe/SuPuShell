/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:29:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:59:39 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes a child process for a pipeline node.
 *
 * Sets up the input and output file descriptors, executes the given AST
 * node, cleans up the shell memory, and exits with the node's status.
 *
 * @param fd_in File descriptor to use as standard input.
 * @param fd_out File descriptor to use as standard output.
 * @param node Pointer to the AST node to execute.
 * @param shell Pointer to the shell structure.
 */
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

/**
 * @brief Closes a file descriptor if it is not STDIN.
 *
 * This helper function closes the given file descriptor only if it is
 * different from STDIN_FILENO to prevent closing standard input by mistake.
 *
 * @param fd File descriptor to close.
 */
static void	close_fd_if_needed(int fd)
{
	if (fd != STDIN_FILENO)
		close(fd);
}

/**
 * @brief Sets up a pipe and forks a child process for a pipeline node.
 *
 * Creates a new pipe, forks a child to execute the left command of the
 * pipeline, and manages the input/output file descriptors. Returns the
 * read end of the pipe to be used as input for the next command.
 *
 * @param node Pointer to the AST node representing the current pipe.
 * @param pipe_in_fd File descriptor for input to this command.
 * @param shell Pointer to the shell structure.
 *
 * @return Read end of the pipe for the next command, or -1 on error.
 */
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

/**
 * @brief Executes the last command in a pipeline.
 *
 * Forks a child process to execute the final command in the pipeline,
 * sets up the input file descriptor, waits for the child to finish,
 * and returns the exit status. Handles both normal exit and termination
 * by signal.
 *
 * @param node Pointer to the AST node representing the command.
 * @param pipe_in_fd File descriptor for the input of this command.
 * @param shell Pointer to the shell structure.
 *
 * @return Exit status of the last command in the pipeline.
 */
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

/**
 * @brief Executes a pipeline of commands represented by a pipe node.
 *
 * Iterates through the pipeline AST nodes, creating pipes and forking
 * child processes for each command. Handles heredocs before executing
 * the pipeline. The last node is executed separately to capture the
 * final exit status.
 *
 * @param node Pointer to the AST node representing the start of a pipe.
 * @param shell Pointer to the shell structure.
 *
 * @return The exit status of the last command in the pipeline.
 */
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
