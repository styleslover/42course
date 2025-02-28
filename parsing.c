/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabrigo <mabrigo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 22:52:55 by mariel            #+#    #+#             */
/*   Updated: 2025/02/26 17:47:15 by mabrigo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

char	*strcmp_from_i(int i, char *src)
{
	int		j;
	int		len;
	char	*dest;

	if (!src)
		return (NULL);
	len = ft_strlen(src) - i + 1;
	if (len <= 0)
		return (NULL);
	dest = malloc(sizeof(char) * len);
	if (!dest)
		return (NULL);
	j = 0;
	while (src[i])
		dest[j++] = src[i++];
	dest[j] = '\0';
	return (dest);
}

void	parse_config_line(char *str, t_map_data *map)
{
	int	i;

	i = 0;
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (ft_strncmp(str, "NO ", 3) == 0)
		map->north_txtr = strcmp_from_i(i + 3, str);
	else if (ft_strncmp(str, "SO ", 3) == 0)
		map->south_txtr = strcmp_from_i(i + 3, str);
	else if (ft_strncmp(str, "WE ", 3) == 0)
		map->west_txtr = strcmp_from_i(i + 3, str);
	else if (ft_strncmp(str, "EA ", 3) == 0)
		map->east_txtr = strcmp_from_i(i + 3, str);
	else if (ft_strncmp(str, "F ", 2) == 0)
		map->floor_color = strcmp_from_i(i + 2, str);
	else if (ft_strncmp(str, "C ", 2) == 0)
		map->ceiling_color = strcmp_from_i(i + 2, str);
}

int	is_map_line(char *str)
{
	int	i;

	i = 0;
	if (!str[i])
		return (0);
	while (str[i] && str[i] == ' ')
		i++;
	if (str[i] == '1' || str[i] == '0' || str[i] == 'N'
		|| str[i] == 'S' || str[i] == 'E' || str[i] == 'W')
	{
		if (str[i + 1] == 'O' || str[i + 1] == 'A' || str[i + 1] == 'E')
			return (0);
		else
		{
			printf("Map line: %s\n", str);//debug
			return (1);
		}
	}
	return (0);
}

int	count_lines(char *av, int fd)
{
	int		lines;
	char	*line;

	lines = 0;
	fd = open(av, O_RDONLY);
	if (fd < 0)
		return (0);
	while ((line = get_next_line(fd)))
	{
		lines++;
		free(line);
	}
	close(fd);
	return (lines);
}

// Funzione per caricare la mappa
char	**load_map(char *av, int fd)
{
	int		i;
	int		lines;
	char	**map;

    if (fd < 0)
		return (0);
	lines = count_lines(av, fd);
	if (lines <= 0)
		return (NULL);
	map = malloc((lines + 1) * sizeof(char *));  // +1 per NULL finale
	if (!map)
		return (NULL);
	fd = open(av, O_RDONLY);
	if (fd < 0)
	{
		free(map);
		return (NULL);
	}
	i = 0;
	while (i < lines)
	{
		map[i] = get_next_line(fd);
		if (!map[i])  // Se get_next_line fallisce
		{
			while (i > 0)
				free(map[--i]);
			free(map);
			close(fd);
			return (NULL);
		}
		printf("Map line %d: %s\n", i, map[i]);//debug
		i++;
	}
	map[i] = NULL;  // Terminatore NULL per l'array di stringhe
	close(fd);
	return (map);
}

void	parse_file(char **av, int fd, t_map_data *map)
{
	char	*line;
	int		line_len;

	init_map(map);
	line_len = 0;
	while ((line = get_next_line(fd)))
	{
		if (is_map_line(line) != 0)
		{
			line_len = ft_strlen(line);
			if (line_len > map->win_width)
				map->win_width = line_len;
			map->win_height++;
		}
		else
			parse_config_line(line, map);
		free(line);
	}
	close(fd);
	map->world = load_map(av[1], fd);
	if (!map->world)
	{
		printf("Error: Failed to load map\n");
		exit(1);
	}
	map->win_width *= TILE_SIZE;
	map->win_height *= TILE_SIZE;
	if (map->win_width >= WIDTH || map->win_height >= 1080)
	{
		map->win_width = WIDTH;
		map->win_height = HEIGHT;
	}
	printf("North Texture: %s\n", map->north_txtr);
	printf("South Texture: %s\n", map->south_txtr);
	printf("West Texture: %s\n", map->west_txtr);
	printf("East Texture: %s\n", map->east_txtr);
	printf("Floor Color: %s\n", map->floor_color);
	printf("Ceiling Color: %s\n", map->ceiling_color);
	printf("Win Size (pxl): %d x %d\n", map->win_width, map->win_height);
}
