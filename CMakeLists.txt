cmake_minimum_required (VERSION 3.21.1)
set (CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "")
set (CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)" CACHE STRING "")
project (sdlapp)

set (APP_NAME "Untitled" CACHE PATH "Name of app")
set (APP_COMPANY "Cycloid" CACHE PATH "Name of app company")

set (DATA_DIRECTORY "" CACHE PATH "Path to directory containing data for the app")
set (APP_RENDER_BACKEND "opengl" CACHE STRING "Backend for the app")
set_property(CACHE APP_RENDER_BACKEND PROPERTY STRINGS "sdl;opengl")
add_compile_definitions(
    "APP_NAME=\"${APP_NAME}\""
    "APP_COMPANY=\"${APP_COMPANY}\""
)

set (CORE_SOURCE
    src/core/allocator.c
    src/core/allocator_malloc.c
    src/core/pngload.c
    src/core/instream.c

    src/core/act.c
    src/core/act_lua.c

    src/core/button.c
    src/core/button_lua.c
    src/core/keyboard.c
    src/core/keyboard_lua.c
    src/core/lua_extra.c

    src/asset/asset_codes.c
    src/asset/luaasset.c
    src/asset/asset_lua.c
    src/asset/lua_extra_asset.c

    src/asset/graphics/font_lua.c

    src/asset/audio/sound_lua.c

    src/asset/bind/audio/sound_binder.cpp
    src/asset/bind/audio/sound_lua_load.cpp

    src/asset/audio/music_lua.c

    src/asset/bind/audio/music_binder.cpp
    src/asset/bind/audio/music_lua_load.cpp

    src/asset/misc/data_lua.c

    src/asset/bind/misc/data_binder.cpp
    src/asset/bind/misc/data_lua_load.cpp

    src/render/list.c
    src/render/render_lua.c

    src/input/input.c
    )

if (APP_RENDER_BACKEND STREQUAL "opengl")
    add_subdirectory(gl3w EXCLUDE_FROM_ALL)
    set (CORE_SOURCE_RENDER
        src/asset/graphics/opengl/texture_lua_opengl.c
        src/asset/graphics/opengl/glyph_lua_opengl.c

        src/asset/bind/graphics/opengl/texture_binder_opengl.cpp
        src/asset/bind/graphics/opengl/texture_lua_load_opengl.cpp
        src/asset/bind/graphics/opengl/glyph_binder_opengl.cpp
        src/asset/bind/graphics/opengl/glyph_lua_load_opengl.cpp
        src/asset/bind/graphics/opengl/font_binder_opengl.cpp
        src/asset/bind/graphics/opengl/font_lua_load_opengl.cpp

        src/render/opengl/lua_extra_render_opengl.c
        src/render/opengl/renderlist_opengl.c
        src/render/opengl/renderlist_lua_opengl.c
        src/render/opengl/sprite_opengl.c
        src/render/opengl/sprite_lua_opengl.c
        src/render/opengl/primrect_opengl.c
        src/render/opengl/primrect_lua_opengl.c
        src/render/opengl/render_opengl.c
    )
else()
    set (CORE_SOURCE_RENDER
        src/asset/graphics/sdl/texture_lua_sdl.c
        src/asset/graphics/sdl/glyph_lua_sdl.c

        src/asset/bind/graphics/sdl/texture_binder_sdl.cpp
        src/asset/bind/graphics/sdl/texture_lua_load_sdl.cpp
        src/asset/bind/graphics/sdl/glyph_binder_sdl.cpp
        src/asset/bind/graphics/sdl/glyph_lua_load_sdl.cpp
        src/asset/bind/graphics/sdl/font_binder_sdl.cpp
        src/asset/bind/graphics/sdl/font_lua_load_sdl.cpp

        src/render/sdl/lua_extra_render_sdl.c
        src/render/sdl/renderlist_sdl.c
        src/render/sdl/renderlist_lua_sdl.c
        src/render/sdl/sprite_sdl.c
        src/render/sdl/sprite_lua_sdl.c
        src/render/sdl/primrect_sdl.c
        src/render/sdl/primrect_lua_sdl.c
        src/render/sdl/render_sdl.c
    )
endif ()

