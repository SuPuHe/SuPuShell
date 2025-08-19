/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 16:45:22 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 16:59:57 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Matches a single character in pattern with escape.
 *
 * Handles '\' escape in pattern, comparing the next character literally.
 *
 * @param str Current position in string.
 * @param pattern Current position in pattern (points to '\').
 * @return 1 if matched, 0 otherwise.
 */
static int	match_escape(const char *str, const char *pattern)
{
	if (*str != *(pattern + 1))
		return (0);
	return (matches_pattern_with_escape(str + 1, pattern + 2));
}

/**
 * @brief Matches a string against a '*' wildcard in the pattern.
 *
 * Skips consecutive '*' and recursively tries to match the remaining string
 * with the rest of the pattern.
 *
 * @param str Current position in string.
 * @param pattern Current position in pattern (points to '*').
 * @return 1 if matched, 0 otherwise.
 */
static int	match_star(const char *str, const char *pattern)
{
	while (*pattern == '*')
		pattern++;
	if (*pattern == '\0')
		return (1);
	while (*str)
	{
		if (matches_pattern_with_escape(str, pattern))
			return (1);
		str++;
	}
	return (matches_pattern_with_escape(str, pattern));
}

/**
 * @brief Matches a string against a pattern with wildcards and escapes.
 *
 * Supports:
 * - '*' matches any sequence of characters
 * - '?' matches any single character
 * - '\' escapes the next character in the pattern
 *
 * @param str Input string to match.
 * @param pattern Pattern string containing wildcards and escapes.
 * @return 1 if string matches pattern, 0 otherwise.
 */
int	matches_pattern_with_escape(const char *str, const char *pattern)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '\\' && *(pattern + 1) != '\0')
		return (match_escape(str, pattern));
	if (*pattern == '*')
		return (match_star(str, pattern));
	if (*str == '\0')
		return (0);
	if (*pattern == *str || *pattern == '?')
		return (matches_pattern_with_escape(str + 1, pattern + 1));
	return (0);
}

/**
 * @brief Expands a pattern with wildcards to matching filenames.
 *
 * If no wildcards or no matches, returns an array with the original pattern.
 *
 * @param pattern Pattern string.
 * @return Null-terminated array of matching filenames.
 */
char	**expand_wildcards_with_escape(const char *pattern)
{
	int	count;

	if (!ft_strchr(pattern, '*') && !ft_strchr(pattern, '?'))
		return (return_single_pattern(pattern));
	count = count_matching_files(pattern);
	if (count == 0)
		return (return_single_pattern(pattern));
	return (collect_matching_files(pattern, count));
}

/**
 * @brief Handles wildcard expansion for word tokens.
 *
 * Expands wildcards in the word and adds all matches to command arguments.
 * If no matches, adds the original word. Frees memory after use.
 *
 * @param input Pointer to the input structure.
 * @param current_tok Pointer to the current token.
 * @param expanded_value Expanded word string.
 */
void	handle_wildcard_expansion(t_input *input,
			t_token *current_tok, char *expanded_value)
{
	char	**wildcards;
	int		i;

	if (current_tok->type == TOKEN_WORD
		&& (ft_strchr(expanded_value, '*') || ft_strchr(expanded_value, '?')))
	{
		wildcards = expand_wildcards(expanded_value);
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
	}
	else
		handle_regular_word(input, expanded_value);
}
