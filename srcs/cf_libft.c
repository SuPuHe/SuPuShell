/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cf_libft.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:35:59 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 18:21:31 by vpushkar         ###   ########.fr       */
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
