/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:21:18 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:40:38 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//part of expand_string_variables
void	expand_digit(t_string_builder *sb, const char *str, int *i)
{
	if (str[*i] == '0')
		sb_append(sb, "SuPuShell");
	(*i)++;
}

//part of expand_string_variables
void	expand_other(t_string_builder *sb)
{
	sb_append_char(sb, '$');
}

//part of expand_string_variables
void	expand_env_var(t_string_builder *sb,
	const char *str, t_env *env, int *i)
{
	int		var_start;
	char	*var_name;
	char	*var_value;

	var_start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	var_name = cf_substr(str, var_start, *i - var_start);
	var_value = get_env_value(env, var_name);
	if (var_value)
		sb_append(sb, var_value);
	cf_free_one(var_name);
}

bool	is_operator(char c)
{
	return (c == '|' || c == '&' || c == '('
		|| c == ')' || c == '<' || c == '>');
}
