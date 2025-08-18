/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_4.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:17:57 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 13:43:55 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Validates and expands a filename token for redirection.
 *
 * Ensures the current token exists and expands it using environment
 * variables. If validation or expansion fails, marks syntax as invalid.
 *
 * @param input Input structure holding parsing state.
 * @param current_tokens Pointer to the current token list iterator.
 * @param env Environment variable list for expansion.
 * @return Expanded filename string, or NULL on error.
 */
char	*get_expanded_filename(t_input *input,
			t_list **current_tokens, t_env *env)
{
	t_token	*filename_token;
	char	*expanded_value;

	filename_token = (t_token *)(*current_tokens)->content;
	if (!filename_token || !filename_token->value)
	{
		input->syntax_ok = false;
		return (NULL);
	}
	expanded_value = expand_filename_token(filename_token, env, input->shell);
	if (!expanded_value)
	{
		input->syntax_ok = false;
		return (NULL);
	}
	return (expanded_value);
}

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
