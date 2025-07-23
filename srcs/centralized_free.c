/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   centralized_free.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:39:15 by omizin            #+#    #+#             */
/*   Updated: 2025/07/23 15:04:59 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_cf_node	**get_cf_head(void)
{
	static t_cf_node	*cf_head = NULL;

	return (&cf_head);
}

void	*cf_malloc(size_t size)
{
	void		*ptr;
	t_cf_node	*node;

	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	node = malloc(sizeof(t_cf_node));
	if (!node)
	{
		free(ptr);
		return (NULL);
	}
	node->ptr = ptr;
	node->next = *get_cf_head();
	*get_cf_head() = node;
	return (ptr);
}

char	*cf_strdup(const char *s)
{
	char		*dup;
	t_cf_node	*node;

	dup = ft_strdup(s);
	if (!dup)
		return (NULL);
	node = malloc(sizeof(t_cf_node));
	if (!node)
	{
		free(dup);
		return (NULL);
	}
	node->ptr = dup;
	node->next = *get_cf_head();
	*get_cf_head() = node;
	return (dup);
}

void	cf_add(void *ptr)
{
	t_cf_node	*node;

	if (!ptr)
		return ;
	node = malloc(sizeof(t_cf_node));
	if (!node)
		return ;
	node->ptr = ptr;
	node->next = *get_cf_head();
	*get_cf_head() = node;
}

void	cf_free_one(void *ptr)
{
	t_cf_node	**cur;
	t_cf_node	*to_free;

	cur = get_cf_head();
	while (*cur)
	{
		if ((*cur)->ptr == ptr)
		{
			to_free = *cur;
			*cur = (*cur)->next;
			free(to_free->ptr);
			free(to_free);
			return ;
		}
		cur = &(*cur)->next;
	}
}

void	cf_free_all(void)
{
	t_cf_node	*cur;
	t_cf_node	*next;

	cur = *get_cf_head();
	while (cur)
	{
		next = cur->next;
		free(cur->ptr);
		free(cur);
		cur = next;
	}
	*get_cf_head() = NULL;
}

void	*cf_realloc(void *ptr, size_t new_size)
{
	void	*new_ptr;
	size_t	old_len;

	if (!ptr)
		return (cf_malloc(new_size));
	if (new_size == 0)
	{
		cf_free_one(ptr);
		return (NULL);
	}
	new_ptr = cf_malloc(new_size);
	if (!new_ptr)
		return (NULL);
	old_len = ft_strlen((char *)ptr) + 1;
	if (old_len > new_size)
		old_len = new_size;
	ft_memcpy(new_ptr, ptr, old_len);
	cf_free_one(ptr);
	return (new_ptr);
}
