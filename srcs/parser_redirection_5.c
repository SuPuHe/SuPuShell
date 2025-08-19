/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_5.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 16:46:44 by omizin            #+#    #+#             */
/*   Updated: 2025/08/19 16:46:55 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Collects the delimiter for a heredoc (<<) operator.
 *
 * Starting from the current token list node, this function gathers all tokens
 * that form the heredoc delimiter into a string builder. It also determines
 * whether the delimiter was quoted, which affects whether variable expansion
 * should occur in the heredoc body.
 *
 * @param current_tokens Pointer to the token list iterator; will be advanced
 *						past the delimiter tokens.
 * @param quoted Output flag set to true if the delimiter is quoted,
 * 					false otherwise.
 * @return A newly allocated string containing the heredoc delimiter,
 *			or NULL if allocation failed or tokens are missing.
 */
char	*collect_heredoc_delimiter(t_list **current_tokens, bool *quoted)
{
	t_list				*start;
	t_string_builder	*sb;

	if (!current_tokens || !*current_tokens || !(*current_tokens)->content)
		return (NULL);
	start = *current_tokens;
	sb = sb_create();
	if (!sb)
		return (NULL);
	*current_tokens = collect_d_tokens_and_sb(start, sb, quoted);
	return (sb_build_and_destroy(sb));
}
