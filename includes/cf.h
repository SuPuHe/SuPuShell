/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cf.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 13:40:47 by omizin            #+#    #+#             */
/*   Updated: 2025/08/11 12:01:19 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CF_H
# define CF_H

# include "types.h"

t_cf_node	**get_cf_head(void);
void		*cf_malloc(size_t size);
char		*cf_strdup(const char *s);
void		cf_free_one(void *ptr);
void		cf_free_all(void);
void		*cf_realloc(void *ptr, size_t new_size);
char		*cf_substr(const char *s, unsigned int start, size_t len);
t_list		*cf_lstnew(void *content);
char		*cf_strjoin(char const *s1, char const *s2);
char		*cf_itoa(int n);
#endif
