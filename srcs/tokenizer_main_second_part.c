/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_main_second_part.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:48:36 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:31:13 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Processes and creates the next token from the input line.
 *
 * Skips whitespace, detects operators, redirections, quotes, or words,
 * and creates the corresponding token.
 *
 * @param line Input line string.
 * @param i Pointer to current index.
 * @param tokens_list Pointer to token list.
 * @param had_space Pointer to space flag.
 * @return Pointer to created token, or NULL if end of line.
 */
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

/**
 * @brief Adds an end token to the token list.
 *
 * Creates a TOKEN_END token and appends it to the token list.
 *
 * @param tokens Pointer to token list.
 */
void	add_end_token(t_list **tokens)
{
	t_token	*end_token;
	t_list	*end_node;

	end_token = create_token(TOKEN_END, NULL);
	end_token->has_space = false;
	end_node = cf_lstnew(end_token);
	ft_lstadd_back(tokens, end_node);
}
