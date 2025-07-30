/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_second_part.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:47:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:45:14 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to check if token is an adjacent word token
bool	is_adjacent_word_token(t_token *token)
{
	return (token->type == TOKEN_SINGLE_QUOTE_WORD
		|| token->type == TOKEN_DOUBLE_QUOTE_WORD
		|| token->type == TOKEN_WORD);
}
