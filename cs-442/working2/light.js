console.log("loaded light.js");
class Sun {
	/**
	 * sun type light
	 * light properties are vectors to make handling colour simpler
	 * @param {WebGLContest} gl
	 * @param {number} program
	 * @param {Vec4} direction
	 * @param {Vec4} ambiant light
	 * @param {Vec4} diffuse light
	 * @param {Vec4} specular light
	 * @param {Vec4} color
	 */
	constructor(gl, program, direction, ambient, diffuse, specular, color) {
		this.program = program;
		this.direction = direction;
		this.ambient = ambient;
		this.diffuse = diffuse;
		this.specular = specular;

		this.color = color;

		update(gl);
	}

	update(gl) {
		let dir = this.direction.mul(this.color);
		//console.log(dir.vec3_data);
		let amb = this.ambient.mul(this.color);
		let dif = this.diffuse.mul(this.color);
		let spe = this.specular.mul(this.color);
		set_uniform_vec3_array(gl, this.program, 'sun.direction', dir.vec3_data);
		set_uniform_vec3_array(gl, this.program, 'sun.ambient', amb.vec3_data);
		set_uniform_vec3_array(gl, this.program, 'sun.diffuse', dif.vec3_data);
		set_uniform_vec3_array(gl, this.program, 'sun.specular', spe.vec3_data);
	}
}

class Point_Light {
	/**
	 * light class
	 * @param {WebGLRenderingContext} gl  
     * @param {number} program
	 * @param {Vec4} position
	 * @param {float} ambient
	 * @param {float} diffuse
	 * @param {float} specular
	 * @param {Vec4} color
	 * @param {float} attenuation constant
	 * @param {float} attenuation linear
	 * @param {float} attenuation quadratic
	 */
	constructor(gl, program, position, ambient_str, diffuse_str, specular_str, color, constant, linear, quadratic) {
		this.position = position;
		this.color = color;

		this.program = program;

		this.ambient_str = ambient_str;
		this.diffuse_str = diffuse_str;
		this.specular_str = specular_str;

		this.constant = constant;
		this.linear = linear;
		this.quadratic = quadratic;

		update(gl);
	}

	update(gl) {
		// diffuse
		set_uniform_vec3_array(gl, this.program, 'light[0].position', this.position.vec3_data);

		let dif = new Vec4(this.diffuse_str, this.diffuse_str, this.diffuse_str, 0);
		let amb = new Vec4(this.ambient_str, this.ambient_str, this.ambient_str, 0);
		let spec = new Vec4(this.specular_str, this.specular_str, this.specular_str, 0);

		dif = dif.mul(this.color);
		amb = amb.pul(this.color);
		spec = spec.mul(this.color);

		set_uniform_vec3_array(gl, this.program, 'light[0].diffuse', dif.vec3_data);
		set_uniform_vec3_array(gl, this.program, 'light[0].ambient', dif.vec3_data);
		set_uniform_vec3_array(gl, this.program, 'light[0].specular', dif.vec3_data);
		
		set_unifomr_float(gl, this.program, 'light[0].constant', this.constant);
		set_unifomr_float(gl, this.program, 'light[0].linear', this.linear);
		set_unifomr_float(gl, this.program, 'light[0].quadratic', this.quadratic);
	}
}
// TODO: Spotlight class