/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:05:42 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 18:03:21 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Parses a primary expression (command or subshell).
 *
 * Handles parsing of a single command, subshell, or error token.
 * Returns the corresponding AST node or NULL on error.
 *
 * @param tokens Pointer to the token list.
 * @param shell Pointer to the shell structure.
 * @return Pointer to the parsed AST node, or NULL on error.
 */
t_ast_node	*parse_primary(t_list **tokens, t_shell *shell)
{
	t_token	*current_token;

	if (!*tokens)
		return (NULL);
	current_token = (t_token *)(*tokens)->content;
	if (current_token->type == TOKEN_LPAREN)
		return (parse_primary_subshell(tokens, shell));
	else if (current_token->type == TOKEN_WORD
		|| current_token->type == TOKEN_SINGLE_QUOTE_WORD
		|| current_token->type == TOKEN_DOUBLE_QUOTE_WORD
		|| current_token->type == TOKEN_REDIR_IN
		|| current_token->type == TOKEN_REDIR_OUT
		|| current_token->type == TOKEN_REDIR_APPEND
		|| current_token->type == TOKEN_HEREDOC)
		return (parse_primary_command(tokens, shell));
	else if (current_token->type == TOKEN_AMPERSAND)
	{
		shell->last_exit_status = 2;
		ft_putstr_fd("Billyshell: syntax error near unexpected token '&'\n", 2);
		return (NULL);
	}
	shell->last_exit_status = 0;
	return (NULL);
}

/**
 * @brief Parses a pipeline of commands separated by '|'.
 *
 * Builds an AST for a sequence of commands connected by pipes.
 * Returns the root AST node for the pipeline.
 *
 * @param tokens Pointer to the token list.
 * @param shell Pointer to the shell structure.
 * @return Pointer to the root AST node of the pipeline.
 */
t_ast_node	*parse_pipeline(t_list **tokens, t_shell *shell)
{
	t_ast_node	*left;
	t_ast_node	*right;

	left = parse_primary(tokens, shell);
	if (!left)
		return (NULL);
	while ((*tokens)->content
		&& ((t_token *)(*tokens)->content)->type == TOKEN_PIPE)
	{
		*tokens = (*tokens)->next;
		right = parse_primary(tokens, shell);
		if (!right)
			return (NULL);
		left = create_binary_node(NODE_PIPE, left, right);
	}
	return (left);
}

/**
 * @brief Parses logical AND/OR expressions in the command line.
 *
 * Handles parsing of '&&' and '||' operators, building the AST
 * for logical expressions. Returns the root AST node.
 *
 * @param tokens Pointer to the token list.
 * @param shell Pointer to the shell structure.
 * @return Pointer to the root AST node for the logical expression.
 */
t_ast_node	*parse_and_or(t_list **tokens, t_shell *shell)
{
	t_ast_node		*left;
	t_token_type	op_type;
	t_ast_node		*right;
	t_node_type		node_type;

	left = parse_pipeline(tokens, shell);
	if (!left)
		return (NULL);
	while ((*tokens)->content
		&& (((t_token *)(*tokens)->content)->type == TOKEN_AND
		|| ((t_token *)(*tokens)->content)->type == TOKEN_OR))
	{
		op_type = ((t_token *)(*tokens)->content)->type;
		*tokens = (*tokens)->next;
		right = parse_pipeline(tokens, shell);
		if (!right)
			return (NULL);
		if (op_type == TOKEN_AND)
			node_type = NODE_AND;
		else
			node_type = NODE_OR;
		left = create_binary_node(node_type, left, right);
	}
	return (left);
}

/**
 * @brief Parses a complete command expression from tokens.
 *
 * This function is the main entry point for parsing a command line.
 * It handles the entire parsing process, including primary commands,
 * pipelines, and logical expressions.
 *
 * @param tokens Pointer to the token list.
 * @param shell Pointer to the shell structure.
 * @return Pointer to the root AST node of the parsed expression.
 */
t_ast_node	*parse_expression(t_list **tokens, t_shell *shell)
{
	return (parse_and_or(tokens, shell));
}

/**
 * @brief Parses a command line string into an AST.
 *
 * Tokenizes the input line, parses it into an AST structure,
 * and returns the root node of the AST.
 *
 * @param line Input command line string.
 * @param shell Pointer to the shell structure.
 * @return Pointer to the root AST node, or NULL on error.
 */
t_ast_node	*parse(const char *line, t_shell *shell)
{
	t_list		*tokens;
	t_ast_node	*ast;

	tokens = tokenize(line);
	if (!tokens)
		return (NULL);
	ast = parse_expression(&tokens, shell);
	if (ast && ((t_token *)tokens->content)->type == TOKEN_END)
		;
	else
	{
		free_ast(ast);
		ast = NULL;
	}
	if (tokens)
		free_token_list(tokens);
	tokens = NULL;
	return (ast);
}
