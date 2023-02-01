class Thing {
    constructor() {
        this.position = new Vec4(0,0,0,1);
        this.pitch = 0;
        this.roll = 0;
        this.yaw = 0;  

        this.up = new Vec4(0,1,0,1);

    }

    // get direction matrix of this thing
    get_dir_mat() {
        let mat = new Mat4();

        mat = mat.mul(Mat4.rotation_xz(this.yaw));
        mat = mat.mul(Mat4.rotation_yz(this.pitch));
        mat = mat.mul(Mat4.rotation_xy(this.roll));

        return mat;
    }
}

class Camera{
    constructor() {
        this.thing = new Thing();
        this.view_mat = this.get_view_matrix();
        this.set_move_speed(0.03,0.03,0.03,0.01,0.01,0.01);

    }

    // like get_pos_mat, but inverted
    get_view_matrix() {
        let mat = new Mat4();
        mat = mat.mul(Mat4.translation(this.thing.position.x, this.thing.position.y, this.thing.position.z));

        // prevent gimble lock
        if (this.thing.pitch > 0.24) {
            this.thing.pitch = 0.24;
        }
        if (this.thing.pitch < -0.24) {
            this.thing.pitch = -0.24
        }
        
        mat = mat.mul(Mat4.rotation_xz(this.thing.yaw));
        mat = mat.mul(Mat4.rotation_yz(this.thing.pitch));
        mat = mat.mul(Mat4.rotation_xy(this.thing.roll));

        return mat.inverse()
    }


    /**
     * orients the camera to look at a specific point
     * @param {Vec4} position to look at
     * @return {Mat4} view matrix
     */
    look_at(pos) {
        let forward = this.thing.position.sub(pos);
        forward = forward.norm();
        let right = forward.cross(Vec4(0,1,0,0));
        let up = new Vec4(0,1,0,0);
        // rotate up by pitch
        // TODO: This
    }
    
    // get direction matrix of a thing
    static get_dir_mat(roll, pitch, yaw) {
        let mat = new Mat4();

        mat = mat.mul(Mat4.rotation_xz(yaw));
        mat = mat.mul(Mat4.rotation_yz(pitch));
        mat = mat.mul(Mat4.rotation_xy(roll));

        return mat;
    }
    

    /**
     * translate camera in direction
     * @param {float} x
     * @param {float} y
     * @param {float} z
     */
     /*
    move_in_dir(x, y, z) {
        let mat = Camera.get_dir_mat(this.thing.roll, this.thing.pitch, this.thing.yaw);
        let txed = mat.transform_vec( new Vec4(x, y, z, 0));
        //console.log(txed);

        this.thing.x += txed.x;
        this.thing.y += txed.y;
        this.thing.z += txed.z;
        console.log(this.thing.x);
    }
    */

    move_in_dir( strafe, up, forward ) {
        //console.log(this.thing.pitch);
        let matrix = Camera.get_dir_mat( this.thing.roll, this.thing.pitch, this.thing.yaw );
        let txed = matrix.transform_vec( new Vec4( strafe, up, forward, 0 ) );

        // console.log( txed.x, txed.y, txed.z, txed.w );

        this.translate_vec( txed );
        
        // note: this is wrong:
        // this.x += strafe;
        // this.y += up;
        // this.z += forward;
    }

    translate( x, y, z ) {
        this.thing.position.x += x;
        this.thing.position.y += y;
        this.thing.position.z += z;
    }

    translate_vec( v ) {
        this.translate( v.x, v.y, v.z );
    }

    /** roll by ammount
     * @param {float} roll
     */
     add_roll(roll) {
        this.roll += roll;
     }

     /** 
      * pitch by ammount
      * limit set in get_view_matrix
      * @param {float} pitch
      */
     add_pitch(pitch) {
        this.pitch += pitch;
     }

     add_yaw(yaw) {
        this.yaw += yaw;
     }

    /**
     * set camera move speeds
     * @param {float} x
     * @param {float} y
     * @param {float} x
     * @param {float} roll
     * @param {float} pitch
     * @param {float} yaw
     */
    set_move_speed(x,y,z,roll,pitch,yaw) {
        this.x_speed = x;
        this.y_speed = y;
        this.z_speed = z;

        this.roll_speed = roll;
        this.pitch_speed = pitch;
        this.yaw_speed = yaw;
    }
}


/*
v = (T*Y*P*R)^-1
Roll: rotating in the XY plane
Pitch: rotating in the YZ plane
Yaw: rotating in the XZ plane
*/