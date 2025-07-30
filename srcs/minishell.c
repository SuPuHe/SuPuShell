/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 18:12:26 by vpushkar         ###   ########.fr       */
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
 * @brief Reads and processes user input, updating the AST.
 *
 * Reads a line from the user, sets up signals, and parses the input
 * into an AST node. Handles SIGINT status and returns the input line.
 *
 * @param shell Pointer to the shell structure.
 * @param ast Pointer to the AST node pointer to update.
 * @return The input line string (must be freed by the caller).
 */
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

/**
 * @brief Processes a command line: history, validation, and execution.
 *
 * Adds the line to history if interactive, checks for valid input,
 * and executes the AST if present. Frees the AST after execution.
 *
 * @param line The input line string.
 * @param ast Pointer to the AST node pointer.
 * @param shell Pointer to the shell structure.
 * @param interactive Interactive mode flag.
 */
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
		if (shell.should_exit)
		{
			rl_clear_history();
			cf_free_all();
			break ;
		}
	}
	rl_clear_history();
	cf_free_all();
	return (shell.last_exit_status);
}

//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
