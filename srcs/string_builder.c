/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:22:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:31:17 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new string builder object.
 *
 * Allocates and initializes a string builder for efficient string
 * concatenation.
 *
 * @return Pointer to new string builder, or NULL on error.
 */
t_string_builder	*sb_create(void)
{
	t_string_builder	*sb;

	sb = cf_malloc(sizeof(t_string_builder));
	if (!sb)
		return (NULL);
	sb->str = cf_strdup("");
	if (!sb->str)
	{
		cf_free_one(sb);
		return (NULL);
	}
	sb->len = 0;
	sb->capacity = 1;
	return (sb);
}

/**
 * @brief Appends a character to the string builder.
 *
 * Expands the buffer if needed and adds the character.
 *
 * @param sb Pointer to string builder.
 * @param c Character to append.
 */
void	sb_append_char(t_string_builder *sb, char c)
{
	size_t	new_capacity;
	char	*new_str;

	if (sb->len + 1 >= sb->capacity)
	{
		new_capacity = sb->capacity * 2;
		new_str = cf_realloc(sb->str, new_capacity);
		if (!new_str)
			return ;
		sb->str = new_str;
		sb->capacity = new_capacity;
	}
	sb->str[sb->len++] = c;
	sb->str[sb->len] = '\0';
}

/**
 * @brief Appends a string to the string builder.
 *
 * Expands the buffer if needed and adds the string.
 *
 * @param sb Pointer to string builder.
 * @param s String to append.
 */
void	sb_append(t_string_builder *sb, const char *s)
{
	size_t	s_len;
	size_t	new_capacity;
	char	*new_str;

	s_len = ft_strlen(s);
	if (!s)
		return ;
	if (sb->len + s_len >= sb->capacity)
	{
		new_capacity = (sb->len + s_len) * 2;
		new_str = cf_realloc(sb->str, new_capacity);
		if (!new_str)
			return ;
		sb->str = new_str;
		sb->capacity = new_capacity;
	}
	ft_strcpy(sb->str + sb->len, s);
	sb->len += s_len;
}

/**
 * @brief Builds the final string and destroys the string builder.
 *
 * Returns the built string and frees the string builder object.
 *
 * @param sb Pointer to string builder.
 * @return Pointer to final string.
 */
char	*sb_build_and_destroy(t_string_builder *sb)
{
	char	*final_str;

	final_str = sb->str;
	if (!sb)
		return (NULL);
	cf_free_one(sb);
	return (final_str);
}
