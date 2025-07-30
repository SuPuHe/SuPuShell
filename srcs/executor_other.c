/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_other.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:30:54 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 12:56:47 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helpers for execute_node: handle AND (&&) AST nodes
int	execute_and_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;

	last_status = execute_node(node->left, shell);
	if (last_status == 0)
		last_status = execute_node(node->right, shell);
	return (last_status);
}

// Helpers for execute_node: handle OR (||) AST nodes
int	execute_or_node(t_ast_node *node, t_shell *shell)
{
	int	last_status;

	last_status = execute_node(node->left, shell);
	if (last_status != 0)
		last_status = execute_node(node->right, shell);
	return (last_status);
}

// Helpers for execute_node: handle subshell child process
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

// Helpers for execute_node: handle subshell AST nodes
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
