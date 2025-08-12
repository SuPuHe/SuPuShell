/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:22:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/12 16:45:47 by omizin           ###   ########.fr       */
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
static char *normalize_spaces(const char *s)
{
	size_t	i;
	size_t	j;
	size_t	len;
	char	*new_str;

	if (!s)
		return NULL;

	len = ft_strlen(s);
	new_str = cf_malloc(len + 1);
	if (!new_str)
		return NULL;
	i = 0;
	j = 0;
	while (s[i] && ft_isspace(s[i]))
		i++;
	if (i != 0)
		i--;
	while (s[i])
	{
		if (!ft_isspace(s[i]))
			new_str[j++] = s[i++];
		else
		{
			while (s[i] && ft_isspace(s[i]))
				i++;
			new_str[j++] = ' ';
		}
	}
	i = j;
	if (j > 0 && new_str[j - 1] == ' ')
		j--;
	if (j != i)
		j++;
	new_str[j] = '\0';
	return new_str;
}

/**
 * @brief Appends a string to the string builder, normalizing spaces.
 *
 * This function first normalizes the input string 's' by replacing
 * multiple spaces with a single space and removing leading/trailing spaces.
 * It then appends this cleaned string to the string builder.
 *
 * @param sb Pointer to the string builder.
 * @param s String to be appended.
 */
void sb_append(t_string_builder *sb, const char *s, bool quotes)
{
	size_t	s_len;
	size_t	new_capacity;
	char	*new_str;
	char	*processed_s; // Use a new pointer to hold the string being processed

	if (!s)
		return;

	// Conditionally normalize spaces only if 'quotes' is false
	if (!quotes)
	{
		processed_s = normalize_spaces(s); // Your helper function
		if (!processed_s)
			return;
	}
	else
	{
		// If 'quotes' is true, use the original string
		processed_s = (char *)s;
	}

	s_len = ft_strlen(processed_s);
	if (sb->len + s_len + 1 >= sb->capacity)
	{
		new_capacity = (sb->len + s_len + 1) * 2;
		new_str = cf_realloc(sb->str, new_capacity);
		if (!new_str)
		{
			if (!quotes)
				cf_free_one(processed_s);
			return;
		}
		sb->str = new_str;
		sb->capacity = new_capacity;
	}
	ft_strcpy(sb->str + sb->len, processed_s);
	sb->len += s_len;
	sb->str[sb->len] = '\0';

	// Free the temporary normalized string only if it was created
	if (!quotes)
		cf_free_one(processed_s);
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
