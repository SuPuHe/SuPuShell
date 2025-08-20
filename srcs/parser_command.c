/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:08:27 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/20 11:34:54 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks for a closing parenthesis in the token stream.
 *
 * Ensures that the current token is a closing parenthesis. If not,
 * it sets the shell exit status to 2 and frees the parsed sub-expression.
 *
 * @param tokens Pointer to the current token list.
 * @param sub_expr Parsed sub-expression AST node.
 * @param shell Pointer to the shell context.
 * @return true if a valid closing parenthesis is found, false otherwise.
 */
static bool	check_closing_paren(t_list **tokens,
	t_ast_node *sub_expr, t_shell *shell)
{
	if (!*tokens || ((t_token *)(*tokens)->content)->type != TOKEN_RPAREN)
	{
		shell->last_exit_status = 2;
		free_ast(sub_expr);
		return (false);
	}
	*tokens = (*tokens)->next;
	return (true);
}

/**
 * @brief Parses redirection tokens following a subshell expression.
 *
 * Handles input/output redirections and heredoc tokens that appear
 * after a subshell expression. Allocates and initializes an input
 * structure when needed, and validates all redirections.
 *
 * @param tokens Pointer to the current token list.
 * @param shell Pointer to the shell context.
 * @param sub_expr The parsed subshell AST node (used for cleanup on failure).
 * @return Pointer to the redirection input structure, or NULL on failure.
 */
static t_input	*parse_subshell_redirs(t_list **tokens,
	t_shell *shell, t_ast_node *sub_expr)
{
	t_input	*redir_input;

	redir_input = NULL;
	while (*tokens
		&& ((t_token *)(*tokens)->content)->type >= TOKEN_REDIR_IN
		&& ((t_token *)(*tokens)->content)->type <= TOKEN_HEREDOC)
	{
		if (!redir_input)
		{
			redir_input = cf_malloc(sizeof(t_input));
			if (!redir_input)
				return (free_ast(sub_expr), NULL);
			init_input_structure(redir_input, shell->env, shell);
		}
		if (!handle_redirection_token(redir_input, tokens, shell->env))
		{
			free_ast(sub_expr);
			free_heredoc_delimiters(redir_input);
			cf_free_one(redir_input);
			return (NULL);
		}
	}
	return (redir_input);
}

/**
 * @brief Parses a subshell expression inside parentheses.
 *
 * Handles parsing of subshells of the form `( ... )`, including
 * optional redirections after the closing parenthesis.
 * Creates a subshell AST node with the parsed expression and
 * associated redirections.
 *
 * @param tokens Pointer to the current token list.
 * @param shell Pointer to the shell context.
 * @return Pointer to the created AST node, or NULL on failure.
 */
t_ast_node	*parse_primary_subshell(t_list **tokens, t_shell *shell)
{
	t_ast_node	*sub_expr;
	t_input		*redir_input;
	t_ast_node	*node;

	*tokens = (*tokens)->next;
	sub_expr = parse_expression(tokens, shell);
	if (!sub_expr)
		return (NULL);
	if (!check_closing_paren(tokens, sub_expr, shell))
		return (NULL);
	redir_input = parse_subshell_redirs(tokens, shell, sub_expr);
	if (!redir_input && shell->last_exit_status == 2)
		return (NULL);
	node = create_subshell_node(sub_expr);
	if (node && redir_input)
		node->command = redir_input;
	return (node);
}

/**
 * @brief Parses a command expression from tokens.
 *
 * Allocates and fills the input structure, parses the command,
 * and returns the AST node. Returns NULL on error.
 *
 * @param tokens Pointer to the token list.
 * @param shell Pointer to the shell structure.
 * @return Pointer to the command AST node, or NULL on error.
 */
t_ast_node	*parse_primary_command(t_list **tokens, t_shell *shell)
{
	t_input		*cmd_data;
	t_ast_node	*node;

	cmd_data = cf_malloc(sizeof(t_input));
	if (!cmd_data)
		return (NULL);
	ft_memset(cmd_data, 0, sizeof(t_input));
	*cmd_data = parse_command_from_tokens(tokens, shell->env, shell);
	if (!cmd_data->syntax_ok)
	{
		cf_free_one(cmd_data);
		shell->last_exit_status = 2;
		return (NULL);
	}
	node = create_cmd_node(cmd_data);
	return (node);
}

/**
 * @brief Parses a command from tokens and fills the input structure.
 *
 * Iterates through tokens, handling redirections, words, and ampersands.
 * Sets flags and arguments in the input structure.
 *
 * @param current_tokens Pointer to the token list pointer.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Filled input structure for the command.
 */
t_input	parse_command_from_tokens(t_list **current_tokens,
	t_env *env, t_shell *shell)
{
	t_input	input;
	t_token	*current_tok;

	init_input_structure(&input, env, shell);
	while (*current_tokens
		&& !is_command_terminator((t_token *)(*current_tokens)->content))
	{
		current_tok = (t_token *)(*current_tokens)->content;
		if (current_tok->type >= TOKEN_REDIR_IN
			&& current_tok->type <= TOKEN_HEREDOC)
		{
			if (!handle_redirection_token(&input, current_tokens, env))
				return (input);
		}
		else if (current_tok->type == TOKEN_WORD
			|| current_tok->type == TOKEN_SINGLE_QUOTE_WORD
			|| current_tok->type == TOKEN_DOUBLE_QUOTE_WORD)
		{
			if (!handle_word_token(&input, current_tokens, env, shell))
				return (input);
		}
		else
			return (input.syntax_ok = false, input);
	}
	return (handle_heredoc_fallback(&input), input);
}
