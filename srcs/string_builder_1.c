/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder_1.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:22:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 12:52:25 by omizin           ###   ########.fr       */
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
 * @brief Preprocesses a string before appending to a string builder.
 *
 * If the string is not quoted, spaces are normalized (e.g., trimming
 * or collapsing multiple spaces). If the string is quoted, it is
 * returned unchanged. For quoted strings, the returned pointer is
 * the original input and must not be freed. For unquoted strings,
 * a new normalized copy is allocated and must be freed.
 *
 * @param s Input string to process.
 * @param quotes Whether the string was originally quoted.
 * @return Processed string (allocated if !quotes, borrowed if quotes),
 *			or NULL on error.
 */
static char	*sb_process_string(const char *s, bool quotes)
{
	char	*processed_s;

	if (!s)
		return (NULL);
	if (!quotes)
	{
		processed_s = normalize_spaces(s);
		if (!processed_s)
			return (NULL);
	}
	else
		processed_s = (char *)s;
	return (processed_s);
}

/**
 * @brief Ensures enough capacity in the string builder to append data.
 *
 * Checks if the string builder has enough capacity for the given length.
 * If not, reallocates the buffer to a larger size (doubling the required
 * capacity). Updates the string builder's internal buffer pointer and
 * capacity on success.
 *
 * @param sb Pointer to the string builder.
 * @param s_len Length of the string to be appended.
 * @return true if capacity is sufficient or reallocation succeeded,
 *			false if reallocation failed.
 */
static bool	sb_expand_capacity(t_string_builder *sb, size_t s_len)
{
	size_t	new_capacity;
	char	*new_str;

	if (sb->len + s_len + 1 < sb->capacity)
		return (true);
	new_capacity = (sb->len + s_len + 1) * 2;
	new_str = cf_realloc(sb->str, new_capacity);
	if (!new_str)
		return (false);
	sb->str = new_str;
	sb->capacity = new_capacity;
	return (true);
}

/**
 * @brief Appends a string to a string builder, handling quotes and spaces.
 *
 * Preprocesses the string (normalizes spaces if unquoted), ensures
 * sufficient capacity in the builder, and appends the processed content
 * to the builder's buffer. Updates the builder's length and maintains
 * null-termination. Frees temporary processed strings when appropriate.
 *
 * @param sb Pointer to the string builder.
 * @param s String to append.
 * @param quotes Whether the string was quoted (affects preprocessing
 *				and memory management).
 */
void	sb_append(t_string_builder *sb, const char *s, bool quotes)
{
	size_t	s_len;
	char	*processed_s;

	processed_s = sb_process_string(s, quotes);
	if (!processed_s)
		return ;
	s_len = ft_strlen(processed_s);
	if (!sb_expand_capacity(sb, s_len))
	{
		if (!quotes)
			cf_free_one(processed_s);
		return ;
	}
	ft_strcpy(sb->str + sb->len, processed_s);
	sb->len += s_len;
	sb->str[sb->len] = '\0';
	if (!quotes)
		cf_free_one(processed_s);
}
