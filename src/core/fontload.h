#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Instream Enj_Instream;
//glyph grayscale data, charcode, w, h, xshift, yshift, advance, userdata
typedef void (*Enj_FontCallback)
    (const char *, unsigned long, int, int, int, int, int, int, void *);

typedef struct Enj_FontLoader{
    void *ft_ptr;
} Enj_FontLoader;

typedef struct Enj_FontGlyph{
    const char *img;
    unsigned long charcode;
    int w, h, pitch, xshift, yshift, advance;
} Enj_FontGlyph;

int Enj_OpenFont(Enj_FontLoader *d, Enj_Instream *rs);
int Enj_OpenFontFile(Enj_FontLoader *d, const char *file);

void Enj_CloseFont(Enj_FontLoader *d);

//Loop through all glyphs in font, calling provided function every glyph
int Enj_ReadFont(Enj_FontLoader *d, size_t size,
    Enj_FontCallback f, void *userdata);

//Reads first font glyph, with size
int Enj_ReadFontBegin(Enj_FontLoader *d, Enj_FontGlyph *g, size_t size);
//Returns 1 if success, otherwise 0 for end
int Enj_ReadFontNext(Enj_FontLoader *d, Enj_FontGlyph *g);
//Reads the null char that represents unknown charcode
void Enj_ReadFontEnd(Enj_FontLoader *d, Enj_FontGlyph *g);

#ifdef __cplusplus
}
#endif
