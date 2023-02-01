// Functions to generate an astroide belt

/**
 * return random number in range
 * @param {float} min
 * @param {float} max
 * @return {float} rand no
 */
function rand_range(min, max) {
	return Math.random() * (max-min) + min;
}

/**
 * generates a random xy within specifide inner and outer radius
 * @param {float} min radius
 * @param {float} max radius
 * @return {float[]} xy pair
 */
function point_in_ring(min_rad, max_rad) {
	let theta = rand_range(0, 2 * Math.PI);
	let w = rand_range(0,1);
	let r = Math.sqrt((1.0 - w) * min_rad**2 + w * max_rad**2);
	return [r*Math.cos(theta), r*Math.sin(theta)];
}

/** 
 * seeded rng
 * from: https://stackoverflow.com/questions/521295/seeding-the-random-number-generator-in-javascript
 */
function mulberry32(a) {
    return function() {
      var t = a += 0x6D2B79F5;
      t = Math.imul(t ^ t >>> 15, t | 1);
      t ^= t + Math.imul(t ^ t >>> 7, t | 61);
      return ((t ^ t >>> 14) >>> 0) / 4294967296;
    }
}

