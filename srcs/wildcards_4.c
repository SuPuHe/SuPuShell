/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_4.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 16:45:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 17:01:03 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_wildcard_results(t_input *input, char *expanded_value,
	char *bash_pattern, t_token_part *parts)
{
	char	**wildcards;
	int		i;

	wildcards = expand_wildcards_with_escape(bash_pattern);
	if (wildcards && wildcards[0])
	{
		i = 0;
		while (wildcards[i])
		{
			input->args = append_arg(input->args, wildcards[i]);
			i++;
		}
		free_expanded_wildcards(wildcards);
	}
	else
		input->args = append_arg(input->args, expanded_value);
	cf_free_one(expanded_value);
	cf_free_one(bash_pattern);
	free_token_parts(parts);
}

static bool	handle_simple_expansion(t_input *input, char *expanded_value,
	t_token_part *parts, char **bash_pattern)
{
	if (!parts)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		return (true);
	}
	if (!has_unquoted_wildcards(expanded_value, parts))
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return (true);
	}
	*bash_pattern = create_bash_compatible_pattern(expanded_value, parts);
	if (!*bash_pattern)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return (true);
	}
	return (false);
}

void	handle_bash_compatible_wildcard_expansion(t_input *input,
	char *expanded_value, t_list *tokens_start, t_list *tokens_end)
{
	t_token_part	*parts;
	char			*bash_pattern;

	parts = create_token_parts_list(tokens_start, tokens_end);
	if (handle_simple_expansion(input, expanded_value, parts, &bash_pattern))
		return ;
	handle_wildcard_results(input, expanded_value, bash_pattern, parts);
}
