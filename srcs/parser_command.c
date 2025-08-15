/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:08:27 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:06:09 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

/**
 * @brief Parses a subshell expression from tokens.
 *
 * Advances tokens, parses the subshell, and returns the AST node.
 * Returns NULL on error or syntax issues.
 *
 * @param tokens Pointer to the token list.
 * @param shell Pointer to the shell structure.
 * @return Pointer to the subshell AST node, or NULL on error.
 */
t_ast_node	*parse_primary_subshell(t_list **tokens, t_shell *shell)
{
	t_ast_node	*sub_expr;
	t_ast_node	*node;

	*tokens = (*tokens)->next;
	sub_expr = parse_expression(tokens, shell);
	if (!sub_expr)
		return (NULL);
	if (!*tokens || ((t_token *)(*tokens)->content)->type != TOKEN_RPAREN)
	{
		shell->last_exit_status = 2;
		free_ast(sub_expr);
		return (NULL);
	}
	*tokens = (*tokens)->next;
	node = create_subshell_node(sub_expr);
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
