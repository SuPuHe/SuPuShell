/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 15:44:09 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_shell(t_shell *shell, char **envp, int *interactive)
{
	shell->env = create_env(envp);
	shell->last_exit_status = 0;
	shell->should_exit = 0;
	disable_echoctl();
	if (isatty(STDIN_FILENO))
	{
		*interactive = 1;
		billy_print();
	}
}

static char	*handle_input(t_shell *shell, t_ast_node **ast)
{
	char	*line;

	setup_signal();
	if (isatty(STDIN_FILENO))
		line = readline(SHELLNAME);
	else
		line = get_next_line(STDIN_FILENO);
	if (g_sigint_exit_status == 1)
	{
		shell->last_exit_status = 1;
		g_sigint_exit_status = 0;
	}
	if (line)
		*ast = parse(line, shell);
	return (line);
}

static void	process_command(char *line, t_ast_node **ast,
	t_shell *shell, int interactive)
{
	if (*line && interactive)
		add_history(line);
	if (!check_for_input(line))
		return ;
	if (*ast)
	{
		execute_node(*ast, shell);
		free_ast(*ast);
		*ast = NULL;
	}
}

int	main(int argc, char **argv, char **envp)
{
	char		*line;
	t_shell		shell;
	int			interactive;
	t_ast_node	*ast;

	interactive = 0;
	ast = NULL;
	(void)argv;
	if (argc != 1)
		return (1);
	init_shell(&shell, envp, &interactive);
	while (1)
	{
		line = handle_input(&shell, &ast);
		if (!line || shell.should_exit)
			break ;
		process_command(line, &ast, &shell, interactive);
		free(line);
	}
	rl_clear_history();
	cf_free_all();
	return (0);
}

//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
