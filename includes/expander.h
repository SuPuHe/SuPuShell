/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:30:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 17:33:03 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

# include "types.h"
# include <stdbool.h>

//expander_utils.c
void	expand_digit(t_string_builder *sb, const char *str, int *i);
void	expand_other(t_string_builder *sb);
void	expand_env_var(t_string_builder *sb,
			const char *str, t_env *env, int *i);
bool	is_operator(char c);

//expander.c
void	expand_status(t_string_builder *sb, t_shell *shell, int *i);
void	expand_loop(const char *str, t_expand_ctx *ctx);
void	expand_hub(t_expand_ctx *ctx);
char	*expand_string_variables(const char *str, t_env *env, t_shell *shell);
char	*expand_token_value(t_token *token, t_env *env, t_shell *shell);

#endif
