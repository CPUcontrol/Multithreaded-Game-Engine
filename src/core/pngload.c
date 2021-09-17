#include <png.h>

#include "instream.h"
#include "pngload.h"

static void user_read_data(
                            png_structp png_ptr,
                            png_bytep data,
                            png_size_t length){

    Enj_Instream *rs = (Enj_Instream *)png_get_io_ptr(png_ptr);
    Enj_ReadBytes(rs, data, length);
}

int Enj_OpenPNG(Enj_PNGLoader *p, Enj_Instream *rs){
    unsigned char sig[8] = {0,0,0,0,0,0,0,0};
    if(Enj_ReadBytes(rs, sig, 8) < 8) return 1;

    int ispng = !png_sig_cmp(sig, 0, 8);
    if(!ispng) return 1;



    p->png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!p->png_ptr) return 2;

    p->info_ptr = png_create_info_struct(p->png_ptr);
    if (!p->info_ptr){
        png_destroy_read_struct(
            (png_structpp)p->png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return 2;
    }

    png_set_read_fn((png_structp)p->png_ptr, rs, (png_rw_ptr)user_read_data);
    png_set_sig_bytes((png_structp)p->png_ptr, 8);

    png_read_info((png_structp)p->png_ptr, (png_infop)p->info_ptr);

    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int bit_depth = 0;
    int color_type = -1;
    png_uint_32 retval = png_get_IHDR(
        (png_structp)p->png_ptr, (png_infop)p->info_ptr,
        &width,
        &height,
        &bit_depth,
        &color_type,
        NULL, NULL, NULL);
    p->width = width;
    p->height = height;
    p->bit_depth = bit_depth;
    p->color_type = color_type;


    return 0;
}
int Enj_ReadPNG(Enj_PNGLoader *p, void *buf, size_t size, size_t pitch){
    //if pitch is zero, assume default with no padding
    pitch = pitch ? pitch : 4 * p->width;

    if(size < p->height * pitch) return 1;

    if (p->color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb((png_structp)p->png_ptr);

    int has_tRNS = png_get_valid((png_structp)p->png_ptr,
        (png_infop)p->info_ptr,
        PNG_INFO_tRNS);
    if (has_tRNS)
        png_set_tRNS_to_alpha((png_structp)p->png_ptr);

    if (p->bit_depth == 16)
        png_set_strip_16((png_structp)p->png_ptr);

    if (p->color_type == PNG_COLOR_TYPE_GRAY ||
        p->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb((png_structp)p->png_ptr);

    if (!has_tRNS)
        png_set_filler ((png_structp)p->png_ptr, 255, PNG_FILLER_AFTER);

    png_color_16p image_background;

    if (png_get_bKGD((png_structp)p->png_ptr,
    (png_infop)p->info_ptr, &image_background))
        png_set_background((png_structp)p->png_ptr, image_background,
          PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);

    for(unsigned int i = 0; i < p->height; ++i){
        png_read_row(
            (png_structp)p->png_ptr, (png_bytep)buf + i*pitch, NULL
            );
    }
    png_read_end((png_structp)p->png_ptr, NULL);

    return 0;
}

void Enj_ClosePNG(Enj_PNGLoader *p){
    png_destroy_read_struct(
        (png_structpp)p->png_ptr, (png_infopp)p->info_ptr, (png_infopp)NULL);
}
