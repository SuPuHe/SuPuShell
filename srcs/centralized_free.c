/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   centralized_free.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:39:15 by omizin            #+#    #+#             */
/*   Updated: 2025/07/28 15:48:23 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cf_node	**get_cf_head(void)
{
	static t_cf_node	*cf_head = NULL;

	return (&cf_head);
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


