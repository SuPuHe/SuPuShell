/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:26:21 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/12 16:44:12 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_BUILDER_H
# define STRING_BUILDER_H

# include "types.h"

t_string_builder	*sb_create(void);
void				sb_append_char(t_string_builder *sb, char c);
void				sb_append(t_string_builder *sb, const char *s, bool quotes);
char				*sb_build_and_destroy(t_string_builder *sb);

#endif
