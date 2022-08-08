
function(image_generate_header TARGET IMAGE OUTPUT_NAME OUTPUT_DIR)
    string(TOLOWER ${OUTPUT_NAME} OUTPUT_NAME_LC)
    set(HEADER "${OUTPUT_DIR}/${OUTPUT_NAME_LC}.image.h")
    set(HEADER_GEN_TARGET "${TARGET}_${OUTPUT_NAME_LC}_font_h")
    
    add_custom_target(${HEADER_GEN_TARGET} DEPENDS ${HEADER})

    add_custom_command(OUTPUT ${HEADER}
            DEPENDS ${IMAGE}
            COMMAND ${PROJECT_SOURCE_DIR}/image_to_cpp.py ${IMAGE} ${OUTPUT_NAME} ${HEADER}
            )
    add_dependencies(${TARGET} ${HEADER_GEN_TARGET})
    get_target_property(target_type ${TARGET} TYPE)
    if ("INTERFACE_LIBRARY" STREQUAL "${target_type}")
        target_include_directories(${TARGET} INTERFACE ${OUTPUT_DIR})
    else()
        target_include_directories(${TARGET} PUBLIC ${OUTPUT_DIR})
    endif()
endfunction()

