/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:38:55 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 18:31:14 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Compares two strings lexicographically.
 *
 * Returns the difference between the first pair of differing characters,
 * or zero if the strings are equal.
 *
 * @param s1 First string.
 * @param s2 Second string.
 * @return Difference between first differing characters, or zero.
 */
int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

/**
 * @brief Copies a string to the destination buffer.
 *
 * Copies src to dest, including the null terminator.
 *
 * @param dest Destination buffer.
 * @param src Source string.
 * @return Pointer to destination buffer.
 */
char	*ft_strcpy(char *dest, const char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

/**
 * @brief Concatenates two strings.
 *
 * Appends src to the end of dest, including the null terminator.
 *
 * @param dest Destination buffer.
 * @param src Source string.
 * @return Pointer to destination buffer.
 */
char	*ft_strcat(char *dest, const char *src)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (dest[i])
		i++;
	while (src[j])
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i + j] = '\0';
	return (dest);
}

/**
 * @brief Duplicates a substring between two pointers.
 *
 * Allocates and copies the substring from start to end (exclusive).
 *
 * @param start Pointer to start of substring.
 * @param end Pointer to end of substring.
 * @return Pointer to allocated substring, or NULL on error.
 */
char	*substr_dup(const char *start, const char *end)
{
	size_t	len;
	char	*s;

	len = end - start;
	s = cf_malloc(len + 1);
	if (!s)
		return (NULL);
	ft_memcpy(s, start, len);
	s[len] = '\0';
	return (s);
}

/**
 * @brief Joins two strings and frees the first.
 *
 * Allocates and concatenates s1 and s2, frees s1, and returns the result.
 *
 * @param s1 First string (will be freed).
 * @param s2 Second string.
 * @return Pointer to joined string, or NULL on error.
 */
char	*ft_strjoin_free(char *s1, const char *s2)
{
	char	*word;
	size_t	len;

	if (!s1 || !s2)
		return (NULL);
	len = ft_strlen(s1) + ft_strlen(s2);
	word = cf_malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	ft_memcpy(word, s1, ft_strlen(s1));
	ft_memcpy(word + ft_strlen(s1), s2, ft_strlen(s2));
	word[len] = '\0';
	cf_free_one(s1);
	return (word);
}
