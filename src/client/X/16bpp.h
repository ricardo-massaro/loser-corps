/* 16bpp.h */


void draw_pixel_16(int x, int y, int color)
{
  ((unsigned short *) (img->data + img->bytes_per_line * y))[x] = color;
}

void draw_pixel_16_dbl(int x, int y, int color)
{
  ((unsigned short *) tmp->line[y])[x] = color;
}

void draw_bmp_line_16(XBITMAP *bmp, int bx, int by, int x, int y, int w)
{
  int2bpp *s, *d;

  s = ((int2bpp *) bmp->line[by]) + bx;
  d = ((int2bpp *) (img->data + img->bytes_per_line * y)) + x;
  memcpy(d, s, w + w);
}

void draw_bmp_line_16_dbl(XBITMAP *bmp, int bx, int by, int x, int y, int w)
{
  int2bpp *s, *d;

  s = ((int2bpp *) bmp->line[by]) + bx;
  d = ((int2bpp *) tmp->line[y]) + x;
  memcpy(d, s, w + w);
}


#define blink16(dest, src) (*(dest) = MAKECOLOR16_real(255,255,255))

static INLINE void darken16(unsigned short *dest, int c)
{
  unsigned char r, g, b, x, y, z;

  r = *dest >> 11;
  g = (*dest >> 5) & 0x3f;
  b = *dest & 0x1f;
  x = c >> 11;
  y = (c >> 5) & 0x3f;
  z = c & 0x1f;
  if (r > x)
     r = x;
  if (g > y)
     g = y;
  if (b > z)
     b = z;
  *dest = (r << 11) | (g << 5) | b;
}

static INLINE void lighten16(unsigned short *dest, int c)
{
  unsigned char r, g, b, tmp;

  r = *dest >> 11;
  g = (*dest >> 5) & 0x3f;
  b = *dest & 0x1f;
  if (r < (tmp = c >> 11))
     r = tmp;
  if (g < (tmp = (c >> 5) & 0x3f))
     g = tmp;
  if (b < (tmp = c & 0x1f))
     b = tmp;
  *dest = (r << 11) | (g << 5) | b;
}


static INLINE void alpha16_25(unsigned short *dest, int c)
{
  unsigned char r, g, b;

  r = *dest >> 11;
  r = ((r<<1)+r + (c >> 11)) >> 2;
  g = (*dest >> 5) & 0x3f;
  g = ((g<<1)+g + ((c >> 5) & 0x3f)) >> 2;
  b = (*dest & 0x1f);
  b = ((b<<1)+b + (c & 0x1f)) >> 2;
  *dest = (r << 11) | (g << 5) | b;
}

static INLINE void alpha16_50(unsigned short *dest, int c)
{
  unsigned char r, g, b;
  r = ((*dest >> 11) + (c >> 11)) >> 1;
  g = (((*dest >> 5) & 0x3f) + ((c >> 5) & 0x3f)) >> 1;
  b = ((*dest & 0x1f) + (c & 0x1f)) >> 1;
  *dest = (r << 11) | (g << 5) | b;
}

static INLINE void alpha16_75(unsigned short *dest, int c)
{
  unsigned char r, g, b;

  r = c >> 11;
  r = ((*dest >> 11) + (r<<1)+r) >> 2;
  g = (c >> 5) & 0x3f;
  g = (((*dest >> 5) & 0x3f) + (g<<1)+g) >> 2;
  b = c & 0x1f;
  b = ((*dest & 0x1f) + (b<<1)+b) >> 2;
  *dest = (r << 11) | (g << 5) | b;
}



void draw_bitmap_16(XBITMAP *bmp, int x, int y)
{
  unsigned short *src, *dest;
  int i;

  src = (unsigned short *) bmp->data;
  dest = ((unsigned short *)
	  (((unsigned char *) img->data) + img->bytes_per_line * y) + x);
  for (i = bmp->h; i > 0; i--) {
    single_memcpy(dest, src, bmp->line_w);
    src += bmp->w;
    dest += img->bytes_per_line/2;
  }
}

