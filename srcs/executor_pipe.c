/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:29:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/07 17:23:34 by omizin           ###   ########.fr       */
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


bool handle_heredocs_in_pipeline(t_ast_node *node)
{
	if (!node)
		return true;

	// Base case: If we're at a command node with heredocs, handle them
	if (node->type == NODE_CMD && node->command->heredoc_count > 0)
	{
		if (!handle_heredoc(node->command))
			return false;
	}
	// Recursive calls for pipe nodes
	else if (node->type == NODE_PIPE)
	{
		if (!handle_heredocs_in_pipeline(node->left))
			return false;
		if (!handle_heredocs_in_pipeline(node->right))
			return false;
	}
	return true;
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
int execute_pipe_node(t_ast_node *node, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;
	int		pipe_in_fd; // Stores the read end of the previous pipe

	// This function handles all heredocs for the entire pipeline at once
	if (!handle_heredocs_in_pipeline(node))
		return 1;

	pipe_in_fd = STDIN_FILENO;

	while (node && node->type == NODE_PIPE)
	{
		if (pipe(pipefd) == -1)
		{
			perror("pipe");
			return 1;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			close(pipefd[0]);
			close(pipefd[1]);
			return 1;
		}
		else if (pid == 0) // Child process for the left command
		{
			close(pipefd[0]); // Child will not read from this pipe

			// Set stdin: pipe from the previous command or the original stdin
			dup2(pipe_in_fd, STDIN_FILENO);
			if (pipe_in_fd != STDIN_FILENO)
				close(pipe_in_fd);

			// Set stdout: pipe to the next command
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);

			// Execute the left command
			status = execute_node(node->left, shell);
			rl_clear_history();
			cf_free_all();
			exit(status);
		}

		// Parent process
		close(pipefd[1]); // Parent will not write to this pipe
		if (pipe_in_fd != STDIN_FILENO)
			close(pipe_in_fd);
		pipe_in_fd = pipefd[0]; // The read end of this pipe becomes the input for the next command
		node = node->right; // Move to the next command in the pipeline
	}

	// Now execute the last command in the pipeline
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (pipe_in_fd != STDIN_FILENO)
			close(pipe_in_fd);
		return 1;
	}
	else if (pid == 0) // Child process for the last command
	{
		// Set stdin: pipe from the previous command or the original stdin
		dup2(pipe_in_fd, STDIN_FILENO);
		if (pipe_in_fd != STDIN_FILENO)
			close(pipe_in_fd);

		// This is the key: `execute_node` will now handle heredocs/redirections
		// for the last command, and they will take precedence over the pipe input
		status = execute_node(node, shell);
		rl_clear_history();
		cf_free_all();
		exit(status);
	}

	// Parent process waits for all children and cleans up
	if (pipe_in_fd != STDIN_FILENO)
		close(pipe_in_fd);
	waitpid(pid, &status, 0);

	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);

	return 1;
}
//❌ 185
//before ❌ 192
