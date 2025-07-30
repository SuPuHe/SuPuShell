/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_main_second_part.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:48:36 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 15:01:25 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to process and create individual tokens
t_token	*process_next_token(const char *line,
	int *i, t_list **tokens_list, bool *had_space)
{
	t_token	*new_token;
	char	*word_str;

	skip_whitespace(line, i, had_space);
	if (!line[*i])
		return (NULL);
	new_token = handle_operator_tokens(line, i);
	if (!new_token)
		new_token = handle_redirection_tokens(line, i);
	if (!new_token)
		new_token = handle_quote_tokens(line, i, tokens_list);
	if (!new_token)
	{
		word_str = extract_non_quoted_word(line, i);
		new_token = create_token(TOKEN_WORD, word_str);
	}
	return (new_token);
}

// Helper function to add end token
void	add_end_token(t_list **tokens)
{
	t_token	*end_token;
	t_list	*end_node;

	end_token = create_token(TOKEN_END, NULL);
	end_token->has_space = false;
	end_node = cf_lstnew(end_token);
	ft_lstadd_back(tokens, end_node);
}
