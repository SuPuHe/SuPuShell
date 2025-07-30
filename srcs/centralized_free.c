/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   centralized_free.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:39:15 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 18:20:48 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Returns a pointer to the head of the centralized free list.
 *
 * Provides access to the static pointer used for tracking all allocated
 * memory blocks for centralized freeing.
 *
 * @return Pointer to the head of the free list.
 */
t_cf_node	**get_cf_head(void)
{
	static t_cf_node	*cf_head = NULL;

	return (&cf_head);
}

/**
 * @brief Frees a single tracked memory block.
 *
 * Searches the centralized free list for the given pointer, removes it
 * from the list, and frees its memory.
 *
 * @param ptr Pointer to memory block to free.
 */
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

/**
 * @brief Frees all tracked memory blocks.
 *
 * Iterates through the centralized free list, freeing all memory blocks
 * and nodes, then resets the list head to NULL.
 */
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
