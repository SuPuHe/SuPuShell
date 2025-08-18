/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cf_libft.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:35:59 by omizin            #+#    #+#             */
/*   Updated: 2025/08/18 12:45:43 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Returns a substring from a given string, tracked for freeing.
 *
 * Allocates and returns a substring starting at 'start' of length 'len'.
 * Uses centralized allocation for memory management.
 *
 * @param s Source string.
 * @param start Starting index for substring.
 * @param len Length of substring.
 * @return Pointer to allocated substring, or NULL on error.
 */
char	*cf_substr(const char *s, unsigned int start, size_t len)
{
	size_t	string_len;
	size_t	i;
	char	*word;

	if (!s)
		return (NULL);
	i = 0;
	string_len = ft_strlen(s);
	if (start >= string_len)
		return (cf_malloc(1));
	if (len > string_len - start)
		len = string_len - start;
	word = cf_malloc(len + 1);
	if (!word)
		return (NULL);
	while (i < len)
	{
		word[i] = s[start + i];
		i++;
	}
	word[len] = '\0';
	return (word);
}

/**
 * @brief Creates a new list node, tracked for centralized freeing.
 *
 * Allocates a new t_list node, sets its content and next pointer to NULL.
 * Uses centralized allocation for memory management.
 *
 * @param content Pointer to node content.
 * @return Pointer to new list node, or NULL on error.
 */
t_list	*cf_lstnew(void *content)
{
	t_list	*node;

	node = cf_malloc(sizeof(t_list));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}

/**
 * @brief Concatenates two strings into a newly allocated string.
 *
 * Allocates and returns a new string consisting of 's1' followed by 's2'.
 * Uses centralized allocation through `cf_malloc` for memory tracking.
 *
 * @param s1 First string.
 * @param s2 Second string.
 * @return Pointer to the newly allocated concatenated string, or NULL on error.
 */
char	*cf_strjoin(char const *s1, char const *s2)
{
	char	*word;
	size_t	len;

	len = ft_strlen(s1) + ft_strlen(s2);
	word = (char *)cf_malloc(sizeof(char) * (len + 1));
	if (!word)
		return ((void *)0);
	ft_memcpy(word, s1, ft_strlen(s1));
	ft_memcpy(word + ft_strlen(s1), s2, ft_strlen(s2));
	word[len] = '\0';
	return (word);
}

static size_t	itoa_len(int n)
{
	size_t	len;

	len = 0;
	if (n <= 0)
		len = 1;
	while (n)
	{
		n = n / 10;
		len++;
	}
	return (len);
}

/**
 * @brief Converts an integer to a newly allocated string.
 *
 * Allocates and returns a string representation of the integer 'n'.
 * Handles negative numbers by prefixing with '-' and uses centralized
 * allocation through `cf_malloc` for memory tracking.
 *
 * @param n Integer to convert.
 * @return Pointer to the newly allocated string containing the number,
 *			or NULL on error.
 */
char	*cf_itoa(int n)
{
	char	*str;
	long	num;
	size_t	len;

	num = n;
	len = itoa_len(n);
	str = cf_malloc(sizeof(char) * (len + 1));
	if (!str)
		return ((void *)0);
	str[len] = '\0';
	if (num < 0)
		num = -num;
	while (len--)
	{
		str[len] = (num % 10) + '0';
		num = num / 10;
	}
	if (n < 0)
		str[0] = '-';
	return (str);
}