void draw_bitmap_16_dbl(XBITMAP *bmp, int x, int y)
{
  unsigned short *src, *dest;
  int i;

  src = (unsigned short *) bmp->data;
  dest = ((unsigned short *) tmp->line[y]) + x;
  for (i = bmp->h; i > 0; i--) {
    single_memcpy(dest, src, bmp->line_w);
    src += bmp->w;
    dest += tmp->w;
  }
}


#define def_func16(func)						  \
void draw_bitmap_##func##_16(XBITMAP *bmp, int x, int y)		  \
{									  \
  unsigned short *src, *dest;						  \
  int w, i, j;								  \
									  \
  w = bmp->w;								  \
  src = (unsigned short *) bmp->data;					  \
  dest = ((unsigned short *)						  \
	  (((unsigned char *) img->data) + img->bytes_per_line * y) + x); \
  for (i = bmp->h; i > 0; i--) {					  \
    for (j = w; j > 0; j--)						  \
      func(dest++, *src++);						  \
    dest += img->bytes_per_line/2 - w;					  \
  }									  \
}

def_func16(darken16)
def_func16(lighten16)
def_func16(alpha16_25)
def_func16(alpha16_50)
def_func16(alpha16_75)


#define def_func16_dbl(func)					\
void draw_bitmap_##func##_16_dbl(XBITMAP *bmp, int x, int y)	\
{								\
  unsigned short *src, *dest;					\
  int i, j, w;							\
								\
  w = bmp->w;							\
  src = (unsigned short *) bmp->data;				\
  dest = ((unsigned short *) tmp->line[y]) + x;			\
  for (i = bmp->h; i > 0; i--) {				\
    for (j = w; j > 0; j--)					\
      func(dest++, *src++);                                     \
    dest += tmp->w - w;						\
  }								\
}

def_func16_dbl(darken16)
def_func16_dbl(lighten16)
def_func16_dbl(alpha16_25)
def_func16_dbl(alpha16_50)
def_func16_dbl(alpha16_75)


void draw_bitmap_tr_displace_16(XBITMAP *bmp, int x, int y)
{
  int i, j, w, h;
  unsigned short *src, *dest;

  w = bmp->w;
  h = bmp->tr_last;
  src = (unsigned short *) bmp->line[bmp->tr_first];
  dest = (((unsigned short *)
	   (((unsigned char *) img->data) + (img->bytes_per_line
					     * (bmp->tr_first+y))) + x));
  for (i = bmp->tr_first; i <= h; i++) {
    for (j = w; j > 0; j--) {
      if (*src != XBMP16_TRANSP)
	*dest = *(dest + (j & 4) + ((j & 2) ? (img->bytes_per_line >> 1) : 0));
      src++;
      dest++;
    }
    dest += (img->bytes_per_line >> 1) - w;
  }
}

void draw_bitmap_tr_16(XBITMAP *bmp, int x, int y)
{
  int i, j, w, h;
  unsigned short *src, *dest;

  w = bmp->w;
  h = bmp->tr_last;
  src = (unsigned short *) bmp->line[bmp->tr_first];
  dest = (((unsigned short *)
	   (((unsigned char *) img->data) + (img->bytes_per_line
					     * (bmp->tr_first+y))) + x));
  for (i = bmp->tr_first; i <= h; i++) {
    for (j = w; j > 0; j--) {
      if (*src != XBMP16_TRANSP)
	*dest = *src;
      src++;
      dest++;
    }
    dest += (img->bytes_per_line >> 1) - w;
  }
}

void draw_bitmap_tr_displace_16_dbl(XBITMAP *bmp, int x, int y)
{
  int i, j, w, h;
  unsigned short *src, *dest;

#if 0
  printf("height = %d, (first,last) = (%d,%d)\n",
	 bmp->h, bmp->tr_first, bmp->tr_last);
#endif

  w = bmp->w;
  h = bmp->tr_last;
  src = (unsigned short *) bmp->line[bmp->tr_first];
  dest = ((unsigned short *) tmp->line[y + bmp->tr_first]) + x;
  for (i = bmp->tr_first; i <= h; i++) {
    for (j = w; j > 0; j--) {
      if (*src != XBMP16_TRANSP)
	*dest = *(dest + (j & 4) + ((j & 2) ? tmp->w : 0));
      src++;
      dest++;
    }
    dest += tmp->w - w;
  }
}

