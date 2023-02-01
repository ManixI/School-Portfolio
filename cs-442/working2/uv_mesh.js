const NUM_STRIDE_ELEMENTS = 12;
const VERTEX_STRIDE = NUM_STRIDE_ELEMENTS * Float32Array.BYTES_PER_ELEMENT;

class UvMesh {
    /** 
     * Creates a new mesh and loads it into video memory.
     * 
     * @param {WebGLRenderingContext} gl  
     * @param {number} program
     * @param {number[]} vertices
     * @param {number[]} indices
    */
    constructor( gl, program, vertices, indices, material ) {
        this.verts = create_and_load_vertex_buffer( gl, vertices, gl.STATIC_DRAW );
        this.indis = create_and_load_elements_buffer( gl, indices, gl.STATIC_DRAW );

        this.n_verts = vertices.length / NUM_STRIDE_ELEMENTS;
        this.n_indis = indices.length;
        this.program = program;
        this.material = material;

    }

    set_vertex_attributes() {
        set_vertex_attrib_to_buffer( 
            gl, this.program, 
            "coordinates", 
            this.verts, 3, 
            gl.FLOAT, false, VERTEX_STRIDE, 0 
        );

        set_vertex_attrib_to_buffer( 
            gl, this.program, 
            "color", 
            this.verts, 4, 
            gl.FLOAT, false, VERTEX_STRIDE, 12
        );

        set_vertex_attrib_to_buffer( 
            gl, this.program,
            "uv",
            this.verts, 2,
            gl.FLOAT, false, VERTEX_STRIDE, 28
        );

        set_vertex_attrib_to_buffer( 
            gl, this.program,
            "normal",
            this.verts, 2,
            gl.FLOAT, false, VERTEX_STRIDE, 36
        );
    }
    

    /**
     * Create a box mesh with the given dimensions and colors. Creates normals.
     * @param {WebGLRenderingContext} gl 
     * @param {number} width 
     * @param {number} height 
     * @param {number} depth 
     */

    static box( gl, program, width, height, depth, material ) {
        let hwidth = width / 2.0;
        let hheight = height / 2.0;
        let hdepth = depth / 2.0;
        
        let verts = [
            hwidth, -hheight, -hdepth,  1.0, 0.0, 1.0, 1.0,     1.0, 1.0,   
            -hwidth, -hheight, -hdepth, 0.0, 1.0, 1.0, 1.0,     0.0, 1.0, 
            -hwidth, hheight, -hdepth,  0.5, 0.5, 1.0, 1.0,     0.0, 0.0, 
            hwidth, hheight, -hdepth,   1.0, 1.0, 0.5, 1.0,     1.0, 0.0, 

            hwidth, -hheight, -hdepth,  1.0, 0.0, 1.0, 1.0,     1.0, 1.0,   
            -hwidth, -hheight, -hdepth, 0.0, 1.0, 1.0, 1.0,     0.0, 1.0, 
            -hwidth, hheight, -hdepth,  0.5, 0.5, 1.0, 1.0,     0.0, 0.0, 
            hwidth, hheight, -hdepth,   1.0, 1.0, 0.5, 1.0,     1.0, 0.0, 

            hwidth, -hheight, hdepth,   1.0, 0.0, 1.0, 1.0,     1.0, 1.0,  
            hwidth, -hheight, -hdepth,  0.0, 1.0, 1.0, 1.0,     0.0, 1.0, 
            hwidth, hheight, -hdepth,   0.5, 0.5, 1.0, 1.0,     0.0, 0.0,  
            hwidth, hheight, hdepth,    1.0, 1.0, 0.5, 1.0,     1.0, 0.0,  

            -hwidth, -hheight, hdepth,  1.0, 0.0, 1.0, 1.0,     1.0, 1.0,  
            hwidth, -hheight, hdepth,   1.0, 1.0, 0.5, 1.0,     0.0, 1.0,  
            hwidth, hheight, hdepth,    0.5, 0.5, 1.0, 1.0,     0.0, 0.0,   
            -hwidth, hheight, hdepth,   0.0, 1.0, 1.0, 1.0,     1.0, 0.0,  
            
            -hwidth, -hheight, hdepth,  1.0, 0.0, 1.0, 1.0,     0.0, 1.0, 
            -hwidth, -hheight, -hdepth, 0.0, 1.0, 1.0, 1.0,     1.0, 1.0, 
            -hwidth, hheight, -hdepth,  0.5, 0.5, 1.0, 1.0,     1.0, 0.0,  
            -hwidth, hheight, hdepth,   1.0, 1.0, 0.5, 1.0,     0.0, 0.0,   

            -hwidth, hheight, -hdepth,  1.0, 0.0, 0.0, 1.0,     0.0, 1.0,  
            hwidth, hheight, -hdepth,   0.0, 1.0, 0.0, 1.0,     1.0, 1.0,  
            hwidth, hheight, hdepth,    0.0, 0.0, 1.0, 1.0,     1.0, 0.0,  
            -hwidth, hheight, hdepth,   1.0, 1.0, 0.0, 1.0,     0.0, 0.0,  

            -hwidth, -hheight, -hdepth, 1.0, 0.0, 0.0, 1.0,     0.0, 1.0,   
            hwidth, -hheight, -hdepth,  0.0, 1.0, 0.0, 1.0,     1.0, 1.0,  
            hwidth, -hheight, hdepth,   0.0, 0.0, 1.0, 1.0,     1.0, 0.0,  
            -hwidth, -hheight, hdepth,  1.0, 1.0, 0.0, 1.0,     0.0, 0.0,  
        ];

        let indis = [
            // clockwise winding
            0, 3, 2, 2, 1, 0,
            4, 7, 6, 6, 5, 4,
            8, 11, 10, 10, 9, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 23, 22, 22, 21, 20,
        ];

        return new UvMesh( gl, program, verts, indis, material );
    }

