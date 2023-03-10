
class Vec4 {

    constructor( x, y, z, w ) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w ?? 0;

        this.vec3_data = [x,y,z];
        this.vec4_data = [x,y,z,w];
    }


    scaled( by ) {
        return new Vec4( this.x * by, this.y * by, this.z * by, this.w* by ); 
    }

    dot( other ) {
        return  this.x * other.x + 
                this.y * other.y +
                this.z * other.z +
                this.w * other.w;
    }

    length() {
        return Math.sqrt( this.x * this.x + this.y * this.y + this.z * this.z + this.w * this.w );
    }

    norm() {
        let len = this.length();

        return new Vec4( this.x / len, this.y / len, this.z / len, this.w / len );
    }

    add( other ) {
        return new Vec4( this.x + other.x, this.y + other.y, this.z + other.z, this.w + other.w );
    }

    sub( other ) {
        return this.add( other.scaled( -1 ) );
    }

    mul(vec) {
        let x = this.x * vec.x;
        let y = this.y * vec.y;
        let z = this.z * vec.z;
        let w = this.w * vec.w;
        return new Vec4(x,y,z,w)
    }

    cross( other ) {
        let x = this.y * other.z - this.z * other.y;
        let y = this.x * other.z - this.z * other.x;
        let z = this.x * other.y - this.y - other.x;

        return new Vec4( x, y, z, 0 );
    }
    /**
     * create normalized vector based of input vector
     * @param {Vec4} vector
     * @param {Vec4} normalized vector
     */
    static normalize(vec) {
        const len = Matn.sqrt(vec.x*vec.x + vec.y*vec.y+vec.z*vec.z);
        return new Vec4(vec.x/len, vec.y/len, vec.z/len, vec.w);
    }

    /**
     * Calculate the normal of the given triangle. 
     * For the resulting normal to point in the positive y direction, p0 to p1 should be to the 
     * left of p0 to p2
     * @param {Vec4} p0 
     * @param {Vec4} p1 
     * @param {Vec4} p2 
     * @returns Vec4
     */
    static normal_of_triangle( p0, p1, p2 ) {
        let v0 = p1.sub( p0 );
        let v1 = p2.sub( p0 );
        return v0.cross( v1 );
    }
}