/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:05:42 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:44:20 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast_node	*parse_primary(t_list **tokens, t_shell *shell)
{
	t_token	*current_token;

	if (!*tokens)
		return (NULL);
	current_token = (t_token *)(*tokens)->content;
	if (current_token->type == TOKEN_LPAREN)
		return (parse_primary_subshell(tokens, shell));
	else if (current_token->type == TOKEN_WORD
		|| current_token->type == TOKEN_REDIR_IN
		|| current_token->type == TOKEN_REDIR_OUT
		|| current_token->type == TOKEN_REDIR_APPEND
		|| current_token->type == TOKEN_HEREDOC)
		return (parse_primary_command(tokens, shell));
	else if (current_token->type == TOKEN_AMPERSAND)
	{
		shell->last_exit_status = 2;
		ft_printf("SuPuShell: syntax error near unexpected token '&'\n");
		return (NULL);
	}
	shell->last_exit_status = 0;
	return (NULL);
}

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

t_ast_node	*parse_expression(t_list **tokens, t_shell *shell)
{
	return (parse_and_or(tokens, shell));
}

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
