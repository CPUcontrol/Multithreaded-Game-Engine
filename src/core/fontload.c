#include <ft2build.h>
#include FT_FREETYPE_H

#include "fontload.h"
#include "instream.h"

typedef struct freetypedata{
    FT_Library lib;
    FT_Face face;
} freetypedata;


int Enj_OpenFont(Enj_FontLoader *d, Enj_Instream *is){
    d->ft_ptr = malloc(sizeof(freetypedata));
    if(!d->ft_ptr) return 1;

    freetypedata *ft = (freetypedata *)d->ft_ptr;

    if(FT_Init_FreeType(&ft->lib)){
        free(d->ft_ptr);
        return 2;
    }
    size_t sz = 0;
    size_t cap = 1<<20;
    void *buf = malloc(cap);
    if(!buf){
        FT_Done_FreeType(ft->lib);
        free(d->ft_ptr);
        return 3;
    }
    sz += Enj_ReadBytes(is, buf, cap);

    while(sz == cap){
        cap *= 2;
        void *newbuf = realloc(buf, cap);
        if(!newbuf){
            free(buf);
            FT_Done_FreeType(ft->lib);
            free(d->ft_ptr);
            return 3;
        }
        else buf = newbuf;

        sz += Enj_ReadBytes(is, (char *)buf + sz, cap-sz);
    }

    if(FT_New_Memory_Face(ft->lib, buf, (FT_Long)sz, 0, &ft->face)){
        free(buf);
        FT_Done_FreeType(ft->lib);
        free(d->ft_ptr);
        return 4;
    }

    free(buf);
    return 0;
}

int Enj_OpenFontFile(Enj_FontLoader *d, const char *file){
    d->ft_ptr = malloc(sizeof(freetypedata));
    if(!d->ft_ptr) return 1;

    freetypedata *ft = (freetypedata *)d->ft_ptr;

    if(FT_Init_FreeType(&ft->lib)){
        free(d->ft_ptr);
        return 2;
    }


    if(FT_New_Face(ft->lib, file, 0, &ft->face)){
        FT_Done_FreeType(ft->lib);
        free(d->ft_ptr);
        return 3;
    }
    return 0;
}

void Enj_CloseFont(Enj_FontLoader *d){
    freetypedata *ft = (freetypedata *)d->ft_ptr;
    FT_Done_Face(ft->face);
    FT_Done_FreeType(ft->lib);
    free(d->ft_ptr);
}

int Enj_ReadFont(Enj_FontLoader *d, size_t size,
    Enj_FontCallback f, void *userdata){

    FT_ULong charcode;
    FT_UInt gindex;

    freetypedata *ft = (freetypedata *)d->ft_ptr;

    FT_Set_Pixel_Sizes(ft->face, 0, (FT_UInt)size);

    FT_Load_Glyph(ft->face, 0, FT_LOAD_DEFAULT);
    FT_Render_Glyph(ft->face->glyph, FT_RENDER_MODE_NORMAL);

    //Handle edge case with null char present or not
    (*f)(
        ft->face->glyph->bitmap.buffer,
        0,
        ft->face->glyph->bitmap.width,
        ft->face->glyph->bitmap.rows,
        ft->face->glyph->bitmap.pitch,
        ft->face->glyph->metrics.horiBearingX/64,
        -ft->face->glyph->metrics.horiBearingY/64,
        ft->face->glyph->metrics.horiAdvance/64,
        userdata
    );

    charcode = FT_Get_First_Char(ft->face, &gindex);

    if((charcode == 0) & (gindex != 0)){
        charcode = FT_Get_Next_Char(ft->face, charcode, &gindex);
    }

    while(gindex != 0)
    {
        FT_Load_Glyph(ft->face, gindex, FT_LOAD_DEFAULT);
        FT_Render_Glyph(ft->face->glyph, FT_RENDER_MODE_NORMAL);

        (*f)(
            ft->face->glyph->bitmap.buffer,
            charcode,
            ft->face->glyph->bitmap.width,
            ft->face->glyph->bitmap.rows,
            ft->face->glyph->bitmap.pitch,
            ft->face->glyph->metrics.horiBearingX/64,
            -ft->face->glyph->metrics.horiBearingY/64,
            ft->face->glyph->metrics.horiAdvance/64,
            userdata
        );

        charcode = FT_Get_Next_Char(ft->face, charcode, &gindex);
    }

    return 0;
}


int Enj_ReadFontBegin(Enj_FontLoader *d, Enj_FontGlyph *g, size_t size){
    freetypedata *ft = (freetypedata *)d->ft_ptr;

    FT_Set_Pixel_Sizes(ft->face, 0, (FT_UInt)size);

    unsigned long charcode;
    FT_UInt gindex;
    charcode = (unsigned long)FT_Get_First_Char(ft->face, &gindex);
    if(gindex == 0) return 0;

    FT_Load_Glyph(ft->face, gindex, FT_LOAD_DEFAULT);
    FT_Render_Glyph(ft->face->glyph, FT_RENDER_MODE_NORMAL);

    g->charcode = charcode;
    g->img = ft->face->glyph->bitmap.buffer;
    g->w = ft->face->glyph->bitmap.width;
    g->h = ft->face->glyph->bitmap.rows;
    g->pitch = ft->face->glyph->bitmap.pitch;
    g->xshift = ft->face->glyph->metrics.horiBearingX/64;
    g->yshift = -ft->face->glyph->metrics.horiBearingY/64;
    g->advance = ft->face->glyph->metrics.horiAdvance/64;
    return 1;
}
int Enj_ReadFontNext(Enj_FontLoader *d, Enj_FontGlyph *g){
    freetypedata *ft = (freetypedata *)d->ft_ptr;

    unsigned long charcode;
    FT_UInt gindex;
    charcode = (unsigned long)FT_Get_Next_Char(ft->face,
        (FT_ULong)g->charcode, &gindex);
    if(gindex == 0) return 0;

    FT_Load_Glyph(ft->face, gindex, FT_LOAD_DEFAULT);
    FT_Render_Glyph(ft->face->glyph, FT_RENDER_MODE_NORMAL);

    g->charcode = charcode;
    g->img = ft->face->glyph->bitmap.buffer;
    g->w = ft->face->glyph->bitmap.width;
    g->h = ft->face->glyph->bitmap.rows;
    g->pitch = ft->face->glyph->bitmap.pitch;
    g->xshift = ft->face->glyph->metrics.horiBearingX/64;
    g->yshift = -ft->face->glyph->metrics.horiBearingY/64;
    g->advance = ft->face->glyph->metrics.horiAdvance/64;
    return 1;
}
void Enj_ReadFontEnd(Enj_FontLoader *d, Enj_FontGlyph *g){
    freetypedata *ft = (freetypedata *)d->ft_ptr;

    FT_Load_Glyph(ft->face, 0, FT_LOAD_DEFAULT);
    FT_Render_Glyph(ft->face->glyph, FT_RENDER_MODE_NORMAL);

    g->charcode = 0;
    g->img = ft->face->glyph->bitmap.buffer;
    g->w = ft->face->glyph->bitmap.width;
    g->h = ft->face->glyph->bitmap.rows;
    g->pitch = ft->face->glyph->bitmap.pitch;
    g->xshift = ft->face->glyph->metrics.horiBearingX/64;
    g->yshift = -ft->face->glyph->metrics.horiBearingY/64;
    g->advance = ft->face->glyph->metrics.horiAdvance/64;
}
