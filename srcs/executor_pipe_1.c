/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:29:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 16:47:11 by vpushkar         ###   ########.fr       */
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
 * @brief Sets up a pipe and forks a child to execute the left node.
 *
 * Creates a pipe, forks a child process, and executes the left command
 * in the pipeline. The child's PID is stored in the PID list. Closes
 * unused file descriptors and returns the read end of the pipe for the
 * next command.
 *
 * @param node AST node containing the left and right commands of the pipe.
 * @param pipe_in_fd File descriptor for input to the current command.
 * @param shell Pointer to the shell structure.
 * @param pl Pointer to the PID list to track child processes.
 *
 * @return Read end of the pipe for the next command, or -1 on error.
 */
static int	setup_pipe_fork(t_ast_node *node, int pipe_in_fd,
				t_shell *shell, t_pid_list *pl)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (perror("pipe"), -1);
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
	pidlist_add(pl, pid);
	close(pipefd[1]);
	close_fd_if_needed(pipe_in_fd);
	return (pipefd[0]);
}

/**
 * @brief Executes the last command in a pipeline and tracks its PID.
 *
 * Forks a child to run the final command in the pipeline, sets up the
 * input file descriptor, stores the child's PID in the PID list, and
 * closes unused file descriptors. Returns 0 on success.
 *
 * @param node AST node of the command to execute.
 * @param pipe_in_fd Input file descriptor for the command.
 * @param shell Pointer to the shell structure.
 * @param pl Pointer to the PID list to store child PIDs.
 *
 * @return 0 on success, 1 if fork fails.
 */
static int	execute_last_pipe_node(t_ast_node *node, int pipe_in_fd,
				t_shell *shell, t_pid_list *pl)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		close_fd_if_needed(pipe_in_fd);
		return (1);
	}
	else if (pid == 0)
		run_child_process(pipe_in_fd, STDOUT_FILENO, node, shell);
	pidlist_add(pl, pid);
	pl->last = pid;
	close_fd_if_needed(pipe_in_fd);
	return (0);
}

/**
 * @brief Execute a pipeline of commands represented by AST nodes.
 *
 * This function sets up pipes and forks child processes for each
 * command in the pipeline. It also handles heredocs before execution
 * and waits for all child processes to finish.
 *
 * @param node Pointer to the root AST node of the pipeline.
 * @param shell Pointer to the shell context containing environment
 *				and execution state.
 * @return int Exit status of the last command in the pipeline or 1
 *				if an error occurs.
 */
int	execute_pipe_node(t_ast_node *node, t_shell *shell)
{
	int			pipe_in_fd;
	t_pid_list	pl;

	if (!handle_heredocs_in_pipeline(node))
		return (1);
	pl.n = 0;
	pl.last = -1;
	pipe_in_fd = STDIN_FILENO;
	while (node && node->type == NODE_PIPE)
	{
		pipe_in_fd = setup_pipe_fork(node, pipe_in_fd, shell, &pl);
		if (pipe_in_fd == -1)
			return (wait_pipeline(&pl));
		node = node->right;
	}
	if (execute_last_pipe_node(node, pipe_in_fd, shell, &pl))
		return (wait_pipeline(&pl));
	return (wait_pipeline(&pl));
}
