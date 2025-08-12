/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:19:19 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/12 16:43:03 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands the special variable $? to the last exit status value.
 *
 * Converts the shell's last exit status to a string and appends it
 * to the string builder. Advances the index pointer.
 *
 * @param sb Pointer to the string builder.
 * @param shell Pointer to the shell structure.
 * @param i Pointer to the current index in the string.
 */
void	expand_status(t_string_builder *sb, t_shell *shell, int *i)
{
	char	*status_str;

	status_str = ft_itoa(shell->last_exit_status);
	sb_append(sb, status_str, false);
	free(status_str);
	(*i)++;
}

/**
 * @brief Expands all shell variables in the input string.
 *
 * Iterates through the string, expanding variables and appending
 * their values to the string builder.
 *
 * @param str Input string to expand.
 * @param ctx Pointer to the expansion context.
 */
void	expand_loop(const char *str, t_expand_ctx *ctx, bool quotes)
{
	int	*i;

	i = ctx->i;
	while (str[*i])
	{
		if (str[*i] == '$')
		{
			(*i)++;
			expand_hub(ctx, quotes);
		}
		else
		{
			sb_append_char(ctx->sb, str[*i]);
			(*i)++;
		}
	}
}

/**
 * @brief Dispatches variable expansion based on the variable type.
 *
 * Determines the type of variable (status, digit, env, other)
 * and calls the appropriate expansion function.
 *
 * @param ctx Pointer to the expansion context.
 */
void	expand_hub(t_expand_ctx *ctx, bool quotes)
{
	char	quote_char;
	int		literal_content_start;
	char	*literal_content;

	ctx->quotes = quotes;
	if (ctx->str[*ctx->i] == '?')
		expand_status(ctx->sb, ctx->shell, ctx->i);
	else if (ft_isdigit(ctx->str[*ctx->i]))
		expand_digit(ctx->sb, ctx->str, ctx->i);
	else if (ctx->str[*ctx->i] == '_' || ft_isalpha(ctx->str[*ctx->i]))
		expand_env_var(ctx->sb, ctx->str, ctx->env, ctx);
	else if (ctx->str[*ctx->i] == '\"' || ctx->str[*ctx->i] == '\'')
	{
		quote_char = ctx->str[*ctx->i];
		(*ctx->i)++;
		literal_content_start = *ctx->i;
		while (ctx->str[*ctx->i] && ctx->str[*ctx->i] != quote_char)
			(*ctx->i)++;
		literal_content = cf_substr(ctx->str, literal_content_start, *ctx->i - literal_content_start);
		sb_append(ctx->sb, literal_content, true);
		cf_free_one(literal_content);
		if (ctx->str[*ctx->i] == quote_char)
			(*ctx->i)++;
	}
	else
		expand_other(ctx->sb);
}

/**
 * @brief Expands all shell variables in a string and returns the result.
 *
 * Uses a string builder to construct the expanded string, replacing
 * variables with their values from the environment and shell.
 *
 * @param str Input string to expand.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Expanded string (must be freed by the caller).
 */
char	*expand_string_variables(const char *str, t_env *env, t_shell *shell, bool quotes)
{
	t_string_builder	*sb;
	int					i;
	t_expand_ctx		ctx;

	sb = sb_create();
	if (!sb)
		return (NULL);
	i = 0;
	ctx.sb = sb;
	ctx.str = str;
	ctx.env = env;
	ctx.shell = shell;
	ctx.i = &i;
	expand_loop(str, &ctx, quotes);
	return (sb_build_and_destroy(sb));
}

/**
 * @brief Expands the value of a token based on its type.
 *
 * Handles single-quoted, double-quoted, and regular word tokens,
 * expanding them as necessary using the environment and shell context.
 *
 * @param token Pointer to the token to expand.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Expanded string (must be freed by the caller).
 */

char	*expand_token_value(t_token *token, t_env *env, t_shell *shell)
{
	if (!token || !token->value)
		return (cf_strdup(""));
	if (token->type == TOKEN_SINGLE_QUOTE_WORD)
	{
		return (cf_strdup(token->value));
	}
	else if (token->type == TOKEN_DOUBLE_QUOTE_WORD)
	{
		return (expand_string_variables(token->value, env, shell, true));
	}
	else if (token->type == TOKEN_WORD)
	{
		return (expand_string_variables(token->value, env, shell, false));
	}
	return (cf_strdup(""));
}
