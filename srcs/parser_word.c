/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:15:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:42:08 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to handle heredoc fallback
void	handle_heredoc_fallback(t_input *input)
{
	if ((!input->args || !input->args[0]) && input->heredoc)
	{
		input->args = cf_malloc(sizeof(char *) * 2);
		if (!input->args)
		{
			input->syntax_ok = false;
			return ;
		}
		input->args[0] = cf_strdup(":");
		input->args[1] = NULL;
	}
}

// Helper function to handle regular word
void	handle_regular_word(t_input *input, char *expanded_value)
{
	if (ft_strlen(expanded_value) > 0)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
	}
	else
		cf_free_one(expanded_value);
}

// Helper function to handle wildcard expansion
void	handle_wildcard_expansion(t_input *input,
	t_token *current_tok, char *expanded_value)
{
	char	**wildcards;
	int		i;

	if (current_tok->type == TOKEN_WORD
		&& (ft_strchr(expanded_value, '*') || ft_strchr(expanded_value, '?')))
	{
		wildcards = expand_wildcards(expanded_value);
		if (wildcards)
		{
			i = 0;
			while (wildcards[i])
			{
				input->args = append_arg(input->args, wildcards[i]);
				i++;
			}
			free_expanded_wildcards(wildcards);
		}
		else
			input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
	}
	else
		handle_regular_word(input, expanded_value);
}

// Helper function to concatenate adjacent tokens
char	*concatenate_adjacent_tokens(t_list **current_tokens,
	char *expanded_value, t_env *env, t_shell *shell)
{
	t_list	*next_token;
	char	*next_expanded;
	char	*combined;

	next_token = (*current_tokens)->next;
	while (next_token && !((t_token *)next_token->content)->has_space
		&& is_adjacent_word_token((t_token *)next_token->content))
	{
		next_expanded = expand_token_value((t_token *)next_token->content,
				env, shell);
		if (next_expanded)
		{
			combined = ft_strjoin_free(expanded_value, next_expanded);
			cf_free_one(expanded_value);
			cf_free_one(next_expanded);
			expanded_value = combined;
			*current_tokens = next_token;
			next_token = next_token->next;
		}
		else
			break ;
	}
	return (expanded_value);
}

bool	handle_word_token(t_input *input,
	t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_token	*current_tok;
	char	*expanded_value;

	current_tok = (t_token *)(*current_tokens)->content;
	expanded_value = expand_token_value(current_tok, env, shell);
	if (!expanded_value)
		return (input->syntax_ok = false, false);
	expanded_value = concatenate_adjacent_tokens(current_tokens,
			expanded_value, env, shell);
	handle_wildcard_expansion(input, current_tok, expanded_value);
	*current_tokens = (*current_tokens)->next;
	return (true);
}
