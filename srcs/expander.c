/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:19:19 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:46:48 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//part of expand_string_variables
void	expand_status(t_string_builder *sb, t_shell *shell, int *i)
{
	char	*status_str;

	status_str = ft_itoa(shell->last_exit_status);
	sb_append(sb, status_str);
	free(status_str);
	(*i)++;
}

//part of expand_string_variables
void	expand_loop(const char *str, t_expand_ctx *ctx)
{
	int	*i;

	i = ctx->i;
	while (str[*i])
	{
		if (str[*i] == '$')
		{
			(*i)++;
			expand_hub(ctx);
		}
		else
		{
			sb_append_char(ctx->sb, str[*i]);
			(*i)++;
		}
	}
}

//part of expand_string_variables
void	expand_hub(t_expand_ctx *ctx)
{
	if (ctx->str[*ctx->i] == '?')
		expand_status(ctx->sb, ctx->shell, ctx->i);
	else if (ft_isdigit(ctx->str[*ctx->i]))
		expand_digit(ctx->sb, ctx->str, ctx->i);
	else if (ctx->str[*ctx->i] == '_' || ft_isalpha(ctx->str[*ctx->i]))
		expand_env_var(ctx->sb, ctx->str, ctx->env, ctx->i);
	else
		expand_other(ctx->sb);
}

char	*expand_string_variables(const char *str, t_env *env, t_shell *shell)
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
	expand_loop(str, &ctx);
	return (sb_build_and_destroy(sb));
}

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
		return (expand_string_variables(token->value, env, shell));
	}
	else if (token->type == TOKEN_WORD)
	{
		return (expand_string_variables(token->value, env, shell));
	}
	return (cf_strdup(""));
}
