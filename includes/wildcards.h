/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:18:15 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 18:01:08 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WILDCARDS_H
# define WILDCARDS_H

// wildcards_1.c
int		matches_pattern_with_escape(const char *str, const char *pattern);
char	**expand_wildcards_with_escape(const char *pattern);
void	handle_wildcard_expansion(t_input *input,
			t_token *current_tok, char *expanded_value);
void	free_expanded_wildcards(char **expanded);

// wildcars_2.c
void	handle_glob_expansion(t_input *input,
			char *expanded_value, t_list *tokens_start, t_list *tokens_end);

// wildcard_helpers.c
char	**return_single_pattern(const char *pattern);
int		count_matching_files(const char *pattern);
bool	append_match_to_result(char **result, int *i, const char *name);
char	**collect_matching_files(const char *pattern, int count);

#endif