void draw_bitmap_tr_16_dbl(XBITMAP *bmp, int x, int y)
{
  int i, j, w, h;
  unsigned short *src, *dest;

#if 0
  printf("height = %d, (first,last) = (%d,%d)\n",
	 bmp->h, bmp->tr_first, bmp->tr_last);
#endif

  w = bmp->w;
  h = bmp->tr_last;
  src = (unsigned short *) bmp->line[bmp->tr_first];
  dest = ((unsigned short *) tmp->line[y + bmp->tr_first]) + x;
  for (i = bmp->tr_first; i <= h; i++) {
    for (j = w; j > 0; j--) {
      if (*src != XBMP16_TRANSP)
	*dest = *src;
      src++;
      dest++;
    }
    dest += tmp->w - w;
  }
}

#define def_func16_tr_dbl(func)						\
void draw_bitmap_tr_##func##_16_dbl(XBITMAP *bmp, int x, int y)	        \
{									\
  int i, j, w;								\
  unsigned short *src, *dest;						\
									\
  w = bmp->w;								\
  src = (unsigned short *) bmp->data;					\
  dest = ((unsigned short *) tmp->line[y]) + x;				\
  for (i = bmp->h; i > 0; i--) {					\
    for (j = w; j > 0; j--) {						\
      if (*src != XBMP16_TRANSP)					\
          func(dest, *src);      					\
      dest++;								\
      src++;								\
    }									\
    dest += tmp->w - w;							\
  }									\
}

def_func16_tr_dbl(darken16)
def_func16_tr_dbl(lighten16)
def_func16_tr_dbl(alpha16_25)
def_func16_tr_dbl(alpha16_50)
def_func16_tr_dbl(alpha16_75)
def_func16_tr_dbl(blink16)

#define def_func16_tr(func)						    \
void draw_bitmap_tr_##func##_16(XBITMAP *bmp, int x, int y)		    \
{									    \
  int i, j, w;								    \
  unsigned short *src, *dest;						    \
									    \
  w = bmp->w;								    \
  src = (unsigned short *) bmp->data;					    \
  for (i = 0; i < bmp->h; i++) {					    \
    dest = (((unsigned short *)						    \
	     (((unsigned char *) img->data) + img->bytes_per_line * (i+y))) \
	    + x);							    \
    for (j = w; j > 0; j--) {						    \
      if (*src != XBMP16_TRANSP)					    \
	func(dest, *src);						    \
      src++;								    \
      dest++;								    \
    }									    \
  }									    \
}

def_func16_tr(darken16)
def_func16_tr(lighten16)
def_func16_tr(alpha16_25)
def_func16_tr(alpha16_50)
def_func16_tr(alpha16_75)
def_func16_tr(blink16)


void draw_energy_bar_16(int x, int y, int w, int h, int energy, int color)
{
  int i, scr_w;
  unsigned short *dest;

  if (energy <= 0) {
    gr_printf(x + 5, y + (h - 8) / 2 + 1, font_8x8, "RIP");
    return;
  }
  if (energy > 100)
    energy = 100;
  energy = (w * energy) / 100;

  scr_w = img->bytes_per_line / 2;
  dest = ((unsigned short *) img->data) + scr_w * y + x;
  for (i = h; i > 0; i--) {
    int j, r, g, b;

    r = (color >> 11) << 3;
    g = ((color >> 5) & 0x3f) << 2;
    b = (color & 0x1f) << 3;
    r -= 2 * w;
    g -= 2 * w;
    b -= 2 * w;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    dest++;
    for (j = 0; j < energy; j++) {
      *dest++ = MAKECOLOR16_real(r, g, b);
      if ((r += 2) > 255) r = 255;
      if ((g += 2) > 255) g = 255;
      if ((b += 2) > 255) b = 255;
    }
    for (; j < w; j++)
      dest++;
    dest += scr_w - (w+1);
  }
}