if (MSVC)
    add_library(core ${CORE_SOURCE} ${CORE_SOURCE_RENDER})
    add_executable (main WIN32
        src/main.cpp
        )

    if (APP_RENDER_BACKEND STREQUAL "opengl")
        target_compile_definitions(main PRIVATE APP_BACKEND=1)
        target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/gl3w/include")
        target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/gl3w/include")
        set (RENDER_LIBS gl3w OpenGL32.lib)
    endif ()

    set_target_properties(main PROPERTIES VS_DPI_AWARE "PerMonitor")

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64 bits
        set (SDL2_LIB
            "${CMAKE_SOURCE_DIR}/SDL2/lib/windows/x64/SDL2.lib"
            "${CMAKE_SOURCE_DIR}/SDL2/lib/windows/x64/SDL2main.lib"
        )
        set (SDL2_MIXER_LIB "${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/windows/x64/SDL2_mixer.lib")
        set (LUA_LIB "${CMAKE_SOURCE_DIR}/lua/lib/windows/x64/lua54.lib")
        set (ZLIB_LIB "${CMAKE_SOURCE_DIR}/zlib/lib/windows/x64/zlib.lib")
        set (PNG_LIB "${CMAKE_SOURCE_DIR}/libpng/lib/windows/x64/libpng16.lib")
        set (FT_LIB "${CMAKE_SOURCE_DIR}/freetype/lib/windows/x64/freetype.lib")

        set (SDL2_BIN "${CMAKE_SOURCE_DIR}/SDL2/lib/windows/x64/SDL2.dll")
        set (SDL2_MIXER_BIN "${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/windows/x64/SDL2_mixer.dll")
        set (LUA_BIN "${CMAKE_SOURCE_DIR}/lua/lib/windows/x64/lua54.dll")
        set (ZLIB_BIN "${CMAKE_SOURCE_DIR}/zlib/lib/windows/x64/zlib.dll")
        set (PNG_BIN "${CMAKE_SOURCE_DIR}/libpng/lib/windows/x64/libpng16.dll")
        set (FT_BIN "${CMAKE_SOURCE_DIR}/freetype/lib/windows/x64/freetype.dll")

        add_custom_command(TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:main>/main"
            COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:main>
                "$<TARGET_FILE_DIR:main>/main"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/crt/windows/x64"
                "$<TARGET_FILE_DIR:main>/main"
        )

        install (TARGETS main RUNTIME DESTINATION .)
        install (DIRECTORY "${DATA_DIRECTORY}/" "${CMAKE_SOURCE_DIR}/crt/windows/x64/" DESTINATION .)

    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        # 32 bits
        set (SDL2_LIB
            "${CMAKE_SOURCE_DIR}/SDL2/lib/windows/x86/SDL2.lib"
            "${CMAKE_SOURCE_DIR}/SDL2/lib/windows/x86/SDL2main.lib"
        )
        set (SDL2_MIXER_LIB "${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/windows/x86/SDL2_mixer.lib")
        set (LUA_LIB "${CMAKE_SOURCE_DIR}/lua/lib/windows/x86/lua54.lib")
        set (ZLIB_LIB "${CMAKE_SOURCE_DIR}/zlib/lib/windows/x86/zlib.lib")
        set (PNG_LIB "${CMAKE_SOURCE_DIR}/libpng/lib/windows/x86/libpng16.lib")
        set (FT_LIB "${CMAKE_SOURCE_DIR}/freetype/lib/windows/x86/freetype.lib")

        set (SDL2_BIN "${CMAKE_SOURCE_DIR}/SDL2/lib/windows/x86/SDL2.dll")
        set (SDL2_MIXER_BIN "${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/windows/x86/SDL2_mixer.dll")
        set (LUA_BIN "${CMAKE_SOURCE_DIR}/lua/lib/windows/x86/lua54.dll")
        set (ZLIB_BIN "${CMAKE_SOURCE_DIR}/zlib/lib/windows/x86/zlib.dll")
        set (PNG_BIN "${CMAKE_SOURCE_DIR}/libpng/lib/windows/x86/libpng16.dll")
        set (FT_BIN "${CMAKE_SOURCE_DIR}/freetype/lib/windows/x86/freetype.dll")

        add_custom_command(TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:main>/main"
            COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:main>
                "$<TARGET_FILE_DIR:main>/main"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/crt/windows/x86"
                "$<TARGET_FILE_DIR:main>/main"
        )

        install (TARGETS main RUNTIME DESTINATION .)
        install (DIRECTORY "${DATA_DIRECTORY}/" "${CMAKE_SOURCE_DIR}/crt/windows/x86/" DESTINATION .)

    endif ()

    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL2_BIN}
            ${SDL2_MIXER_BIN}
            ${LUA_BIN}
            ${ZLIB_BIN}
            ${PNG_BIN}
            ${FT_BIN}
            "$<TARGET_FILE_DIR:main>/main"
    )
    install (FILES
        ${SDL2_BIN}
        ${SDL2_MIXER_BIN}
        ${LUA_BIN}
        ${ZLIB_BIN}
        ${PNG_BIN}
        ${FT_BIN}

        DESTINATION .
    )

    if (DATA_DIRECTORY)
        add_custom_command(TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${DATA_DIRECTORY}"
                "$<TARGET_FILE_DIR:main>/main"
        )
        install (DIRECTORY "${DATA_DIRECTORY}/" DESTINATION .)
    endif ()

