/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:47:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 17:40:35 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a token is an adjacent word token.
 *
 * Returns true if the token is a single-quoted, double-quoted,
 * or regular word token.
 *
 * @param token Pointer to token to check.
 * @return true if adjacent word token, false otherwise.
 */
bool	is_adjacent_word_token(t_token *token)
{
	if (!token)
		return false;
	return (token->type == TOKEN_SINGLE_QUOTE_WORD
		|| token->type == TOKEN_DOUBLE_QUOTE_WORD
		|| token->type == TOKEN_WORD);
}
