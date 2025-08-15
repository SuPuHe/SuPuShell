/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:43:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:25:39 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a string matches a wildcard pattern.
 *
 * Supports '*' and '?' wildcards. Recursively compares the string
 * and pattern, handling multiple consecutive '*' and matching any
 * character for '?'.
 *
 * @param str String to check.
 * @param pattern Wildcard pattern to match.
 * @return 1 if match, 0 otherwise.
 */
int	matches_pattern(const char *str, const char *pattern)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '*')
	{
		while (*pattern == '*')
			pattern++;
		if (*pattern == '\0')
			return (1);
		while (*str)
		{
			if (matches_pattern(str, pattern))
				return (1);
			str++;
		}
		return (matches_pattern(str, pattern));
	}
	if (*str == '\0')
		return (0);
	if (*pattern == *str || *pattern == '?')
		return (matches_pattern(str + 1, pattern + 1));
	return (0);
}

/**
 * @brief Counts files in current directory matching a wildcard pattern.
 *
 * Scans directory and counts non-hidden files that match the pattern
 * using matches_pattern().
 *
 * @param pattern Wildcard pattern to match.
 * @return Number of matching files.
 */
int	count_matches(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	count = 0;
	dir = opendir(".");
	if (!dir)
		return (0);
	entry = readdir(dir);
	while (entry != NULL)
	{
		if (entry->d_name[0] != '.' && matches_pattern(entry->d_name, pattern))
			count++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (count);
}

/**
 * @brief Allocates an array for storing wildcard expansion results.
 *
 * Allocates memory for an array of strings with space for size + 1
 * elements (including NULL terminator).
 *
 * @param size Number of elements to allocate.
 * @return Pointer to allocated array, or NULL on failure.
 */
char	**create_result_array(int size)
{
	char	**result;

	result = cf_malloc(sizeof(char *) * (size + 1));
	if (!result)
		return (NULL);
	return (result);
}

/**
 * @brief Handles patterns without wildcards by returning the pattern itself.
 *
 * Allocates an array with one string, copies the pattern, and sets
 * the second element to NULL.
 *
 * @param pattern Pattern string without wildcards.
 * @return Array containing the pattern, or NULL on allocation failure.
 */
char	**handle_no_wildcards(const char *pattern)
{
	char	**result;

	result = create_result_array(1);
	if (!result)
		return (NULL);
	result[0] = cf_strdup(pattern);
	if (!result[0])
	{
		cf_free_one(result);
		return (NULL);
	}
	result[1] = NULL;
	return (result);
}
