/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:08:27 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:41:09 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to check if token is a command terminator
bool	is_command_terminator(t_token *token)
{
	return (token->type == TOKEN_PIPE || token->type == TOKEN_AND
		|| token->type == TOKEN_OR || token->type == TOKEN_RPAREN
		|| token->type == TOKEN_END);
}

// Helper function to initialize input structure
void	init_input_structure(t_input *input, t_env *env, t_shell *shell)
{
	ft_memset(input, 0, sizeof(t_input));
	input->env = env;
	input->syntax_ok = true;
	input->shell = shell;
}

// part of parse_primary
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

// part of parse_primary
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
			if (!handle_redirection_token(&input, current_tokens, env, shell))
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
