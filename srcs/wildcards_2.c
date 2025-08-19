/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:43:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 18:02:51 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a string contains unquoted wildcards.
 *
 * Iterates through the string and checks if it contains '*' or '?' characters
 * that are not inside quoted token parts.
 *
 * @param str Input string to check.
 * @param parts Linked list of token parts.
 * @return true if unquoted wildcards are found, false otherwise.
 */
static void	handle_wildcard_results(t_input *input, char *expanded_value,
	char *pattern, t_token_part *parts)
{
	char	**wildcards;
	int		i;

	wildcards = expand_wildcards_with_escape(pattern);
	if (wildcards && wildcards[0])
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
	cf_free_one(pattern);
	free_token_parts(parts);
}

/**
 * @brief Handles simple expansion and prepares wildcard patterns if needed.
 *
 * This function decides how to handle an expanded value:
 * - If there are no token parts, the value is directly added as an argument.
 * - If the value does not contain unquoted wildcards, it is appended directly.
 * - If unquoted wildcards are present, it escapes quoted parts and produces
 *   a glob pattern string in @p pattern for later expansion.
 *
 * @param input Pointer to the input structure holding parsed arguments.
 * @param expanded_value Expanded value string (ownership is transferred).
 * @param parts List of token parts describing quotes and special characters.
 * @param pattern Output pointer for the prepared glob pattern (if any).
 * @return true if the argument was fully processed and appended to input->args,
 *			false if a glob pattern was prepared and requires further expansion.
 */
static bool	handle_simple_expansion(t_input *input, char *expanded_value,
	t_token_part *parts, char **pattern)
{
	if (!parts)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		return (true);
	}
	if (!has_unquoted_wildcards(expanded_value, parts))
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return (true);
	}
	*pattern = escape_wildcards_in_quotes(expanded_value, parts);
	if (!*pattern)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return (true);
	}
	return (false);
}

/**
 * @brief Handles wildcard (glob) expansion for tokens.
 *
 * Builds a globbing pattern from the expanded value and token parts,
 * then expands it into matching filenames. If no matches are found,
 * the original expanded value is preserved.
 *
 * @param input Pointer to the input structure.
 * @param expanded_value Expanded value string.
 * @param tokens_start Start of the token list.
 * @param tokens_end End of the token list.
 */
void	handle_glob_expansion(t_input *input,
	char *expanded_value, t_list *tokens_start, t_list *tokens_end)
{
	t_token_part	*parts;
	char			*pattern;

	parts = create_token_parts_list(tokens_start, tokens_end);
	if (handle_simple_expansion(input, expanded_value, parts, &pattern))
		return ;
	handle_wildcard_results(input, expanded_value, pattern, parts);
}
