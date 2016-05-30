project(model-ghdl)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})


add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ln -sf ${PROJECT_NAME} vsim
    COMMAND ln -sf ${PROJECT_NAME} vcom
    COMMAND ln -sf ${PROJECT_NAME} vmap
    COMMAND ln -sf ${PROJECT_NAME} vdel
    COMMAND ln -sf ${PROJECT_NAME} vlib
)

install (TARGETS model-ghdl DESTINATION bin)
install (FILES vsim vcom vmap vdel vlib DESTINATION bin)
