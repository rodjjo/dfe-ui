cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)

set(APP_NAME all-components)

file(GLOB Sources
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)

add_executable(
    ${APP_NAME}
    ${Sources}
)

target_link_libraries(
    ${APP_NAME}
    simple-ui
    stdc++ 
    sfml-system
    sfml-graphics
    GLU 
    GL
)

if(WIN32)
    target_link_libraries(
        ${APP_NAME}
        python3.lib
        python310.lib
        opengl32
        user32
        comctl32
        gdiplus
        ws2_32
        glu32
        Shcore
    )
    set(SUBSYSTEM_LINKER_OPTIONS "-mconsole")
    target_link_options(${APP_NAME} PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})
else(WIN32)
    target_link_libraries(
        ${APP_NAME}
        X11
    )
endif(WIN32)


install(TARGETS ${APP_NAME}
        RUNTIME DESTINATION bin
        COMPONENT runtime)

set_target_properties(${APP_NAME}
    PROPERTIES
    LINKER_LANGUAGE C
)

target_include_directories(
    ${APP_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../src/include"
)