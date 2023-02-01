
class Light {
    /**
     * Construct a light. 
     * @param {number} x 
     * @param {number} y 
     * @param {number} z 
     * @param {number} r 
     * @param {number} g 
     * @param {number} b 
     * @param {number} light_no 
     */
    constructor( x, y, z, r, g, b, light_no ) {
        this.x = x;
        this.y = y;
        this.z = z;

        this.r = r;
        this.g = g;
        this.b = b;

        this.light_no = light_no;
    }

    /**
     * 
     * @param {WebGLRenderingContext} gl 
     * @param {WebGLProgram} program
     */
    bind( gl, program, modelview ) {
        if( this.light_no == 0 ) { // this is the sun
            set_uniform_vec3( gl, program, 'sun_dir', this.x, this.y, this.z );
            set_uniform_vec3( gl, program, 'sun_color', this.r, this.g, this.b );
        }
        else {
            set_uniform_vec3( 
                gl, program, 
                'light' + this.light_no + '_loc', 
                this.x, this.y, this.z 
            );

            set_uniform_vec3( 
                gl, program, 
                'light' + this.light_no + '_color', 
                this.r, this.g, this.b 
            );
        }
    }

}

/**
 * light class but for nodes
 * skip xyz because they are stored in the node
 * skip light_no because that is decided by positon
 * 
 * @param {number} r 
 * @param {number} g 
 * @param {number} b
 */ 
class NodeLight {
    constructor(r, b, g, is_the_sun){
        this.r = r;
        this.b = b;
        this.g = g;

        this.is_the_sun = is_the_sun;
    }

    bind( gl, program, modelview, x, y, z, light_no ) {
        if( this.is_the_sun) { // this is the sun
            set_uniform_vec3( gl, program, 'sun_dir', x, y, z );
            set_uniform_vec3( gl, program, 'sun_color', this.r, this.g, this.b );
        }
        else {
            set_uniform_vec3( 
                gl, program, 
                'light' + light_no + '_loc', 
                x, y, z 
            );

            set_uniform_vec3( 
                gl, program, 
                'light' + light_no + '_color', 
                r, g, b 
            );
        }
    }

}