void draw_energy_bar_16_dbl(int x, int y, int w, int h, int energy, int color)
{
  int i, scr_w;
  unsigned short *dest;

  if (energy <= 0) {
    gr_printf(x + 1, y + (h - 8) / 2, font_8x8, "RIP");
    return;
  }
  if (energy > 100)
    energy = 100;
  energy = (energy * w) / 100;
  scr_w = tmp->w;
  dest = ((unsigned short *) tmp->line[y]) + x;
  for (i = h; i > 0; i--) {
    int j, r, g, b;

    r = (color >> 11) << 3;
    g = ((color >> 5) & 0x3f) << 2;
    b = (color & 0x1f) << 3;
    r -= 2 * w;
    g -= 2 * w;
    b -= 2 * w;
    
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    dest++;
    for (j = 0; j < energy; j++) {
      *dest++ = MAKECOLOR16_real(r, g, b);
      if ((r += 2) > 255) r = 255;
      if ((g += 2) > 255) g = 255;
      if ((b += 2) > 255) b = 255;
    }
    for (; j < w; j++)
      dest++;
    dest += scr_w - (w+1);
  }
}


void use_16bpp_dbl(void)
{
  draw_pixel = draw_pixel_16_dbl;
  draw_bmp_line = draw_bmp_line_16_dbl;
  draw_bitmap = draw_bitmap_16_dbl;
  draw_bitmap_darken = draw_bitmap_darken16_16_dbl;
  draw_bitmap_lighten = draw_bitmap_lighten16_16_dbl;
  draw_bitmap_alpha25 = draw_bitmap_alpha16_25_16_dbl;
  draw_bitmap_alpha50 = draw_bitmap_alpha16_50_16_dbl;
  draw_bitmap_alpha75 = draw_bitmap_alpha16_75_16_dbl;
  draw_bitmap_tr = draw_bitmap_tr_16_dbl;
  draw_bitmap_tr_blink = draw_bitmap_tr_blink16_16_dbl;
  draw_bitmap_tr_displace = draw_bitmap_tr_displace_16_dbl;
  draw_bitmap_tr_darken = draw_bitmap_tr_darken16_16_dbl;
  draw_bitmap_tr_lighten = draw_bitmap_tr_lighten16_16_dbl;
  draw_bitmap_tr_alpha25 = draw_bitmap_tr_alpha16_25_16_dbl;
  draw_bitmap_tr_alpha50 = draw_bitmap_tr_alpha16_50_16_dbl;
  draw_bitmap_tr_alpha75 = draw_bitmap_tr_alpha16_75_16_dbl;
  draw_energy_bar = draw_energy_bar_16_dbl;
}

void use_16bpp(void)
{
  draw_pixel = draw_pixel_16;
  draw_bmp_line = draw_bmp_line_16;
  draw_bitmap = draw_bitmap_16;
  draw_bitmap_darken = draw_bitmap_darken16_16;
  draw_bitmap_lighten = draw_bitmap_lighten16_16;
  draw_bitmap_alpha25 = draw_bitmap_alpha16_25_16;
  draw_bitmap_alpha50 = draw_bitmap_alpha16_50_16;
  draw_bitmap_alpha75 = draw_bitmap_alpha16_75_16;
  draw_bitmap_tr = draw_bitmap_tr_16;
  draw_bitmap_tr_blink = draw_bitmap_tr_blink16_16;
  draw_bitmap_tr_displace = draw_bitmap_tr_displace_16;
  draw_bitmap_tr_darken = draw_bitmap_tr_darken16_16;
  draw_bitmap_tr_lighten = draw_bitmap_tr_lighten16_16;
  draw_bitmap_tr_alpha25 = draw_bitmap_tr_alpha16_25_16;
  draw_bitmap_tr_alpha50 = draw_bitmap_tr_alpha16_50_16;
  draw_bitmap_tr_alpha75 = draw_bitmap_tr_alpha16_75_16;
  draw_energy_bar = draw_energy_bar_16;
}

