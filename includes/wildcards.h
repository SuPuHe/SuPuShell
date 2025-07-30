/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:18:15 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 17:35:08 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WILDCARDS_H
# define WILDCARDS_H

// wildcards.c
char	**expand_wildcards(const char *pattern);
void	free_expanded_wildcards(char **expanded);

// wildcars_second_part.c
int		matches_pattern(const char *str, const char *pattern);
int		count_matches(const char *pattern);
char	**create_result_array(int size);
char	**handle_no_wildcards(const char *pattern);

#endif
