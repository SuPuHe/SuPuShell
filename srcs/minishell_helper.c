/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_helper.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 17:06:05 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/20 11:35:11 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a line is empty or contains only whitespace.
 *
 * Iterates through the line and determines if all characters are
 * whitespace. Updates the shell exit status to 2 if the line is
 * not empty but only whitespace is found.
 *
 * @param line The input string to check.
 * @param shell Pointer to the shell structure.
 * @return true if the line is empty or only whitespace.
 */
static bool	is_line_empty_or_whitespace(char *line, t_shell *shell)
{
	int	i;

	i = 0;
	while (line[i] && ft_isspace(line[i]))
		i++;
	if (!line[i])
		return (true);
	shell->last_exit_status = 2;
	return (true);
}

/**
 * @brief Validates the syntax of a parsed AST node.
 *
 * Checks if the AST node is NULL, or if it is not a command or
 * subshell node and lacks proper child nodes. Prints an error
 * message and updates the shell exit status if invalid.
 *
 * @param ast Pointer to the AST node to validate.
 * @param shell Pointer to the shell structure.
 * @return true if AST syntax is valid, false otherwise.
 */
static bool	validate_ast_syntax(t_ast_node *ast, t_shell *shell)
{
	if (!ast || (ast->type != NODE_CMD && ast->type != NODE_SUBSHELL
			&& (!ast->left || !ast->right)))
	{
		write(2, "Billyshell: syntax error near unexpected token\n", 48);
		shell->last_exit_status = 2;
		if (ast)
			free_ast(ast);
		return (false);
	}
	return (true);
}

/**
 * @brief Processes a single command line iteration in the shell.
 *
 * Handles history addition, input validation, parsing, AST syntax
 * checking, command execution, and AST cleanup.
 *
 * @param line Pointer to the input command line string.
 * @param shell Pointer to the shell structure.
 * @param interactive Flag indicating if the shell is in interactive mode.
 * @return true if the iteration was processed successfully, false otherwise.
 */
bool	process_one_command_iteration(char *line,
		t_shell *shell, int interactive)
{
	t_ast_node	*ast;

	ast = NULL;
	if (*line && interactive)
		add_history(line);
	if (!check_for_input(line))
		return (is_line_empty_or_whitespace(line, shell));
	ast = parse(line, shell);
	if (!validate_ast_syntax(ast, shell))
		return (true);
	execute_node(ast, shell);
	free_ast(ast);
	return (true);
}

/**
 * @brief Checks if a token is a command terminator.
 *
 * Returns true if the token is a pipe, AND, OR, right parenthesis,
 * or end token.
 *
 * @param token Pointer to the token to check.
 * @return true if token is a terminator, false otherwise.
 */
bool	is_command_terminator(t_token *token)
{
	return (token->type == TOKEN_PIPE || token->type == TOKEN_AND
		|| token->type == TOKEN_OR || token->type == TOKEN_RPAREN
		|| token->type == TOKEN_END);
}

/**
 * @brief Initializes the input structure for a command.
 *
 * Sets environment, syntax flag, and shell pointer in the input struct.
 *
 * @param input Pointer to the input structure to initialize.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 */
void	init_input_structure(t_input *input, t_env *env, t_shell *shell)
{
	ft_memset(input, 0, sizeof(t_input));
	input->env = env;
	input->syntax_ok = true;
	input->shell = shell;
	input->heredoc_processed = false;
}
