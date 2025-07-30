/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_main.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:25:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:50:45 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helpers for execute_node: check if command is a built-in
bool	is_builtin_command(const char *command)
{
	return (ft_strncmp(command, "cd", 3) == 0
		|| ft_strncmp(command, "export", 7) == 0
		|| ft_strncmp(command, "unset", 6) == 0
		|| ft_strncmp(command, "exit", 5) == 0
		|| ft_strncmp(command, "echo", 5) == 0
		|| ft_strncmp(command, "env", 4) == 0
		|| ft_strncmp(command, "pwd", 4) == 0
		|| ft_strncmp(command, ":", 2) == 0);
}

// Helpers for execute_node: handle external (non-built-in) commands
int	execute_external_command(t_input *command, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		last_status;

	last_status = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == 0)
		execute_external_child(command, shell);
	else if (pid < 0)
	{
		perror("fork");
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

// Helpers for execute_node: handle built-in shell commands
int	execute_builtin_command(t_input *command, t_shell *shell)
{
	int	last_status;

	last_status = 0;
	if (ft_strncmp(command->args[0], "cd", 3) == 0)
		do_cd(command->args, &shell->env);
	else if (ft_strncmp(command->args[0], "export", 7) == 0)
		do_export(command->args, &shell->env);
	else if (ft_strncmp(command->args[0], "unset", 6) == 0)
		do_unset(command->args, &shell->env);
	else if (ft_strncmp(command->args[0], "exit", 5) == 0)
	{
		shell->should_exit = 1;
		return (0);
	}
	else if (ft_strncmp(command->args[0], "echo", 5) == 0)
		do_echo(command->args);
	else if (ft_strncmp(command->args[0], "env", 4) == 0)
		do_env(shell->env);
	else if (ft_strncmp(command->args[0], "pwd", 4) == 0)
		do_pwd();
	else if (ft_strncmp(command->args[0], ":", 2) == 0)
		last_status = 0;
	return (last_status);
}

// Helpers for execute_node: handle command AST nodes (built-in or external)
int	execute_command_node(t_ast_node *node, t_shell *shell)
{
	int		last_status;

	last_status = 0;
	if (!apply_redirections(node->command))
		last_status = 1;
	else if (node->command->args && node->command->args[0])
	{
		if (is_builtin_command(node->command->args[0]))
			last_status = execute_builtin_command(node->command, shell);
		else
			last_status = execute_external_command(node->command, shell);
	}
	return (last_status);
}

// Main dispatcher for AST execution
int	execute_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;
	int	original_stdin;
	int	original_stdout;

	last_status = 0;
	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	if (!node)
		return (1);
	if (node->type == NODE_CMD)
		last_status = execute_command_node(node, shell);
	else if (node->type == NODE_PIPE)
		last_status = execute_pipe_node(node, shell);
	else if (node->type == NODE_AND)
		last_status = execute_and_node(node, shell);
	else if (node->type == NODE_OR)
		last_status = execute_or_node(node, shell);
	else if (node->type == NODE_SUBSHELL)
		last_status = execute_subshell_node(node, shell);
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
	shell->last_exit_status = last_status;
	return (last_status);
}
