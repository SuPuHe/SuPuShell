/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:29:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/06 17:02:41 by vpushkar         ###   ########.fr       */
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
int execute_pipe_node(t_ast_node *node, t_shell *shell)
{
	int		pipefd[2];
	pid_t	left_pid;
	pid_t	right_pid;
	int		status;
	int		last_status;
	int		heredoc_fd = -1;
	bool	has_heredoc = false;
	bool	is_last_command = true;  // Добавляем флаг для последней команды

	// Проверяем, является ли правая часть последней командой в пайплайне
	if (node->right && node->right->type == NODE_PIPE)
		is_last_command = false;

	// Проверяем наличие heredoc
	if (node->right && node->right->type == NODE_CMD &&
		node->right->command && node->right->command->heredoc_count > 0)
	{
		has_heredoc = true;
		if (!handle_heredoc(node->right->command))
			return 1;
		// Открываем heredoc файл для чтения только если это последняя команда
		if (is_last_command && node->right->command->infile)
			heredoc_fd = open(node->right->command->infile, O_RDONLY);
	}

	last_status = 0;
	if (pipe(pipefd) == -1)
	{
		if (heredoc_fd != -1)
			close(heredoc_fd);
		perror("pipe");
		return 1;
	}

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	// Создаем левый процесс всегда, если это не последняя команда с heredoc
	if (!has_heredoc || !is_last_command)
	{
		left_pid = fork();
		if (left_pid == 0)
		{
			if (heredoc_fd != -1)
				close(heredoc_fd);
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			status = execute_node(node->left, shell);
			exit(status);
		}
	}

	// Создаем правый процесс
	right_pid = fork();
	if (right_pid == 0)
	{
		close(pipefd[1]);

		// Если это последняя команда и есть heredoc, используем его
		if (is_last_command && has_heredoc && heredoc_fd != -1)
		{
			dup2(heredoc_fd, STDIN_FILENO);
			close(heredoc_fd);
			close(pipefd[0]);
		}
		else
		{
			// Иначе читаем из пайпа
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
		}

		status = execute_node(node->right, shell);
		exit(status);
	}

	// Закрываем дескрипторы в родителе
	if (heredoc_fd != -1)
		close(heredoc_fd);
	close(pipefd[0]);
	close(pipefd[1]);

	// Ждем завершения процессов
	if (!has_heredoc || !is_last_command)
		waitpid(left_pid, &status, 0);
	waitpid(right_pid, &status, 0);

	if (WIFEXITED(status))
		last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		last_status = 128 + WTERMSIG(status);

	return last_status;
}
//❌ 185
//before ❌ 192
