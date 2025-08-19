/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:47:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 17:20:15 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees a linked list of token parts.
 *
 * Iterates through the linked list of t_token_part structures and frees
 * each node using centralized memory management.
 *
 * @param parts Pointer to the head of the token parts list.
 */
void	free_token_parts(t_token_part *parts)
{
	t_token_part	*next;

	while (parts)
	{
		next = parts->next;
		cf_free_one(parts);
		parts = next;
	}
}

/**
 * @brief Checks if the character at index is inside a quoted token part.
 *
 * @param parts Linked list of token parts.
 * @param index Index of the character in the string.
 * @return true if the character is inside a quoted part, false otherwise.
 */
bool	is_in_quoted_part(t_token_part *parts, int index)
{
	t_token_part	*current;

	current = parts;
	while (current)
	{
		if (index >= current->start_pos && index <= current->end_pos
			&& current->is_quoted)
			return (true);
		current = current->next;
	}
	return (false);
}

/**
 * @brief Checks if the string contains unquoted wildcards '*' or '?'.
 *
 * Iterates through the string and returns true if a wildcard is found
 * outside of quoted token parts.
 *
 * @param str The string to check.
 * @param parts Linked list of token parts with quotation info.
 * @return true if there is at least one unquoted wildcard, false otherwise.
 */
bool	has_unquoted_wildcards(char *str, t_token_part *parts)
{
	int	len;
	int	i;

	len = ft_strlen(str);
	i = 0;
	while (i < len)
	{
		if (str[i] == '*' || str[i] == '?')
		{
			if (!is_in_quoted_part(parts, i))
				return (true);
		}
		i++;
	}
	return (false);
}

/**
 * @brief Concatenates adjacent word tokens into a single string.
 *
 * Combines adjacent tokens without spaces, expanding each and joining
 * them into one argument string for the command.
 *
 * @param current_tokens Pointer to the token list pointer.
 * @param expanded_value Initial expanded value string.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Combined expanded string (must be freed).
 */
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
