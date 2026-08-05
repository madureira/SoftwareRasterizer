addToLibrary({
  platform_js_get_window_width: () => window.innerWidth,

  platform_js_get_window_height: () => window.innerHeight,

  platform_js_set_title: (titlePtr) => {
    document.title = UTF8ToString(titlePtr);
  },

  $platform_canvas: null,

  platform_js_init_canvas__deps: ['$platform_canvas'],
  platform_js_init_canvas: (idPtr) => {
    platform_canvas = document.getElementById(UTF8ToString(idPtr));
  },

  platform_js_request_fullscreen__deps: ['$platform_canvas'],
  platform_js_request_fullscreen: () => {
    const handler = () => {
      platform_canvas.requestFullscreen().catch(() => {});
      platform_canvas.removeEventListener('click', handler);
    };
    platform_canvas.addEventListener('click', handler);
  },

  platform_js_present__deps: ['$platform_canvas'],
  platform_js_present: (pixelsPtr, width, height) => {
    const ctx = platform_canvas.getContext('2d');
    const imgData = ctx.createImageData(width, height);
    const dst = imgData.data;
    const src = pixelsPtr >>> 2;
    const n = width * height;
    for (let i = 0; i < n; i++) {
      const px = HEAPU32[src + i];
      const idx = i << 2;
      dst[idx] = (px >> 16) & 0xFF;
      dst[idx + 1] = (px >> 8) & 0xFF;
      dst[idx + 2] = px & 0xFF;
      dst[idx + 3] = 255;
    }
    ctx.putImageData(imgData, 0, 0);
  }
});
