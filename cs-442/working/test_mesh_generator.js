const VERTEX_STRIDE = 36;


class TestMesh {
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

        this.n_verts = vertices.length;
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

    }

    /**
     * Create a box mesh with the given dimensions and colors.
     * @param {WebGLRenderingContext} gl 
     * @param {number} width 
     * @param {number} height 
     * @param {number} depth 
     */

    static box_with_tex( gl, program, width, height, depth, texture, t_width, t_height) {
        let hwidth = width / 2.0;
        let hheight = height / 2.0;
        let hdepth = depth / 2.0;
        // x, y, z,     R, B, G, A,     u, v
        let verts = [
            // 1
            hwidth, -hheight, -hdepth,      0.0, 0.0, 0.0, 1.0,     1,1, // bottom right fromt
            -hwidth, -hheight, -hdepth,     0.0, 0.0, 0.0, 1.0,     0,1, // bottom left front
            -hwidth, hheight, -hdepth,      0.0, 0.0, 0.0, 1.0,     0,0, // top left front
            hwidth, hheight, -hdepth,       0.0, 0.0, 0.0, 1.0,     1,0, // top right front

            // 2 
            hwidth, hheight, -hdepth,       0.0, 0.0, 0.0, 1.0,     1,1, // top right front
            -hwidth, hheight, -hdepth,      0.0, 0.0, 0.0, 1.0,     0,1, // top left front
            -hwidth, hheight, hdepth,       0.0, 0.0, 0.0, 1.0,     0,0, // top left back
            hwidth, hheight, hdepth,        0.0, 0.0, 0.0, 1.0,     1,0, // top right back

            // 3
            hwidth, -hheight, hdepth,      0.0, 0.0, 0.0, 1.0,     1,1, // bottom right back
            hwidth, -hheight, -hdepth,      0.0, 0.0, 0.0, 1.0,     0,1, // bottom right front
            hwidth, hheight, -hdepth,       0.0, 0.0, 0.0, 1.0,     0,0, // top right front
            hwidth, hheight, hdepth,        0.0, 0.0, 0.0, 1.0,     1,0, // top right back

            // 4
            -hwidth, -hheight, -hdepth,     0.0, 0.0, 0.0, 1.0,     1,1, // bottom left front
            -hwidth, -hheight, hdepth,       0.0, 0.0, 0.0, 1.0,     0,1, // bottom left back
            -hwidth, hheight, hdepth,       0.0, 0.0, 0.0, 1.0,     0,0, // top left back
            -hwidth, hheight, -hdepth,      0.0, 0.0, 0.0, 1.0,     1,0, // top left front

            // 5
            hwidth, -hheight, hdepth,      0.0, 0.0, 0.0, 1.0,     1,1, // bottom right back
            -hwidth, -hheight, hdepth,       0.0, 0.0, 0.0, 1.0,     0,1, // bottom left back
            -hwidth, -hheight, -hdepth,     0.0, 0.0, 0.0, 1.0,     0,0, // bottom left front
            hwidth, -hheight, -hdepth,      0.0, 0.0, 0.0, 1.0,     1,0, // bottom right front

            // 6
            hwidth, -hheight, hdepth,       0.0, 0.0, 0.0, 1.0,     1,1, // bottom left back
            -hwidth, -hheight, hdepth,      0.0, 0.0, 0.0, 1.0,     0,1, // bottom right back
            -hwidth, hheight, hdepth,       0.0, 0.0, 0.0, 1.0,     0,0, // top left back
            hwidth, hheight, hdepth,        0.0, 0.0, 0.0, 1.0,     1,0, // top right back
        ];

        let indis = [
            // clockwise winding
            /*
            0, 1, 2, 2, 3, 0, 
            4, 0, 3, 3, 7, 4, 
            5, 4, 7, 7, 6, 5, 
            1, 5, 6, 6, 2, 1,
            3, 2, 6, 6, 7, 3,
            4, 5, 1, 1, 0, 4,
            */

            // counter-clockwise winding
            0,  3,  2,  2,  1,  0,
            4,  7,  6,  6,  5,  4,
            8,  11, 10, 10, 9,  8,
            12, 15, 14, 14, 13, 12,
            16, 19, 18, 18, 17, 16,
            20, 21, 22, 22, 23, 20,
            //20, 23, 22, 22, 21, 20,
        ];


        // debug texture
        let tex = gl.createTexture();
        gl.bindTexture (gl.TEXTURE_2D, tex);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.bindTexture (gl.TEXTURE_2D, tex);
        gl.texImage2D(
            gl.TEXTURE_2D, 0, gl.RGBA,
            t_width, t_height, 0,
            gl.RGBA, gl.UNSIGNED_BYTE,
            texture
            );
            

        return new TestMesh( gl, program, verts, indis, tex);
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
     * create an orb at a given position
     * @parma {WebGLRenderingGontext} gl
     * @param {WebGLProgram} program
     * @param {Vec4} position
     * @param {Texture} texture
     */
    static orb(gl, program, position, tex)

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
        this.material.bind( gl );

        gl.drawElements( gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0 );
    }