/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cf.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:40:47 by omizin            #+#    #+#             */
/*   Updated: 2025/07/28 15:48:55 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CF_H
# define CF_H

# include <stdlib.h>
# include "libft.h"

typedef struct s_cf_node
{
	void				*ptr;
	struct	s_cf_node	*next;
}	t_cf_node;

t_cf_node	**get_cf_head(void);
void		*cf_malloc(size_t size);
char		*cf_strdup(const char *s);
void		cf_free_one(void *ptr);
void		cf_free_all(void);
void		*cf_realloc(void *ptr, size_t new_size);
char		*cf_substr(const char *s, unsigned int start, size_t len);
t_list		*cf_lstnew(void *content);
#endif
