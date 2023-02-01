class Texture {

	contructor (gl, tex_source, blend_mode) {
		this.tex = gl.createTexture();
		this.blend_mode = blend_mode;
		this.loaded = false;

		const old_tex_binding = gl.getPramater(gl.TEXTURE_BINDING_2D);
		this.bind(gl);

		gl.texImage2D(
			gl.TEXTURE_2d, 0, gl.RGBA,
			256, 256, 0,
			gl.RGBA, gl.UNSIGNED_BYTE,
			Material.xor_tex(256)
		);
		gl.generateMipmap(gl.TEXTURE_2D);

		if (old_tex_binding === null) {
			gl.bindTexture(gl.TEXTURE_2D, old_tex_binding);
		}

		// end now to not load beyond debug texture
		if (tex_source == 'xor') {
			return;
		}

		let image = new Image();
		let _tex = this;

		// listin for tex to be loaded
		image.addEventListener( 'load', function() {

			const old_tex_binding = gl.getPramater(gl.TEXTURE_BINDING_2D);
			_tex.bind(gl);

			gl.texImage2D (
				gl.TEXTURE_2D, 0, gl.RGBA,
				gl.RGBA, gl.UNSIGNED_BYTE, image
			);

			_tex.width = image.width;
			_tex.height = image.height;

			gl.generateMipmap(gl.TEXTURE_2D);

			let err = gl.getError();
			if (err != 0) {
				gl.getError();
				throw new Error( 'Error generating mipmap: ' + err );
			}

			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, blend_mode);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

			err = gl.getError();
            if( err != 0 ) {
                gl.getError(); //clear potential 2nd error.
                throw new Error( 'Error setting texture parameters: ' + err );
            }

            //console.log('loaded texture: ',image.src);

            _tex.loaded = true;

            if( old_tex_binding === null ) {
                gl.bindTexture( gl.TEXTURE_2D, old_tex_binding );
            }
		});

		image.src = tex_source;
	}

	bind( gl ) {
        gl.bindTexture( gl.TEXTURE_2D, this.tex );
    }

	 /**
	 * generates the xor texture of size width * width
	 * @param {int} width
	 * @return {Unit8Array} texture
	 */
    static xor_texture(width) {
	    //const width = 256;
	    let data = new Array(width * width * 4);

	    for(let row = 0; row < width; row++) {
	        for(let col = 0; col < width; col++) {
	            let pix = (row * width + col) * 4;
	            data[pix] = data[pix+1] = data[pix+2] = row ^ col;
	            //data[pix] = 1.0;
	            //data[pix+1] = data[pix+2] = 0.0;
	            data[pix+3] = 255;
	        }
	    }
	    //console.log(data);
	    return new Uint8Array(data);
	}

}