
function(font_generate_header TARGET FONT FONT_SIZE OUTPUT_NAME OUTPUT_DIR)
    string(TOLOWER ${OUTPUT_NAME} OUTPUT_NAME_LC)
    set(HEADER "${OUTPUT_DIR}/${OUTPUT_NAME_LC}.font.h")

    set(HEADER_GEN_TARGET "${TARGET}_${OUTPUT_NAME_LC}_font_h")

    add_custom_target(${HEADER_GEN_TARGET} DEPENDS ${HEADER})

    add_custom_command(OUTPUT ${HEADER}
            COMMAND ${PROJECT_SOURCE_DIR}/font_to_cpp.py ${FONT} ${FONT_SIZE} ${OUTPUT_NAME} ${HEADER}
            )
    add_dependencies(${TARGET} ${HEADER_GEN_TARGET})
    get_target_property(target_type ${TARGET} TYPE)
    if ("INTERFACE_LIBRARY" STREQUAL "${target_type}")
        target_include_directories(${TARGET} INTERFACE ${OUTPUT_DIR})
    else()
        target_include_directories(${TARGET} PUBLIC ${OUTPUT_DIR})
    endif()
endfunction()