    /**
     * Create a box mesh but using UV coordinates that support 6-sided texture mapping. 
     * @param {WebGLRenderingContext} gl 
     * @param {number} width 
     * @param {number} height 
     * @param {number} depth 
     */

     static mapped_box( gl, program, width, height, depth, material ) {
        let hwidth = width / 2.0;
        let hheight = height / 2.0;
        let hdepth = depth / 2.0;

        let verts = [
            hwidth, -hheight, -hdepth,  1.0, 0.0, 1.0, 1.0,     .25, .5,   
            -hwidth, -hheight, -hdepth, 0.0, 1.0, 1.0, 1.0,     0, .5, 
            -hwidth, hheight, -hdepth,  0.5, 0.5, 1.0, 1.0,     0, .25, 
            hwidth, hheight, -hdepth,   1.0, 1.0, 0.5, 1.0,     .25, .25, 

            hwidth, -hheight, hdepth,   1.0, 0.0, 1.0, 1.0,     .5, .5,  
            hwidth, -hheight, -hdepth,  0.0, 1.0, 1.0, 1.0,     .25, .5, 
            hwidth, hheight, -hdepth,   0.5, 0.5, 1.0, 1.0,     .25, .25,  
            hwidth, hheight, hdepth,    1.0, 1.0, 0.5, 1.0,     .5, .25,  

            -hwidth, -hheight, hdepth,  1.0, 0.0, 1.0, 1.0,     .75, .5,  
            hwidth, -hheight, hdepth,   1.0, 1.0, 0.5, 1.0,     .5, .5,  
            hwidth, hheight, hdepth,    0.5, 0.5, 1.0, 1.0,     .5, .25,   
            -hwidth, hheight, hdepth,   0.0, 1.0, 1.0, 1.0,     .75, .25,  
            
            -hwidth, -hheight, hdepth,  1.0, 0.0, 1.0, 1.0,     1, .5, 
            -hwidth, -hheight, -hdepth, 0.0, 1.0, 1.0, 1.0,     .75, .5, 
            -hwidth, hheight, -hdepth,  0.5, 0.5, 1.0, 1.0,     .75, .25,  
            -hwidth, hheight, hdepth,   1.0, 1.0, 0.5, 1.0,     1, .25,   

            -hwidth, hheight, -hdepth,  1.0, 0.0, 0.0, 1.0,     .75,.25,  
            hwidth, hheight, -hdepth,   0.0, 1.0, 0.0, 1.0,     .5, .25,  
            hwidth, hheight, hdepth,    0.0, 0.0, 1.0, 1.0,     .5, 0,  
            -hwidth, hheight, hdepth,   1.0, 1.0, 0.0, 1.0,     .75, 0,  

            -hwidth, -hheight, -hdepth, 1.0, 0.0, 0.0, 1.0,     .75, .75,   
            hwidth, -hheight, -hdepth,  0.0, 1.0, 0.0, 1.0,     .5, .75,  
            hwidth, -hheight, hdepth,   0.0, 0.0, 1.0, 1.0,     .5, .5,  
            -hwidth, -hheight, hdepth,  1.0, 1.0, 0.0, 1.0,     .75, .5,  
        ];

        let indis = [
            // clockwise winding
            0, 3, 2, 2, 1, 0,
            4, 7, 6, 6, 5, 4,
            8, 11, 10, 10, 9, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 23, 22, 22, 21, 20,
        ];

        return new UvMesh( gl, program, verts, indis, material );
    }

