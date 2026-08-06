addToLibrary({
  platform_js_get_window_width: () => window.innerWidth,

  platform_js_get_window_height: () => window.innerHeight,

  platform_js_set_title: (titlePtr) => {
    document.title = UTF8ToString(titlePtr);
  },

  $platform_canvas: null,
  $platform_gl: null,
  $platform_prog: null,
  $platform_vbuf: null,
  $platform_tex: null,
  $platform_posLoc: 0,
  $platform_tex_w: 0,
  $platform_tex_h: 0,

  platform_js_init_canvas__deps: ['$platform_canvas', '$platform_gl', '$platform_prog',
    '$platform_vbuf', '$platform_tex', '$platform_posLoc',
    '$platform_tex_w', '$platform_tex_h'],
  platform_js_init_canvas: (idPtr) => {
    platform_canvas = document.getElementById(UTF8ToString(idPtr));
    const gl = platform_canvas.getContext('webgl2') ||
      platform_canvas.getContext('webgl') ||
      platform_canvas.getContext('experimental-webgl');
    platform_gl = gl;

    const mkShader = (type, src) => {
      const s = gl.createShader(type);
      gl.shaderSource(s, src);
      gl.compileShader(s);
      return s;
    };

    const vs = mkShader(gl.VERTEX_SHADER,
      'attribute vec2 a;varying vec2 v;' +
      'void main(){v=vec2(a.x*.5+.5,.5-a.y*.5);gl_Position=vec4(a,0,1);}');

    const fs = mkShader(gl.FRAGMENT_SHADER,
      'precision mediump float;uniform sampler2D t;varying vec2 v;' +
      'void main(){vec4 c=texture2D(t,v);gl_FragColor=vec4(c.b,c.g,c.r,1.);}');

    platform_prog = gl.createProgram();
    gl.attachShader(platform_prog, vs);
    gl.attachShader(platform_prog, fs);
    gl.linkProgram(platform_prog);
    gl.useProgram(platform_prog);

    platform_vbuf = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, platform_vbuf);
    gl.bufferData(gl.ARRAY_BUFFER,
      new Float32Array([-1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1]),
      gl.STATIC_DRAW);

    platform_posLoc = gl.getAttribLocation(platform_prog, 'a');
    gl.enableVertexAttribArray(platform_posLoc);
    gl.vertexAttribPointer(platform_posLoc, 2, gl.FLOAT, false, 0, 0);

    platform_tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, platform_tex);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

    platform_tex_w = 0;
    platform_tex_h = 0;
  },

  platform_js_request_fullscreen__deps: ['$platform_canvas'],
  platform_js_request_fullscreen: () => {
    const handler = () => {
      platform_canvas.requestFullscreen().catch(() => { });
      platform_canvas.removeEventListener('click', handler);
    };
    platform_canvas.addEventListener('click', handler);
  },

  platform_js_present__deps: ['$platform_canvas', '$platform_gl', '$platform_prog',
    '$platform_vbuf', '$platform_tex', '$platform_posLoc',
    '$platform_tex_w', '$platform_tex_h'],
  platform_js_present: (pixelsPtr, width, height) => {
    const gl = platform_gl;

    if (platform_tex_w !== width || platform_tex_h !== height) {
      platform_canvas.width = width;
      platform_canvas.height = height;
      gl.useProgram(platform_prog);
      gl.bindBuffer(gl.ARRAY_BUFFER, platform_vbuf);
      gl.enableVertexAttribArray(platform_posLoc);
      gl.vertexAttribPointer(platform_posLoc, 2, gl.FLOAT, false, 0, 0);
      gl.bindTexture(gl.TEXTURE_2D, platform_tex);
      gl.viewport(0, 0, width, height);
      gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0,
        gl.RGBA, gl.UNSIGNED_BYTE, null);
      platform_tex_w = width;
      platform_tex_h = height;
    }

    gl.bindTexture(gl.TEXTURE_2D, platform_tex);

    gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, width, height,
      gl.RGBA, gl.UNSIGNED_BYTE,
      new Uint8Array(HEAPU8.buffer, pixelsPtr, width * height * 4));
    gl.drawArrays(gl.TRIANGLES, 0, 6);
  }
});
