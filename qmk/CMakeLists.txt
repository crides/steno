target_include_directories(app PRIVATE .)
target_sources(app PRIVATE hist.c stroke.c steno.c)
target_include_directories(app PRIVATE impl/zmk)
target_sources(app PRIVATE impl/zmk/behavior_steno.c impl/zmk/flash.c impl/zmk/keypress.c)

if (CONFIG_ZMK_EMBEDDED_STENO_NOUI)
    target_compile_definitions(app PRIVATE -DSTENO_NOUI)
else()
    target_sources(app PRIVATE impl/zmk/disp.c)
    target_sources(app PRIVATE impl/zmk/iosevka_16.c)
endif()

if (CONFIG_ZMK_EMBEDDED_STENO_READONLY)
	target_compile_definitions(app PRIVATE -DSTENO_READONLY)
else()
	target_sources(app PRIVATE dict_editing.c freemap.c)
endif()

# Don't need MSD

if (CONFIG_ZMK_EMBEDDED_STENO_STROKE_DISPLAY)
    target_compile_definitions(app PRIVATE -DSTENO_STROKE_DISPLAY)
endif()

if (CONFIG_ZMK_EMBEDDED_STENO_NOUNICODE)
	target_compile_definitions(app PRIVATE -DSTENO_NOUNICODE)
endif()

if (CONFIG_ZMK_EMBEDDED_STENO_NOORTHOGRAPHY)
	target_compile_definitions(app PRIVATE -DSTENO_NOORTHOGRAPHY)
else()
	target_sources(app PRIVATE orthography.c)
endif()

if (CONFIG_ZMK_EMBEDDED_STENO_DEBUG)
	target_compile_definitions(app PRIVATE -DSTENO_DEBUG)

    if (CONFIG_ZMK_EMBEDDED_STENO_DEBUG_HIST)
        target_compile_definitions(app PRIVATE -DSTENO_DEBUG_HIST)
    endif()

    if (CONFIG_ZMK_EMBEDDED_STENO_DEBUG_STROKE)
        target_compile_definitions(app PRIVATE -DSTENO_DEBUG_STROKE)
    endif()

    if (CONFIG_ZMK_EMBEDDED_STENO_DEBUG_FLASH)
        target_compile_definitions(app PRIVATE -DSTENO_DEBUG_FLASH)
    endif()

    if (CONFIG_ZMK_EMBEDDED_STENO_DEBUG_DICTED)
        target_compile_definitions(app PRIVATE -DSTENO_DEBUG_DICTED)
    endif()
endif()
