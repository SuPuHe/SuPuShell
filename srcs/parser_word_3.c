/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:07:56 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 17:10:04 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a token is quoted.
 *
 * Determines whether a token was originally enclosed in single or double quotes.
 *
 * @param token Pointer to the token to check.
 * @return true if the token is single- or double-quoted, false otherwise.
 */
bool	is_token_quoted(t_token *token)
{
	if (!token)
		return (false);
	return (token->type == TOKEN_SINGLE_QUOTE_WORD
		|| token->type == TOKEN_DOUBLE_QUOTE_WORD);
}

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
		return (false);
	return (token->type == TOKEN_SINGLE_QUOTE_WORD
		|| token->type == TOKEN_DOUBLE_QUOTE_WORD
		|| token->type == TOKEN_WORD);
}

/**
 * @brief Creates a single token part from a token.
 *
 * Allocates a new t_token_part, sets start/end positions and
 * quotation status.
 *
 * @param token Pointer to the token.
 * @param start_pos Start position of the token in the combined string.
 * @return Pointer to the newly allocated token part,
 * 			or NULL on allocation failure.
 */
static t_token_part	*create_token_part(t_token *token, int start_pos)
{
	t_token_part	*part;

	part = cf_malloc(sizeof(t_token_part));
	if (!part)
		return (NULL);
	part->start_pos = start_pos;
	part->end_pos = start_pos + ft_strlen(token->value) - 1;
	part->is_quoted = is_token_quoted(token);
	part->next = NULL;
	return (part);
}

/**
 * @brief Appends a token part to the linked list.
 *
 * Updates the head and current_part pointers and returns the new current.
 *
 * @param head Pointer to the head of the list (may be updated if empty).
 * @param current_part Current last node of the list.
 * @param new_part New token part to append.
 * @return Updated current_part pointer after appending.
 */
static t_token_part	*append_token_part(t_token_part **head,
	t_token_part *current_part, t_token_part *new_part)
{
	if (!*head)
	{
		*head = new_part;
		return (new_part);
	}
	else
	{
		current_part->next = new_part;
		return (new_part);
	}
}

/**
 * @brief Creates a linked list of token parts from a range of tokens.
 *
 * Iterates from tokens_start up to (but not including) tokens_end,
 * creating t_token_part nodes for each token and linking them into a list.
 * Frees already allocated parts if any allocation fails.
 *
 * @param tokens_start Start of the token list.
 * @param tokens_end End of the token list (exclusive).
 * @return Pointer to the head of the linked list of token parts,
 *			or NULL if allocation failed.
 */
t_token_part	*create_token_parts_list(t_list *tokens_start,
	t_list *tokens_end)
{
	t_token_part	*head;
	t_token_part	*current_part;
	t_token_part	*new_part;
	t_list			*token_node;
	int				pos;

	head = NULL;
	current_part = NULL;
	token_node = tokens_start;
	pos = 0;
	while (token_node && token_node != tokens_end)
	{
		new_part = create_token_part((t_token *)token_node->content, pos);
		if (!new_part)
		{
			free_token_parts(head);
			return (NULL);
		}
		current_part = append_token_part(&head, current_part, new_part);
		pos += ft_strlen(((t_token *)token_node->content)->value);
		token_node = token_node->next;
	}
	return (head);
}
