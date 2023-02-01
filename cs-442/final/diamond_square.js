// Diamond Square map generator







function fractal_map(scale, roughness, min, max, center) {
	let side_len = 2 ** scale +1;
	let map = []
	for (let i = 0; i<side_len; i++) {
		map.push(new Array(side_len));
	}

	side_len = side_len-1
	map[0][0] = rand_range(min, max);
	map[side_len][0] = rand_range(min, max);
	map[0][side_len] = rand_range(min, max);
	map[side_len][side_len] = rand_range(min, max);

	map[center][center] = 
		(map[0][0] + map[0][side_len] + map[side_len][0] + map[side_len][side_len]) / 4 + rand_range(min, max)

	north 
}