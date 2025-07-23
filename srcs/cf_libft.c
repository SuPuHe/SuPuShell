/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cf_libft.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:35:59 by omizin            #+#    #+#             */
/*   Updated: 2025/07/23 15:17:24 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
