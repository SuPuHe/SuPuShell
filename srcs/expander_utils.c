/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:21:18 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/12 15:51:15 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands $0 variable to the shell name.
 *
 * If the variable is $0, appends "Billyshell" to the string builder.
 * Advances the index pointer.
 *
 * @param sb Pointer to string builder.
 * @param str Input string.
 * @param i Pointer to current index.
 */
void	expand_digit(t_string_builder *sb, const char *str, int *i)
{
	if (str[*i] == '0')
		sb_append(sb, "Billyshell", true);
	(*i)++;
}

/**
 * @brief Handles expansion of unknown or unsupported variables.
 *
 * Appends a literal '$' character to the string builder.
 *
 * @param sb Pointer to string builder.
 */
void	expand_other(t_string_builder *sb)
{
	sb_append_char(sb, '$');
}

/**
 * @brief Expands an environment variable in the input string.
 *
 * Extracts the variable name, looks up its value in the environment,
 * and appends the value to the string builder if found.
 * Advances the index pointer.
 *
 * @param sb Pointer to string builder.
 * @param str Input string.
 * @param env Pointer to environment structure.
 * @param i Pointer to current index.
 */
void	expand_env_var(t_string_builder *sb,
	const char *str, t_env *env, t_expand_ctx *ctx)
{
	int		var_start;
	char	*var_name;
	char	*var_value;

	var_start = *ctx->i;
	while (str[*ctx->i] && (ft_isalnum(str[*ctx->i]) || str[*ctx->i] == '_'))
		(*ctx->i)++;
	var_name = cf_substr(str, var_start, *ctx->i - var_start);
	var_value = get_env_value(env, var_name);
	if (var_value)
		sb_append(sb, var_value, ctx->quotes);
	cf_free_one(var_name);
}

/**
 * @brief Checks if a character is a shell operator.
 *
 * Returns true if the character is one of: | & ( ) < >
 *
 * @param c Character to check.
 * @return true if operator, false otherwise.
 */
bool	is_operator(char c)
{
	return (c == '|' || c == '&' || c == '('
		|| c == ')' || c == '<' || c == '>');
}
