/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:12:38 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 17:12:42 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLORS_H
# define COLORS_H

# define RESET   "\033[0m"
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE    "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN    "\033[36m"
# define WHITE   "\033[37m"

# define BOLDBLACK   "\033[1m\033[30m"
# define BOLDRED     "\033[1m\033[31m"
# define BOLDGREEN   "\033[1m\033[32m"
# define BOLDYELLOW  "\033[1m\033[33m"
# define BOLDBLUE    "\033[1m\033[34m"
# define BOLDMAGENTA "\033[1m\033[35m"
# define BOLDCYAN    "\033[1m\033[36m"
# define BOLDWHITE   "\033[1m\033[37m"

# define SHELLNAME \
"\001\033[1m\033[32m\002➜  \001\033[0m\002" \
"\001\033[1m\033[36m\002SuPuShell\001\033[0m\002" \
"\001\033[1m\033[34m\002 git:(\001\033[0m\002" \
"\001\033[1m\033[31m\002master\001\033[0m\002" \
"\001\033[1m\033[34m\002)\001\033[0m\002" \
"\001\033[1m\033[33m\002 ✗ \001\033[0m\002"

#endif
