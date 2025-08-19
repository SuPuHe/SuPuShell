/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:43:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 17:38:54 by vpushkar         ###   ########.fr       */
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
	char *bash_pattern, t_token_part *parts)
{
	char	**wildcards;
	int		i;

	wildcards = expand_wildcards_with_escape(bash_pattern);
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
	cf_free_one(bash_pattern);
	free_token_parts(parts);
}

/**
 * @brief Handles wildcard expansion for Bash-compatible patterns.
 *
 * If the expanded value contains unquoted wildcards,
 * it creates a Bash-compatible pattern and expands it.
 * Otherwise, it appends the expanded value directly.
 *
 * @param input Pointer to the input structure.
 * @param expanded_value Expanded value string.
 * @param tokens_start Start of the token list.
 * @param tokens_end End of the token list.
 */
static bool	handle_simple_expansion(t_input *input, char *expanded_value,
	t_token_part *parts, char **bash_pattern)
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
	*bash_pattern = create_bash_compatible_pattern(expanded_value, parts);
	if (!*bash_pattern)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return (true);
	}
	return (false);
}

/**
 * @brief Handles Bash-compatible wildcard expansion.
 *
 * Creates a Bash-compatible pattern from the expanded value and token parts,
 * then expands it to matching filenames. If no matches, appends the original
 * expanded value.
 *
 * @param input Pointer to the input structure.
 * @param expanded_value Expanded value string.
 * @param tokens_start Start of the token list.
 * @param tokens_end End of the token list.
 */
void	handle_bash_compatible_wildcard_expansion(t_input *input,
	char *expanded_value, t_list *tokens_start, t_list *tokens_end)
{
	t_token_part	*parts;
	char			*bash_pattern;

	parts = create_token_parts_list(tokens_start, tokens_end);
	if (handle_simple_expansion(input, expanded_value, parts, &bash_pattern))
		return ;
	handle_wildcard_results(input, expanded_value, bash_pattern, parts);
}
