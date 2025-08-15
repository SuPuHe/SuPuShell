/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder_second_part.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:13:43 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 15:30:20 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	skip_leading_spaces(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] && ft_isspace(s[i]))
		i++;
	if (i != 0)
		i--;
	return (i);
}

static size_t	copy_normalized(const char *s, char *dst, size_t i)
{
	size_t	j;

	j = 0;
	while (s[i])
	{
		if (!ft_isspace(s[i]))
			dst[j++] = s[i++];
		else
		{
			while (s[i] && ft_isspace(s[i]))
				i++;
			dst[j++] = ' ';
		}
	}
	return (j);
}

static size_t	trim_trailing_space(char *dst, size_t j)
{
	size_t	orig;

	orig = j;
	if (j > 0 && dst[j - 1] == ' ')
		j--;
	if (j != orig)
		j++;
	dst[j] = '\0';
	return (j);
}

char	*normalize_spaces(const char *s)
{
	size_t	i;
	size_t	j;
	size_t	len;
	char	*new_str;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	new_str = cf_malloc(len + 1);
	if (!new_str)
		return (NULL);
	i = skip_leading_spaces(s);
	j = copy_normalized(s, new_str, i);
	trim_trailing_space(new_str, j);
	return (new_str);
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
