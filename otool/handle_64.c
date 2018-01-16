/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_64.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ademenet <ademenet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 15:50:23 by ademenet          #+#    #+#             */
/*   Updated: 2018/01/16 14:06:56 by ademenet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/ft_otool.h"

static int						display_64(struct section_64 *sec)
{
	uint64_t					i;
	long unsigned int			addr;

	i = 0;
	addr = sec->addr;
	while (i < sec->size)
	{
		if (i == 0 || i % 16 == 0)
		{
			if (i != 0)
				addr += 16;
			ft_printf("%016llx\t", addr);
		}
		// display_bytes();
		i++;
	}
	if (i > 0)
		ft_putchar('\n');
	return (EXIT_SUCCESS);
}

static int						print_output_64(struct segment_command_64 *seg,
								struct section_64 *sec, struct mach_header_64
								*header, char show_filename)
{
	uint32_t					i;

	i = 0;
	if (show_filename)
		ft_printf("%s:\n", g_env.file);
	while (i < seg->nsects)
	{
		if (ft_strcmp(sec->sectname, SECT_TEXT) == 0 &&
			ft_strcmp(sec->segname, SEG_TEXT) == 0)
		{
			ft_printf("(char *)header + sect->offset = %s\n", (char *)header + sec->offset);
			ft_printf("%s\n", sec->sectname);
			display_64(sec);
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

int								handle_64(char *ptr, char show_filename)
{
	uint32_t					i;
	struct mach_header_64		*header;
	struct load_command			*lc;
	struct segment_command_64	*seg;
	struct section_64			*sec;

	i = -1;
	header = (struct mach_header_64 *)ptr;
	lc = (void *)ptr + sizeof(*header);
	while (++i < header->ncmds)
	{
		if (check(lc))
			return (EXIT_FAILURE);
		if (lc->cmd == LC_SEGMENT_64)
		{
			seg = (struct segment_command_64 *)lc;
			sec = (struct section_64 *)((void *)seg + sizeof(* seg));
			if (check(seg) || check(sec) ||
				print_output_64(seg, sec, header, show_filename))
				return (EXIT_FAILURE);
		}
		lc = (void *)lc + lc->cmdsize;
	}
	return (EXIT_SUCCESS);
}
