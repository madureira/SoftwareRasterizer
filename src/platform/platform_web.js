addToLibrary({
  platform_js_get_window_width: () => window.innerWidth,

  platform_js_get_window_height: () => window.innerHeight,

  platform_js_set_title: (titlePtr) => {
    document.title = UTF8ToString(titlePtr);
  },

  platform_js_present: (pixelsPtr, width, height) => {
    const canvas = document.getElementById('canvas');
    const ctx = canvas.getContext('2d');
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
