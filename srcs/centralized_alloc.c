/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   centralized_alloc.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:42:01 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:20:31 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Allocates memory and tracks it for centralized freeing.
 *
 * Allocates a memory block and registers it in the centralized free list.
 * Returns pointer to allocated memory, or NULL on failure.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to allocated memory, or NULL on error.
 */
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

/**
 * @brief Duplicates a string and tracks it for centralized freeing.
 *
 * Allocates a copy of the input string and registers it in the centralized
 * free list. Returns pointer to duplicated string, or NULL on failure.
 *
 * @param s String to duplicate.
 * @return Pointer to duplicated string, or NULL on error.
 */
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

/**
 * @brief Reallocates a tracked memory block to a new size.
 *
 * Allocates a new block, copies data from the old block, frees the old block,
 * and registers the new block in the centralized free list.
 * Returns pointer to reallocated memory, or NULL on error.
 *
 * @param ptr Pointer to memory block to reallocate.
 * @param new_size New size in bytes.
 * @return Pointer to reallocated memory, or NULL on error.
 */
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
