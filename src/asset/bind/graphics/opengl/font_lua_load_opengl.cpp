#include <condition_variable>
#include <mutex>
#include <string>

#include <stdlib.h>

#include <GL/gl3w.h>
#include <lua.hpp>

#include "../../../../core/allocator.h"
#include "../../../../core/act_lua.h"
#include "../../../../core/graphics/opengl/texture_opengl.h"
#include "../../../../core/graphics/opengl/glyph_opengl.h"

#include "../../../util/multi_dispatch.hpp"

#include "../../../asset_codes.h"
#include "../../../luaasset.h"
#include "../../../graphics/luafont.h"
#include "font_binder_opengl.hpp"
#include "font_lua_load_opengl.h"

#include "texture_binder_opengl.hpp"
#include "texture_lua_load_opengl.h"
#include "glyph_binder_opengl.hpp"
#include "glyph_lua_load_opengl.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

typedef struct glyphpair{
    FT_ULong charcode;
    int x;
    int y;
    int w;
    int h;

    int xoffset;
    int yoffset;
    int advance;

    size_t texture_id;
    Enj_Glyph_OpenGL *glyph;
}glyphpair;

static int luagetnullchar(lua_State *L){
    lua_geti(L, 1, -1);
    return 1;
}

int Enj_Lua_FontOnPreload_OpenGL(lua_State *L){
    if(!lua_isstring(L, 2)) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }
    int isint;
    lua_Integer fontsize = lua_tointegerx(L, 3, &isint);
    if(!isint) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }
    if((fontsize <= 0) | (fontsize > 1<<10)) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }
    unsigned int fsize = (unsigned int)fontsize;

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    font_binder_OpenGL *ctx = (font_binder_OpenGL *)la->ctx;
    luafont *lf = (luafont *)lua_newuserdatauv(L, sizeof(luafont), 0);
    lua_setiuservalue(L, 1, 1);

    {
        std::lock_guard<std::mutex> lock(ctx->dispatch.wq.mtx);

        ctx->dispatch.wq.q.push(
        [la, path = ctx->basepath + lua_tostring(L, 2), fsize, ctx](){

            FT_Library ft;
            FT_Face face;
            if(FT_Init_FreeType(&ft)){
                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_LIBRARY);
                });

                return;
            }
            if(FT_New_Face(ft, path.c_str(), 0, &face)){
                FT_Done_FreeType(ft);

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_FILE);
                });
                return;
            }
            FT_Set_Pixel_Sizes(face, 0, (FT_UInt)fsize);
            unsigned int fontheight = (unsigned int)fsize;

            FT_UInt gindex;

            size_t numchars = 0;
            for(
                FT_ULong charcode = FT_Get_First_Char(face, &gindex);
                gindex != 0;
                charcode = FT_Get_Next_Char(face, charcode, &gindex)
            )
            {
                ++numchars;
            }

            glyphpair *glyphpairs = (glyphpair *)malloc((numchars+1) * sizeof(glyphpair));
            if(!glyphpairs){
                FT_Done_Face(face);
                FT_Done_FreeType(ft);

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_MEMORY);
                });
                return;
            }
            FT_Glyph *ftglyphs = (FT_Glyph *)malloc((numchars+1) * sizeof(FT_Glyph));
            if(!ftglyphs){
                free(glyphpairs);
                FT_Done_Face(face);
                FT_Done_FreeType(ft);

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_MEMORY);
                });
                return;
            }
            //Handle regular chars
            size_t i;
            FT_ULong charcode;
            for(
                i = 0, charcode = FT_Get_First_Char(face, &gindex);
                i < numchars;
                i++,charcode = FT_Get_Next_Char(face, charcode, &gindex))
            {
                FT_Load_Glyph(face, gindex, FT_LOAD_DEFAULT);
                FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
                FT_Get_Glyph(face->glyph, ftglyphs + i);
                glyphpairs[i].w = face->glyph->bitmap.width;
                glyphpairs[i].h = face->glyph->bitmap.rows;

                glyphpairs[i].xoffset = face->glyph->metrics.horiBearingX/64;
                glyphpairs[i].yoffset = -face->glyph->metrics.horiBearingY/64;
                glyphpairs[i].advance = face->glyph->metrics.horiAdvance/64;

                glyphpairs[i].charcode = charcode;
            }
            //Handle last null glyph

            FT_Load_Glyph(face, 0, FT_LOAD_DEFAULT);
            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            FT_Get_Glyph(face->glyph, ftglyphs + numchars);
            glyphpairs[numchars].w = face->glyph->bitmap.width;
            glyphpairs[numchars].h = face->glyph->bitmap.rows;

            glyphpairs[numchars].xoffset = face->glyph->metrics.horiBearingX/64;
            glyphpairs[numchars].yoffset = -face->glyph->metrics.horiBearingY/64;
            glyphpairs[numchars].advance = face->glyph->metrics.horiAdvance/64;

            glyphpairs[numchars].charcode = 0;



            int pad = (fsize + 63)/64;

            size_t prog = 0;


            size_t extrabuffers = 0;

            unsigned char dimpow;

            while(true){
                for(dimpow = 8; dimpow < 12; dimpow++){
                    //Try to fit all glyphs into a single pow-of-2 texture
                    //TODO: Improve naive algorithm
                    int xpen = 0;
                    int ypen = 0;
                    int rowheight = 0;
                    for(i = prog; i < numchars+1; i++){
                        int iw = glyphpairs[i].w;
                        int ih = glyphpairs[i].h;
                        //Glyph too wide
                        if (iw + 2*pad > 1<<dimpow){
                            goto fontrect_fit_fail;
                        }
                        rowheight = ih > rowheight ? ih : rowheight;

                        if(xpen + iw + 2*pad > 1<<dimpow){
                            xpen = 0;
                            ypen += rowheight + 2*pad;
                        }
                        //Not enough height for glyph
                        if(ypen + rowheight + 2*pad > 1<<dimpow){
                            goto fontrect_fit_fail;
                        }


                        glyphpairs[i].x = xpen + pad;
                        glyphpairs[i].y = ypen + pad;
                        //Assign texture id as buffer aka number of textures
                        glyphpairs[i].texture_id = extrabuffers;

                        xpen += iw + 2*pad;
                    }
                    goto fontrect_success;

                fontrect_fit_fail:
                    continue;
                }
                //Implement multi texture by counting extra buffers needed
                //instead of dynamic array
                //No progress made means failure as a whole
                if (i == prog) goto fontrect_fail;

                ++extrabuffers;
                prog = i;
            }

        fontrect_fail:
            {
                for(size_t i = 0; i < numchars+1; i++){
                    FT_Done_Glyph(ftglyphs[i]);
                }
                free(ftglyphs);
                FT_Done_Face(face);
                FT_Done_FreeType(ft);
                free(glyphpairs);

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_PARAM);
                });
                return;
            }

        fontrect_success:

            //TODO create and fill extra buffers before last one
            unsigned char **atlasarray = (unsigned char **)
                malloc((extrabuffers+1)*sizeof(unsigned char *));
            if(!atlasarray){
                for(size_t i = 0; i < numchars+1; i++){
                    FT_Done_Glyph(ftglyphs[i]);
                }
                free(ftglyphs);
                FT_Done_Face(face);
                FT_Done_FreeType(ft);
                free(glyphpairs);

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_MEMORY);
                });
                return;
            }

            for(size_t i = 0; i < extrabuffers; i++){
                atlasarray[i] = (unsigned char *)calloc((1<<11) * (1<<11), 1);
                if(!atlasarray[i]){
                    for(size_t k = 0; k < i; k++){
                        free(atlasarray[k]);
                    }
                    free(atlasarray);

                    for(size_t i = 0; i < numchars+1; i++){
                        FT_Done_Glyph(ftglyphs[i]);
                    }
                    free(ftglyphs);
                    FT_Done_Face(face);
                    FT_Done_FreeType(ft);
                    free(glyphpairs);

                    std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                    ctx->dispatch.mq.q.push([la, ctx](){
                        luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_MEMORY);
                    });
                    return;
                }
            }

            size_t atlassize = ((size_t)1<<dimpow) * ((size_t)1<<dimpow);
            atlasarray[extrabuffers] =
                (unsigned char *)calloc(atlassize, 1);
            if(!atlasarray[extrabuffers]){
                for(size_t i = 0; i < extrabuffers; i++){
                    free(atlasarray[i]);
                }
                free(atlasarray);

                for(size_t i = 0; i < numchars+1; i++){
                    FT_Done_Glyph(ftglyphs[i]);
                }
                free(ftglyphs);
                FT_Done_Face(face);
                FT_Done_FreeType(ft);
                free(glyphpairs);

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_MEMORY);
                });
                return;
            }


            for(i = 0; i < numchars+1; i++){
                //Fill out buffer to be used in texture on lua side
                int gx = glyphpairs[i].x;
                int gy = glyphpairs[i].y;
                int gw = glyphpairs[i].w;
                int gh = glyphpairs[i].h;

                unsigned char dim =
                        (glyphpairs[i].texture_id != extrabuffers) ?
                            11 : dimpow;

                for(int r = 0; r < gh; r++){
                    for(int c = 0; c < gw; c++){
                        unsigned char *pixel =
                            atlasarray[glyphpairs[i].texture_id]
                                + (((gy + r)<<dim) + (gx + c));

                        FT_BitmapGlyph bmg = (FT_BitmapGlyph)ftglyphs[i];

                        int pitch = bmg->bitmap.pitch;
                        *pixel = bmg->bitmap.buffer[r*pitch + c];
                    }
                }
            }

            for(size_t i = 0; i < numchars+1; i++){
                FT_Done_Glyph(ftglyphs[i]);
            }
            free(ftglyphs);
            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
            ctx->dispatch.mq.q.push([la, ctx, atlasarray, extrabuffers,
                glyphpairs, dimpow, numchars, fontheight]() {


                for(size_t i = 0; i < numchars+1; i++){
                    glyphpairs[i].glyph = (Enj_Glyph_OpenGL *)
                        Enj_Alloc(&ctx->glyphbinder.alloc, sizeof(Enj_Glyph_OpenGL));

                    if(!glyphpairs[i].glyph){
                        for(size_t k = 0; k < i; k++){
                            Enj_Free(&ctx->glyphbinder.alloc, glyphpairs[k].glyph);
                        }
                        for(size_t i = 0; i < extrabuffers+1; i++){
                            free(atlasarray[i]);
                        }
                        free(atlasarray);
                        free(glyphpairs);
                        luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_POOL);
                        return;
                    }

                    unsigned char dim =
                        (glyphpairs[i].texture_id != extrabuffers) ?
                            11 : dimpow;

                    glyphpairs[i].glyph->width = glyphpairs[i].w;
                    glyphpairs[i].glyph->height = glyphpairs[i].h;

                    glyphpairs[i].glyph->fliprotate =
                        0<<0 | 1<<2 | 2<<4 | 3<<6;

                    glyphpairs[i].glyph->u_ul = (GLfloat)
                        glyphpairs[i].x / (1<<dim);
                    glyphpairs[i].glyph->v_ul = (GLfloat)
                        glyphpairs[i].y / (1<<dim);
                    glyphpairs[i].glyph->u_dr = (GLfloat)
                        (glyphpairs[i].x+glyphpairs[i].w) / (1<<dim);
                    glyphpairs[i].glyph->v_dr = (GLfloat)
                        (glyphpairs[i].y+glyphpairs[i].h) / (1<<dim);
                }

                Enj_Texture_OpenGL **texarray = (Enj_Texture_OpenGL **)
                    malloc((extrabuffers+1)*sizeof(Enj_Texture_OpenGL *));

                for(size_t i = 0; i < extrabuffers+1; i++){
                    texarray[i] = (Enj_Texture_OpenGL *)
                        Enj_Alloc(&ctx->texturebinder.alloc, sizeof(Enj_Texture_OpenGL));
                    if(!texarray[i]){
                        for(size_t k = 0; k < i; k++){
                            Enj_Free(&ctx->texturebinder.alloc, texarray[k]);
                        }
                        free(texarray);
                        for(size_t i = 0; i < numchars+1; i++){
                            Enj_Free(&ctx->glyphbinder.alloc, glyphpairs[i].glyph);
                        }
                        for(size_t i = 0; i < extrabuffers+1; i++){
                            free(atlasarray[i]);
                        }
                        free(atlasarray);
                        free(glyphpairs);
                        luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_POOL);

                        return;
                    }

                    unsigned char dim = (i != extrabuffers) ?
                                            11 : dimpow;
                    texarray[i]->width = 1<<dim;
                    texarray[i]->height = 1<<dim;
                    glGenTextures(1, &texarray[i]->id);
                    glBindTexture(GL_TEXTURE_2D, texarray[i]->id);
                    //Power of 2 texture at least 2^8 by 2^8
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
                    glTexImage2D(
                        GL_TEXTURE_2D, 0, GL_R8, 1<<dim, 1<<dim,
                        0, GL_RED, GL_UNSIGNED_BYTE, atlasarray[i]
                    );
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    //Set swizzle to make the single channel the alpha
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

                }
                for(size_t i = 0; i < extrabuffers+1; i++){
                    free(atlasarray[i]);
                }
                free(atlasarray);




                lua_getfield(ctx->Lmain, LUA_REGISTRYINDEX, "assetweaktable");
                lua_getfield(ctx->Lmain, LUA_REGISTRYINDEX, "gameproto");
                lua_getfield(ctx->Lmain, 2, "asset");
                lua_pushlightuserdata(ctx->Lmain, la->data);
                lua_gettable(ctx->Lmain, 1);
                //Init lua texture

                lua_createtable(ctx->Lmain, extrabuffers+1, 0);

                for(size_t i = 0; i < extrabuffers+1; i++){
                    luaasset *la_texture = (luaasset *)
                        lua_newuserdatauv(ctx->Lmain, sizeof(luaasset), 0);
                    la_texture->ctx = &ctx->texturebinder;
                    la_texture->data = texarray[i];
                    la_texture->refcount = 1;
                    la_texture->flag = 5;
                    la_texture->onunload = Enj_Lua_TextureOnUnload_OpenGL;
                    la_texture->oncanunload = Enj_Lua_TextureOnCanUnload_OpenGL;
                    lua_getfield(ctx->Lmain, 3, "texture");
                    lua_setmetatable(ctx->Lmain, 6);

                    lua_seti(ctx->Lmain, 5, i+1);
                }
                lua_setiuservalue(ctx->Lmain, 4, 2);

                //Init lua glyphs, using the lua texture

                for(size_t i = 0; i < numchars+1; i++){
                    glyphpairs[i].glyph->texture =
                        texarray[glyphpairs[i].texture_id];
                }
                lua_createtable(ctx->Lmain, 0, 0);
                for(size_t i = 0; i < numchars; i++){
                    luafontchar *lfontchar = (luafontchar *)
                        lua_newuserdatauv(ctx->Lmain, sizeof(luafontchar), 1);
                    lfontchar->xoffset = glyphpairs[i].xoffset;
                    lfontchar->yoffset = glyphpairs[i].yoffset;
                    lfontchar->advance = glyphpairs[i].advance;
                    lua_getfield(ctx->Lmain, 2, "fontchar");
                    lua_setmetatable(ctx->Lmain, 6);


                    luaasset *la_glyph = (luaasset *)
                        lua_newuserdatauv(ctx->Lmain, sizeof(luaasset), 1);
                    la_glyph->ctx = &ctx->glyphbinder;
                    la_glyph->data = glyphpairs[i].glyph;
                    la_glyph->refcount = 1;
                    la_glyph->flag = 5;
                    la_glyph->onunload = Enj_Lua_GlyphOnUnload_OpenGL;
                    la_glyph->oncanunload = Enj_Lua_GlyphOnCanUnload_OpenGL;
                    lua_getfield(ctx->Lmain, 3, "glyph");
                    lua_setmetatable(ctx->Lmain, 7);
                    lua_getiuservalue(ctx->Lmain, 4, 2);
                    lua_geti(ctx->Lmain, 8, glyphpairs[i].texture_id+1);
                    lua_setiuservalue(ctx->Lmain, 7, 1);
                    lua_pop(ctx->Lmain, 1);

                    //Set uservalue glyph of fontchar
                    lua_setiuservalue(ctx->Lmain, 6, 1);

                    lua_seti(ctx->Lmain, 5, glyphpairs[i].charcode);
                }

                {
                    luafontchar *lfontchar = (luafontchar *)
                        lua_newuserdatauv(ctx->Lmain, sizeof(luafontchar), 1);
                    lfontchar->xoffset = glyphpairs[numchars].xoffset;
                    lfontchar->yoffset = glyphpairs[numchars].yoffset;
                    lfontchar->advance = glyphpairs[numchars].advance;
                    lua_getfield(ctx->Lmain, 2, "fontchar");
                    lua_setmetatable(ctx->Lmain, 6);


                    luaasset *la_glyph = (luaasset *)
                        lua_newuserdatauv(ctx->Lmain, sizeof(luaasset), 1);
                    la_glyph->ctx = &ctx->glyphbinder;
                    la_glyph->data = glyphpairs[numchars].glyph;
                    la_glyph->refcount = 1;
                    la_glyph->flag = 5;
                    la_glyph->onunload = Enj_Lua_GlyphOnUnload_OpenGL;
                    la_glyph->oncanunload = Enj_Lua_GlyphOnCanUnload_OpenGL;
                    lua_getfield(ctx->Lmain, 3, "glyph");
                    lua_setmetatable(ctx->Lmain, 7);
                    lua_getiuservalue(ctx->Lmain, 4, 2);
                    lua_geti(ctx->Lmain, 8, glyphpairs[numchars].texture_id+1);
                    lua_setiuservalue(ctx->Lmain, 7, 1);
                    lua_pop(ctx->Lmain, 1);

                    //Set uservalue glyph of fontchar
                    lua_setiuservalue(ctx->Lmain, 6, 1);
                }
                lua_seti(ctx->Lmain, 5, -1);
                //Set metatable of glyph table to handle null char
                lua_createtable(ctx->Lmain, 0, 1);
                lua_pushcfunction(ctx->Lmain, luagetnullchar);
                lua_setfield(ctx->Lmain, 6, "__index");
                //Set metatable for fontchar table
                lua_setmetatable(ctx->Lmain, 5);


                //Set glyphs uservalue for the font luaasset
                lua_setiuservalue(ctx->Lmain, 4, 3);

                //update C-side lua asset metadata
                luafont *lf = (luafont *)la->data;
                lf->height = fontheight;

                free(texarray);
                free(glyphpairs);

                lua_settop(ctx->Lmain, 0);
                luafinishpreloadasset(ctx->Lmain, la, ASSET_OK);

            });

        });
        ctx->dispatch.cv.notify_one();
    }

    la->data = lf;
    lua_pushvalue(L, 1);
    return 1;
}
int Enj_Lua_FontOnUnload_OpenGL(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    //Free all glyphs
    lua_getiuservalue(L, 1, 3);
    lua_pushnil(L);
    while(lua_next(L, 2) != 0){
        luafontchar *lfc = (luafontchar *)lua_touserdata(L, 4);
        lua_getiuservalue(L, 4, 1);
        luaasset *la_glyph = (luaasset *)lua_touserdata(L, 5);

        la_glyph->refcount = 0;
        lua_pushcfunction(L, Enj_Lua_GlyphOnUnload_OpenGL);
        lua_pushvalue(L, 5);
        lua_call(L, 1, 0);
        la_glyph->data = NULL;
        la_glyph->flag &= ~(1<<0);

        lua_pop(L, 2);
    }

    //Free textures

    lua_getiuservalue(L, 1, 2);
    lua_pushnil(L);
    while(lua_next(L, 3) != 0){
        lua_pushcfunction(L, Enj_Lua_TextureOnUnload_OpenGL);
        lua_pushvalue(L, 5);
        luaasset *la_texture = (luaasset *)lua_touserdata(L, 5);
        la_texture->refcount = 0;
        lua_call(L, 1, 0);
        la_texture->data = NULL;
        la_texture->flag &= ~(1<<0);

        lua_pop(L, 1);
    }

    return 0;
}
int Enj_Lua_FontOnCanUnload_OpenGL(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    //Check all textures
    lua_getiuservalue(L, 1, 2);
    lua_pushnil(L);
    while(lua_next(L, 2) != 0){
        lua_pushcfunction(L, Enj_Lua_TextureOnCanUnload_OpenGL);
        lua_pushvalue(L, 2);
        luaasset *la_texture = (luaasset *)lua_touserdata(L, 4);
        if(la_texture->refcount > 1){
            lua_pushboolean(L, 0);
            return 1;
        }
        lua_call(L, 1, 1);
        if(!lua_toboolean(L, 2)){
            return 1;
        }

        lua_pop(L, 2);
    }

    //Check all glyphs
    lua_getiuservalue(L, 1, 3);
    lua_pushnil(L);
    while(lua_next(L, 3) != 0){
        luafontchar *lfc = (luafontchar *)lua_touserdata(L, 5);
        lua_getiuservalue(L, 5, 1);
        luaasset *la_glyph = (luaasset *)lua_touserdata(L, 6);
        if(la_glyph->refcount > 1){
            lua_pushboolean(L, 0);
            return 1;
        }

        lua_pushcfunction(L, Enj_Lua_GlyphOnCanUnload_OpenGL);
        lua_pushvalue(L, 6);
        lua_call(L, 1, 1);
        if(!lua_toboolean(L, 7)){
            return 1;
        }

        lua_pop(L, 3);
    }
    lua_pushboolean(L, 1);
    return 1;
}
