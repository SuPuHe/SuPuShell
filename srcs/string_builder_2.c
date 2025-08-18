/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:13:43 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 12:53:21 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Skips leading spaces in a string (keeping one if present).
 *
 * Advances through the input string until the first non-space character.
 * If at least one space was skipped, steps back one index to preserve
 * a single leading space in the normalized result.
 *
 * @param s Input string.
 * @return Index at which normalization should begin.
 */
static size_t	skip_leading_spaces(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] && ft_isspace(s[i]))
		i++;
	if (i != 0)
		i--;
	return (i);
}

/**
 * @brief Copies characters from input string to output, normalizing spaces.
 *
 * Consecutive whitespace characters in the input are collapsed into a
 * single space in the output. Non-space characters are copied directly.
 *
 * @param s Input string.
 * @param dst Destination buffer.
 * @param i Starting index in the input string.
 * @return Number of characters written to the destination.
 */
static size_t	copy_normalized(const char *s, char *dst, size_t i)
{
	size_t	j;

	j = 0;
	while (s[i])
	{
		if (!ft_isspace(s[i]))
			dst[j++] = s[i++];
		else
		{
			while (s[i] && ft_isspace(s[i]))
				i++;
			dst[j++] = ' ';
		}
	}
	return (j);
}

/**
 * @brief Trims a trailing space from the normalized string.
 *
 * If the last written character is a space, it is removed and the
 * string is properly null-terminated.
 *
 * @param dst Destination string being normalized.
 * @param j Current length of the destination string.
 * @return New length of the destination string after trimming.
 */
static size_t	trim_trailing_space(char *dst, size_t j)
{
	size_t	orig;

	orig = j;
	if (j > 0 && dst[j - 1] == ' ')
		j--;
	if (j != orig)
		j++;
	dst[j] = '\0';
	return (j);
}

/**
 * @brief Normalizes spaces in a string.
 *
 * Produces a new string where:
 * - Leading spaces are trimmed to at most one.
 * - Multiple consecutive spaces are collapsed into one.
 * - A trailing space (if present) is trimmed.
 *
 * Allocates a new string with centralized memory management
 * (`cf_malloc`) and returns it.
 *
 * @param s Input string to normalize.
 * @return Newly allocated normalized string, or NULL on error.
 */
char	*normalize_spaces(const char *s)
{
	size_t	i;
	size_t	j;
	size_t	len;
	char	*new_str;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	new_str = cf_malloc(len + 1);
	if (!new_str)
		return (NULL);
	i = skip_leading_spaces(s);
	j = copy_normalized(s, new_str, i);
	trim_trailing_space(new_str, j);
	return (new_str);
}

/**
 * @brief Builds the final string and destroys the string builder.
 *
 * Returns the built string and frees the string builder object.
 *
 * @param sb Pointer to string builder.
 * @return Pointer to final string.
 */
char	*sb_build_and_destroy(t_string_builder *sb)
{
	char	*final_str;

	final_str = sb->str;
	if (!sb)
		return (NULL);
	cf_free_one(sb);
	return (final_str);
}
