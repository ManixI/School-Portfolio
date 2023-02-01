
class Camera {

    constructor() {
        this.x = 0;
        this.y = 0;
        this.z = 0;

        this.position = new Vec4(0,0,0,1);

        this.yaw = 0;
        this.pitch = 0;
        this.roll = 0;
    }

    // slightly less than a quarter turn up or down allowed
    static get PITCH_LIMIT() { return .24; }

    add_yaw( amount ) { 
        this.yaw += amount; 

        if( this.yaw < 0 ) {
            this.yaw = 1 + this.yaw;
        }

        if( this.yaw > 1 ) {
            this.yaw = this.yaw % 1;
        }
    }
    
    add_pitch( amount ) { 
        this.pitch += amount; 
        if( this.pitch > this.PITCH_LIMIT ) {
            this.pitch = this.PITCH_LIMIT;
        }
        else if( this.pitch < - this.PITCH_LIMIT ) {
            this.pitch = -this.PITCH_LIMIT;
        }
    }

    add_roll( amount ) { 
        this.roll += amount; 

        if( this.roll < -.5 ) { this.roll = -.5; }
        if( this.roll > 5 ) { this.roll = .5; }
    }

    move_in_direction( strafe, up, forward ) {
        let matrix = Camera.get_dir_matrix( this.roll, this.pitch, this.yaw );
        let txed = matrix.transform_vec( new Vec4( strafe, up, forward, 0 ) );

        // console.log( txed.x, txed.y, txed.z, txed.w );

        this.translate_vec( txed );
        
        // note: this is wrong:
        // this.x += strafe;
        // this.y += up;
        // this.z += forward;
    }

    translate( x, y, z ) {
        this.x += x;
        this.y += y;
        this.z += z;
        this.position = new Vec4(x,y,z,1);
    }

    translate_vec( v ) {
        this.translate( v.x, v.y, v.z );
    }

    warp( x, y, z ) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    warp_vec( v ) {
        this.warp( v.x, v.y, v.z );
    }

    static get_dir_matrix( roll, pitch, yaw ) {
        let matrix = new Mat4();

        
        matrix = matrix.mul( Mat4.rotation_xz( yaw ) );
        matrix = matrix.mul( Mat4.rotation_yz( pitch ) );
        matrix = matrix.mul( Mat4.rotation_xy( roll ) );
        
        return matrix;
    }


    get_view_matrix() {
        let matrix = new Mat4();

        matrix = matrix.mul( Mat4.translation( this.x, this.y, this.z ) );
        
        matrix = matrix.mul( Mat4.rotation_xz( this.yaw ) );
        matrix = matrix.mul( Mat4.rotation_yz( this.pitch ) );
        matrix = matrix.mul( Mat4.rotation_xy( this.roll ) );
        
        return matrix.inverse();
    }
}

// workaround because light.js won't load for some reason

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