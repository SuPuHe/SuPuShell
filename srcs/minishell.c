/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/08/05 17:18:48 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Initializes the shell structure and environment variables.
 *
 * Sets up the shell state, disables echoctl, and prints the prompt if
 * running in interactive mode.
 *
 * @param shell Pointer to the shell structure to initialize.
 * @param envp Array of environment variable strings.
 * @param interactive Pointer to the interactive mode flag.
 */
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

/**
 * @brief Reads a line of input from the shell prompt or stdin.
 *
 * Sets up signal handlers, displays the prompt in interactive mode,
 * and reads input using readline or get_next_line. Updates shell exit
 * status if interrupted by SIGINT.
 *
 * @param shell Pointer to the shell structure.
 * @return Pointer to the input line (must be freed by caller).
 */
static char	*read_shell_input(t_shell *shell)
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
	return (line);
}

/**
 * @brief Processes a single command iteration in the shell loop.
 *
 * Adds input to history if interactive, checks for syntax errors,
 * parses the input into an AST, executes the command, and frees the AST.
 * Updates shell exit status on error.
 *
 * @param line Input line to process.
 * @param shell Pointer to the shell structure.
 * @param interactive Interactive mode flag.
 * @return true to continue loop, false to exit.
 */
static bool	process_one_command_iteration(char *line, t_shell *shell,
	int interactive)
{
	t_ast_node	*ast;
	int			i;

	i = 0;
	ast = NULL;
	if (*line && interactive)
		add_history(line);
	if (!check_for_input(line))
	{
		while (line[i] && ft_isspace(line[i]))
			i++;
		if (!line[i])
			return (true);
		return (shell->last_exit_status = 2, true);
	}
	ast = parse(line, shell);
	if (!ast
		|| (ast->type != NODE_CMD && ast->type != NODE_SUBSHELL
			&& (!ast->left || !ast->right)))
	{
		write(2, "Billyshell: syntax error near unexpected token\n", 48);
		shell->last_exit_status = 2;
		if (ast)
			free_ast(ast);
		return (true);
	}
	if (ast)
	{
		execute_node(ast, shell);
		free_ast(ast);
		ast = NULL;
	}
	return (true);
}

/**
 * @brief Main function for the minishell program.
 *
 * Initializes the shell, reads input in a loop, processes commands,
 * and exits cleanly when done. Handles interactive and non-interactive modes.
 *
 * @param argc Argument count.
 * @param argv Argument vector (not used).
 * @param envp Environment variables.
 * @return Exit status of the shell.
 */
int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_shell	shell;
	int		interactive;

	interactive = 0;
	(void)argv;
	if (argc != 1)
		return (1);
	init_shell(&shell, envp, &interactive);
	while (1)
	{
		line = read_shell_input(&shell);
		if (!line)
			break ;
		process_one_command_iteration(line, &shell, interactive);
		free(line);
		if (shell.should_exit)
			break ;
	}
	rl_clear_history();
	cf_free_all();
	return (shell.last_exit_status);
}
