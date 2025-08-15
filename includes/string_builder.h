/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:26:21 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:27:13 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_BUILDER_H
# define STRING_BUILDER_H

# include "types.h"

// string_builder_1.c
t_string_builder	*sb_create(void);
void				sb_append_char(t_string_builder *sb, char c);
void				sb_append(t_string_builder *sb, const char *s, bool quotes);

// string_builder_2.c
char				*normalize_spaces(const char *s);
char				*sb_build_and_destroy(t_string_builder *sb);
#endif