elseif (APPLE)
    string (JOIN " " CMAKE_XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS
            "\"${CMAKE_SOURCE_DIR}/SDL2/lib/macos\""
            "\"${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/macos\""
            "\"${CMAKE_SOURCE_DIR}/lua/lib/macos\""
            "\"${CMAKE_SOURCE_DIR}/libpng/lib/macos\""
            "\"${CMAKE_SOURCE_DIR}/freetype/lib/macos\""
    )

    if (DATA_DIRECTORY)
        file (GLOB RESOURCE_FILES CONFIGURE_DEPENDS "${DATA_DIRECTORY}/*")
    endif ()

    add_library(core ${CORE_SOURCE} ${CORE_SOURCE_RENDER})
    add_executable (main MACOSX_BUNDLE
        src/main.cpp
        ${RESOURCE_FILES}
        )

    if (APP_RENDER_BACKEND STREQUAL "opengl")
        target_compile_definitions(main PRIVATE APP_BACKEND=1)
        target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/gl3w/include")
        target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/gl3w/include")
        set (RENDER_LIBS gl3w "-framework OpenGL")
    endif ()

    install (TARGETS main BUNDLE DESTINATION .)


    set (EMBEDS
        "${CMAKE_SOURCE_DIR}/SDL2/lib/macos/SDL2.framework"
        "${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/macos/SDL2_mixer.framework"
        "${CMAKE_SOURCE_DIR}/lua/lib/macos/lua.framework"
        "${CMAKE_SOURCE_DIR}/libpng/lib/macos/png.framework"
        "${CMAKE_SOURCE_DIR}/freetype/lib/macos/freetype.framework"
    )

    set_target_properties(main PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME main
        MACOSX_BUNDLE_BUNDLE_NAME main
        XCODE_EMBED_FRAMEWORKS "${EMBEDS}"

        XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY TRUE
        XCODE_EMBED_FRAMEWORKS_REMOVE_HEADERS_ON_COPY TRUE

        INSTALL_RPATH_USE_LINK_PATH FALSE
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE

        RESOURCE "${RESOURCE_FILES}"
    )

    find_package (ZLIB REQUIRED)

    set (SDL2_LIB "-framework SDL2")
    set (SDL2_MIXER_LIB "-framework SDL2_mixer")
    set (LUA_LIB "-framework lua")
    set (ZLIB_LIB ${ZLIB_LIBRARY})
    set (PNG_LIB "-framework png")
    set (FT_LIB "-framework freetype")


endif ()

set_target_properties(core PROPERTIES CXX_STANDARD 14)
set_target_properties(main PROPERTIES CXX_STANDARD 14)

target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/SDL2/include")
target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/SDL2_mixer/include")
target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/lua/include")
target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/zlib/include")
target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/libpng/include")
target_include_directories(core PRIVATE "${CMAKE_SOURCE_DIR}/freetype/include")
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/SDL2/include")
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/SDL2_mixer/include")
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/lua/include")
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/zlib/include")
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/libpng/include")
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/freetype/include")

target_link_libraries (main PUBLIC core
    ${SDL2_LIB}
    ${SDL2_MIXER_LIB}
    ${LUA_LIB}
    ${ZLIB_LIB}
    ${PNG_LIB}
    ${FT_LIB}

    ${RENDER_LIBS})



add_executable (testallo src/testallo.cpp src/core/allocator.c)

include (CPack)