    /**
     * make an orb of given radius at position
     */
    static orb(gl, program, subdivs, material) {
        let verts = [];
        let indis = [];
        //let normals = []
        /*
        const TAU = (2*Math.PI);
        // for each layer
        for (let layer = 0; layer <= subdivs; layer++){
                let y_turns = layer / subdivs / 2;
                let y = Math.cos(y_turns * TAU) / 2;
                for(let subdiv=0; subdiv<=subdivs; subdiv++) {
                    let turns = subdiv/subdivs;
                    let rads = turns*TAU;
                    //TODO: squish to sphear instead of cilinder
                    let x=Math.cos(rads)/2;
                    x = x * Math.sqrt(1-(y*y*4));
                    y = y * Math.sqrt(1-(y*y*4));
                    let z=Math.sin(rads)/2;

                    let u = subdiv/subdivs;
                    let v = layer/subdivs;

                    verts.push(x,y,z);
                    // push colour
                    verts.push(0.5,0.5,0.5,1);
                    verts.push(u,v);

                    // normal vecttor from 0,0,0 to point xyz
                    //let normal = Vec4.normalize(new Vec4(x,y,z,0));
                    // cheaty way of maing normals, but faster
                    
                    //let max_val = max(x,max(y,z));
                    //let normal = new Vec4(x/max_val,y/max_val,z/max_val,0)
                    //verts.push(normal.x, normal.y, normal.z)
                    
                    verts.push(x,y,z);
                    
                    // indis
                    let t1 = layer*(subdivs+1)+subdiv;
                    let tr = t1 + 1;
                    let b1 = t1 + subdivs + 1;
                    let br = b1 + 1;
                    indis.push(t1, b1, br);
                    ind
                    is.push(t1, br, tr);
                    
                }
            }
            */
            // trying assumed good code from:
            // https://github.com/loukoum/sphere-generation/blob/master/sphere-generator.js
            // still dosen't work though...
            var x, y, z;
            var s, t;
            var phi, theta;
            for (var i = 0; i <= subdivs; i++) {
                phi = i * Math.PI / subdivs;
                for (var j = 0; j <= subdivs; j++) {
                    theta = j * 2.0 * Math.PI / subdivs;
                    z = Math.sin(phi) * Math.cos(theta);
                    x = Math.sin(phi) * Math.sin(theta);
                    y = Math.cos(phi);
                    verts.push(x);
                    verts.push(y);
                    verts.push(z);

                    verts.push(1,1,1,1);

                    t = phi / Math.PI;
                    s = theta / (2 * Math.PI);

                    verts.push(s);
                    verts.push(t);

                    verts.push(x);
                    verts.push(y);
                    verts.push(z);
                }
            }

              // Then the function generate indices for drawing triangles.
              // Each quad will be drawn as two triangles, top-left and bottom-right.
              for (var i = 0; i < subdivs; i++) {
                for (var j = 0; j < subdivs; j++) {

                  // index for top-left, top-right, bot-left and bot-right points.
                  var tl = (i * (subdivs + 1)) + j;
                  var tr = tl + 1;
                  var bl = tl + subdivs + 1;
                  var br = bl + 1;

                  indis.push(tl);
                  indis.push(bl);
                  indis.push(br);

                  indis.push(tl);
                  indis.push(br);
                  indis.push(tr);
                }
              }


        return new UvMesh(gl, program, verts, indis, material);
    }

    /**
     * Render the mesh. Does NOT preserve array/index buffer, program, or texture bindings! 
     * 
     * @param {WebGLRenderingContext} gl 
     */


    render( gl ) {
        gl.enable( gl.CULL_FACE );
        
        gl.useProgram( this.program );
        this.set_vertex_attributes();
        gl.bindBuffer( gl.ARRAY_BUFFER, this.verts );
        gl.bindBuffer( gl.ELEMENT_ARRAY_BUFFER, this.indis );
        bind_texture_samplers( gl, this.program, "tex_0" );

        gl.activeTexture( gl.TEXTURE0 );
        this.material.bind( gl, this.program );
        //this.material.render_mat(gl, this.program);

        gl.drawElements( gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0 );
    }
}