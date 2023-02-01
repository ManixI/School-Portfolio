
const VERTEX_STRIDE = 48;


class NormalMesh {
    /** 
     * Creates a new mesh and loads it into video memory.
     * 
     * @param {WebGLRenderingContext} gl  
     * @param {number} program
     * @param {number[]} vertices
     * @param {number[]} indices
    */
    constructor( gl, program, vertices, indices, material, use_color ) {
        this.verts = create_and_load_vertex_buffer( gl, vertices, gl.STATIC_DRAW );
        this.indis = create_and_load_elements_buffer( gl, indices, gl.STATIC_DRAW );

        this.n_verts = vertices.length / VERTEX_STRIDE * 4;
        this.n_indis = indices.length;
        this.program = program;
        this.material = material;

        this.use_color = use_color ?? false;
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
            "surf_normal",
            this.verts, 3, 
            gl.FLOAT, false, VERTEX_STRIDE, 36
        )
    }
    

    /**
     * Create a box mesh with the given dimensions and colors. Creates normals.
     * @param {WebGLRenderingContext} gl 
     */

    static box( gl, program, width, height, depth, material ) {
        let hwidth = width / 2.0;
        let hheight = height / 2.0;
        let hdepth = depth / 2.0;

        let verts = [
            hwidth, -hheight, -hdepth,  1.0, 0.0, 1.0, 1.0,     1.0, 1.0,   0.0, 0.0, -1.0,
            -hwidth, -hheight, -hdepth, 0.0, 1.0, 1.0, 1.0,     0.0, 1.0,   0.0, 0.0, -1.0,
            -hwidth, hheight, -hdepth,  0.5, 0.5, 1.0, 1.0,     0.0, 0.0,   0.0, 0.0, -1.0,
            hwidth, hheight, -hdepth,   1.0, 1.0, 0.5, 1.0,     1.0, 0.0,   0.0, 0.0, -1.0,

            hwidth, -hheight, hdepth,   1.0, 0.0, 1.0, 1.0,     1.0, 1.0,   1.0, 0.0, 0.0,
            hwidth, -hheight, -hdepth,  0.0, 1.0, 1.0, 1.0,     0.0, 1.0,   1.0, 0.0, 0.0,
            hwidth, hheight, -hdepth,   0.5, 0.5, 1.0, 1.0,     0.0, 0.0,   1.0, 0.0, 0.0,
            hwidth, hheight, hdepth,    1.0, 1.0, 0.5, 1.0,     1.0, 0.0,   1.0, 0.0, 0.0,

            -hwidth, -hheight, hdepth,  1.0, 0.0, 1.0, 1.0,     1.0, 1.0,   0.0, 0.0, 1.0,
            hwidth, -hheight, hdepth,   1.0, 1.0, 0.5, 1.0,     0.0, 1.0,   0.0, 0.0, 1.0,
            hwidth, hheight, hdepth,    0.5, 0.5, 1.0, 1.0,     0.0, 0.0,   0.0, 0.0, 1.0,
            -hwidth, hheight, hdepth,   0.0, 1.0, 1.0, 1.0,     1.0, 0.0,   0.0, 0.0, 1.0,
            
            -hwidth, -hheight, hdepth,  1.0, 0.0, 1.0, 1.0,     0.0, 1.0,   -1.0, 0.0, 0.0,
            -hwidth, -hheight, -hdepth, 0.0, 1.0, 1.0, 1.0,     1.0, 1.0,   -1.0, 0.0, 0.0,
            -hwidth, hheight, -hdepth,  0.5, 0.5, 1.0, 1.0,     1.0, 0.0,   -1.0, 0.0, 0.0,
            -hwidth, hheight, hdepth,   1.0, 1.0, 0.5, 1.0,     0.0, 0.0,   -1.0, 0.0, 0.0,

            -hwidth, hheight, -hdepth,  1.0, 0.0, 0.0, 1.0,     0.0, 1.0,   0.0, 1.0, 0.0,
            hwidth, hheight, -hdepth,   0.0, 1.0, 0.0, 1.0,     1.0, 1.0,   0.0, 1.0, 0.0,
            hwidth, hheight, hdepth,    0.0, 0.0, 1.0, 1.0,     1.0, 0.0,   0.0, 1.0, 0.0,
            -hwidth, hheight, hdepth,   1.0, 1.0, 0.0, 1.0,     0.0, 0.0,   0.0, 1.0, 0.0,

            -hwidth, -hheight, -hdepth, 1.0, 0.0, 0.0, 1.0,     0.0, 1.0,   0.0, -1.0, 0.0,
            hwidth, -hheight, -hdepth,  0.0, 1.0, 0.0, 1.0,     1.0, 1.0,   0.0, -1.0, 0.0,
            hwidth, -hheight, hdepth,   0.0, 0.0, 1.0, 1.0,     1.0, 0.0,   0.0, -1.0, 0.0,
            -hwidth, -hheight, hdepth,  1.0, 1.0, 0.0, 1.0,     0.0, 0.0,   0.0, -1.0, 0.0,
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

        return new NormalMesh( gl, program, verts, indis, material, false );
    }

    /**
     * Create a flat platform in the xz plane.
     * @param {WebGLRenderingContext} gl 
     */
    static platform( gl, program, width, depth, uv_min, uv_max, material ) {
        let hwidth = width / 2;
        let hdepth = depth / 2;
        
        let verts = [
            -hwidth, 0, -hdepth,  1.0, 1.0, 1.0, 1.0,     uv_min, uv_max,   0.0, 1.0, 0.0,
            hwidth, 0, -hdepth,   1.0, 1.0, 1.0, 1.0,     uv_max, uv_max,   0.0, 1.0, 0.0,
            hwidth, 0, hdepth,    1.0, 1.0, 1.0, 1.0,     uv_max, uv_min,   0.0, 1.0, 0.0,
            -hwidth, 0, hdepth,   1.0, 1.0, 1.0, 1.0,     uv_min, uv_min,   0.0, 1.0, 0.0,
        ];

        let indis = [ 0, 1, 2, 2, 3, 0, ];

        return new NormalMesh( gl, program, verts, indis, material, false );
    }

    

    /**
     * Render the mesh. Does NOT preserve array/index buffer, program, or texture bindings! 
     * 
     * @param {WebGLRenderingContext} gl 
     */
    render( gl ) {
        // gl.enable( gl.CULL_FACE );
        
        gl.useProgram( this.program );
        this.set_vertex_attributes();
        gl.bindBuffer( gl.ARRAY_BUFFER, this.verts );
        gl.bindBuffer( gl.ELEMENT_ARRAY_BUFFER, this.indis );
        bind_texture_samplers( gl, this.program, "tex_0" );

        gl.activeTexture( gl.TEXTURE0 );
        this.material.bind( gl, this.program );

        set_uniform_int( gl, this.program, 'use_color', this.use_color );

        gl.drawElements( gl.TRIANGLES, this.n_indis, gl.UNSIGNED_SHORT, 0 );
    }

    /**
     * Create a UV sphere.
     * @param {*} gl 
     * @param {*} program 
     * @param {*} radius 
     * @param {*} subdivs the number of subdivisions, both vertically and radially
     * @param {*} material 
     * @returns 
     */
    static uv_sphere( gl, program, radius, subdivs, material ) {
        if( subdivs < 3 ) {
            throw new Error( "subdivs must be at least 3. value: " + subdivs );
        }

        let verts = []
        let indis = []

        for( let layer = 0; layer <= subdivs; layer++ ) {
            // let y = layer / subdivs - 0.5;
            let y_turns = layer /  subdivs / 2;
            let y = Math.cos( 2 * Math.PI * y_turns ) / 2;
            let radius_scale_for_layer = Math.sin( 2 * Math.PI * y_turns );

            for( let subdiv = 0; subdiv <= subdivs; subdiv++ ) {
                let turns = subdiv / subdivs; 
                let rads = 2 * Math.PI * turns;
    
                let x = Math.cos( rads ) / 2 * radius_scale_for_layer;
                let z = Math.sin( rads ) / 2 * radius_scale_for_layer;

                let point_norm = new Vec4( x, y, z, 0.0 ).norm();
                let scaled_point = point_norm.scaled( radius );

                // coordinates
                verts.push( scaled_point.x, scaled_point.y, scaled_point.z );

                // console.log( layer, subdiv, scaled_point.x, scaled_point.y, scaled_point.z );
                
                // color (we're making it white for simplicity)
                verts.push( 1, 1, 1, 1 );

                // uvs
                verts.push( subdiv / subdivs, layer / subdivs );
                
                // normal vector. make sure you understand why the normalized coordinate is 
                // equivalent to the normal vector for the sphere.
                verts.push( point_norm.x, point_norm.y, point_norm.z );
            }
        }

        function get_indi_no_from_layer_and_subdiv_no( layer, subdiv ) {
            let layer_start = layer * ( subdivs + 1 );
            return layer_start + subdiv % ( subdivs + 1 );
        }

        for( let layer = 1; layer <= subdivs; layer++ ) {
            for( let subdiv = 0; subdiv < subdivs; subdiv++ ) {
                let i0 = get_indi_no_from_layer_and_subdiv_no( layer - 1, subdiv );
                let i1 = get_indi_no_from_layer_and_subdiv_no( layer - 1, subdiv + 1 );
                let i2 = get_indi_no_from_layer_and_subdiv_no( layer, subdiv );
                let i3 = get_indi_no_from_layer_and_subdiv_no( layer, subdiv + 1 );

                indis.push( i0, i2, i3, i3, i1, i0 );
            }
        }

        return new NormalMesh( gl, program, verts, indis, material, false );
    }

    /**
     * turn heightmap into mesh
     * @param {WebGLRenderingContext} gl 
     * @param {WebGLProgram} program
     * @param {number][][]} map
     * @param {number} min 
     * @param {number} max
     */
    static from_heightmap(gl, program, map, min, max, material) {
        let rows = map.length;
        let cols = map[0].length;
        let off_x = cols/2;
        let off_z = rows/2;

        const MIN_HEIGHT_COLOR = 0.2;

        let verts = [];
        let indis = [];

        function color(height) {
            let normed_height = (height - min) / (max - min);
            return MIN_HEIGHT_COLOR + normed_height;
        }

        function push_verts(vert_list, vec, u, v, normal) {
            let height_color = color(vec.y);
            vert_list.push(
                vec.x, vec.y, vec.z,                            // position
                height_color, height_color, height_color, 1,     // brightness (function of height)
                u, v,                                           // uv cords
                normal.x, normal.y, normal.z                    // normal
                );
        }

        for (let row = 1; row<rows; row++) {
            for (let col = 1; col<cols; col++) {
                // position of each corner of quad
                let pos_tl = map[row-1][col-1];
                let pos_tr = map[row-1][col];
                let pos_bl = map[row][col-1];
                let pos_br = map[row][col];

                // vector for each cornor of quad
                let v_tl = new Vec4(-1, pos_tl, -1);
                let v_tr = new Vec4(0, pos_tr, -1);
                let v_bl = new Vec4(-1, pos_bl, 0);
                let v_br = new Vec4(0, pos_br, 0);

                // normal for wach triangle in quad
                let normal_t1 = Vec4.normal_of_triangle(v_tl, v_tr, v_bl);
                let normal_t2 = Vec4.normal_of_triangle(v_br, v_bl, v_tr);                

                // move x and z to correct position
                v_tl.x += col-off_x;
                v_tl.z += row-off_z;

                v_tr.x += col-off_x;
                v_tr.z += row-off_z;

                v_bl.x += col-off_x;
                v_bl.z += row-off_z;

                v_br.x += col-off_x;
                v_br.z += row-off_z;

                
                push_verts(verts, v_tl, 0, 1, normal_t1);
                push_verts(verts, v_tr, 1, 1, normal_t1);
                push_verts(verts, v_bl, 0, 0, normal_t1);

                push_verts(verts, v_br, 1, 0, normal_t2);
                push_verts(verts, v_bl, 0, 0, normal_t2);
                push_verts(verts, v_tr, 1, 1, normal_t2);
                
                /*
                // TODO: fix this so maps top left as 0.0 and bottom right as 1,1
                push_verts(verts, v_tl, col/cols, (row+1)/rows, normal_t1);
                push_verts(verts, v_tr, (col+1)/cols, (row+1)/rows, normal_t1);
                push_verts(verts, v_bl, col/cols, row/rows, normal_t1);

                push_verts(verts, v_br, (col+1)/cols, row/rows, normal_t2);
                push_verts(verts, v_bl, col/cols, row/rows, normal_t2);
                push_verts(verts, v_tr, (col+1)/cols, (row+1)/rows, normal_t2);
                */

                let indi_start = indis.length;
                for (let i = 0; i<6; i++) {
                    indis.push(indi_start+i);
                }
            }
        }

        return new NormalMesh(gl, program, verts, indis, material, true);
    }

    

    static fractal_map(gl, program, scale, roughness, min, max, center, material) {
        let len = 2 ** scale + 1;
        console.log(len);
        let map = new Array(len).fill(0).map(() => new Array(len).fill(null));
        console.log(map.length, map[0].length)

        map[0][0] = Math.floor(rand_range(min, max));
        map[0][len-1] = Math.floor(rand_range(min, max));
        map[len-1][0] = Math.floor(rand_range(min, max));
        map[len-1][len-1]= Math.floor(rand_range(min, max));

        /*
        function diampnd_square(map, scale, min, max, center) {
            let block = 2**scale;
            offset = rand_range(min, max);

            while (block > 1) {
                map = square(map, block, offset);
                diamond(map, block, offset);

                block = block / 2;
                offset = offset / 2;
            }

            return map;
        } */

        function square(map, block, offset) {
            let sum = 0;
            for (let i=0; i<map.length; i+=block) {
                for (let j=0; j<map.length; j+=block) {
                    let bl = null;
                    let br = null
                    if (block+j < map.length) {
                        br = map[j+block][i+block];
                        bl = map[j+block][i];
                    }
                    let tr = map[j][i+block];
                    let tl = map[j][i];
                    let count = 0;
                    if (block+j < map.length) {
                        count = 4;
                        sum += tl+tr+bl+br;
                    } else {
                        count = 2;
                        sum += tr+tl
                    }
                    const row = (j+block)/2;
                    const col = (i+block)/2;
                    console.log(row, col);
                    map[row][col] = sum / count + Math.floor(rand_range(-offset, offset));
                }
            }
            return map;
        }

        function diamond(map, block, offset) {
            let mid = block/2;
            for (let i=0; i<map.length; i+=mid) {
                for (let j = (i+mid)%block; j<map.length; j+=block) {

                    let left = map[i][j-mid];
                    let right = map[i][j+mid];

                    let bottom = null;
                    if ((i+mid) < map.length) {
                        bottom = map[i+mid][j];
                    } 
                    let top = null;
                    if ((i-mid) > 0) {
                        top = map[i-mid][j];
                    } 

                    let count = 2;
                    let sum = left + right;
                    if (bottom != null) {
                        count++;
                        sum += bottom
                    }
                    if (top != null) {
                        count++;
                        sum += null;
                    }
                    map[i][j] = sum/count + rand_range(-offset, offset);
                }
            }
            return map;
        }

        let block = len - 1;
        console.log(block);
        let offset = rand_range(min, max);

        while (block > 1) {
            map = square(map, block, offset);
            map = diamond(map, block, offset);

            block = block / 2;
            offset = offset / 2;
        }

        return NormalMesh.from_heightmap(gl, program, map, min, max, material)

    }

    static fractal_map_test(init_range, scale) {
        const MATRIX_LENGTH = scale ** 2 + 1;
        const RANDOM_INITIAL_RANGE = init_range;
        function randomInRange(min, max) {
            return Math.floor(Math.random() * (max - min + 1) + min);
        }

        function generateeMatrix() {
            const matrix = new Array(MATRIX_LENGTH)
            .fill(0)
            .map(() => new Array(MATRIX_LENGTH).fill(null));

            matrix[0][MATRIX_LENGTH - 1] = randomInRange(0, RANDOM_INITIAL_RANGE);
            matrix[MATRIX_LENGTH - 1][0] = randomInRange(0, RANDOM_INITIAL_RANGE);
            matrix[0][0] = randomInRange(0, RANDOM_INITIAL_RANGE);
            matrix[MATRIX_LENGTH - 1][MATRIX_LENGTH - 1] = randomInRange(
                0,
                RANDOM_INITIAL_RANGE
            );

            return matrix;
        }


        function diamondSquare(matrix) {
            let chunkSize = MATRIX_LENGTH - 1;
            let randomFactor = RANDOM_INITIAL_RANGE;

            while (chunkSize > 1) {
                calculateSquare(matrix, chunkSize, randomFactor)

                calculateDiamond(matrix, chunkSize, randomFactor)

                chunkSize /= 2;
                randomFactor /= 2;
          }

          return matrix;
        }

        function calculateDiamond(matrix, chunkSize, randomFactor) {
          let sumComponents = 0;
          let sum = 0;
          for (let i = 0; i < matrix.length - 1; i += chunkSize) {
            for (let j = 0; j < matrix.length - 1; j += chunkSize) {
              const BOTTOM_RIGHT = matrix[j + chunkSize]
                ? matrix[j + chunkSize][i + chunkSize]
                : null;
              const BOTTOM_LEFT = matrix[j + chunkSize]
                ? matrix[j + chunkSize][i]
                : null;
              const TOP_LEFT = matrix[j][i];
              const TOP_RIGHT = matrix[j][i + chunkSize];
              const { count, sum } = [
                BOTTOM_RIGHT,
                BOTTOM_LEFT,
                TOP_LEFT,
                TOP_RIGHT
              ].reduce(
                (result, value) => {
                  if (isFinite(value) && value != null) {
                    result.sum += value;
                    result.count += 1;
                  }
                  return result;
                },
                { sum: 0, count: 0 }
              );
              const changed = {row: j + chunkSize / 2, column: i + chunkSize / 2};
              matrix[changed.row][changed.column] =
                sum / count + randomInRange(-randomFactor, randomFactor);
            }
          }
          return matrix;
        }

        function calculateSquare(matrix, chunkSize, randomFactor) {
          const half = chunkSize / 2;
          for (let y = 0; y < matrix.length; y += half) {
            for (let x = (y + half) % chunkSize; x < matrix.length; x += chunkSize) {
              const BOTTOM = matrix[y + half] ? matrix[y + half][x] : null;
              const LEFT = matrix[y][x - half];
              const TOP = matrix[y - half] ? matrix[y - half][x] : null;
              const RIGHT = matrix[y][x + half];
              const { count, sum } = [BOTTOM, LEFT, TOP, RIGHT].reduce(
                (result, value) => {
                  if (isFinite(value) && value != null) {
                    result.sum += value;
                    result.count += 1;
                  }
                  return result;
                },
                { sum: 0, count: 0 }
              );
              matrix[y][x] = sum / count + randomInRange(-randomFactor, randomFactor);
            }
          }
          return matrix;
        }

        function calculateSquare(matrix, chunkSize, randomFactor) {
          const half = chunkSize / 2;
          for (let y = 0; y < matrix.length; y += half) {
            for (let x = (y + half) % chunkSize; x < matrix.length; x += chunkSize) {
              const BOTTOM = matrix[y + half] ? matrix[y + half][x] : null;
              const LEFT = matrix[y][x - half];
              const TOP = matrix[y - half] ? matrix[y - half][x] : null;
              const RIGHT = matrix[y][x + half];
              const { count, sum } = [BOTTOM, LEFT, TOP, RIGHT].reduce(
                (result, value) => {
                  if (isFinite(value) && value != null) {
                    result.sum += value;
                    result.count += 1;
                  }
                  return result;
                },
                { sum: 0, count: 0 }
              );
              matrix[y][x] = sum / count + randomInRange(-randomFactor, randomFactor);
            }
          }
          return matrix;
        }

        function normalizeMatrix(matrix) {
          const maxValue = matrix.reduce((max, row) => {
            return row.reduce((max, value) => Math.max(value, max));
          }, -Infinity);

          return matrix.map((row) => {
            return row.map((val) => val / maxValue);
          });
        }

        let map = generateeMatrix();
        map = diamondSquare(map);
        normalizeMatrix(map);
        console.log(map);
        console.log(map.length);
        return map;
    }
}