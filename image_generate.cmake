
function(image_generate_header TARGET IMAGE OUTPUT_DIR)
    get_filename_component(IMAGE_NAME ${IMAGE} NAME_WE)
    set(HEADER "${OUTPUT_DIR}/${IMAGE_NAME}.image.h")
    
    get_filename_component(HEADER_GEN_TARGET ${IMAGE} NAME_WE)
    set(HEADER_GEN_TARGET "${TARGET}_${HEADER_GEN_TARGET}_image_h")

    add_custom_target(${HEADER_GEN_TARGET} DEPENDS ${HEADER})

    add_custom_command(OUTPUT ${HEADER}
            DEPENDS ${IMAGE}
            COMMAND ${PROJECT_SOURCE_DIR}/image_to_cpp.py ${IMAGE} ${HEADER}
            )
    add_dependencies(${TARGET} ${HEADER_GEN_TARGET})
    get_target_property(target_type ${TARGET} TYPE)
    if ("INTERFACE_LIBRARY" STREQUAL "${target_type}")
        target_include_directories(${TARGET} INTERFACE ${OUTPUT_DIR})
    else()
        target_include_directories(${TARGET} PUBLIC ${OUTPUT_DIR})
    endif()
endfunction()

