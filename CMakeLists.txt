cmake_minimum_required(VERSION 3.3)
project(game_text C)

set(CMAKE_C_FLAGS "-std=c99 -g -Wall --coverage")
set(SOURCES game.c game_aux.c game_ext.c queue.c game_tools.c)

include(CTest)
enable_testing()

## find SDL2
include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

include_directories(${SDL2_ALL_INC})
add_executable(game_sdl game_sdl.c model.c ${SOURCES})
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} m)
add_executable(model game_sdl.c model.c ${SOURCES})
target_link_libraries(model ${SDL2_ALL_LIBS} m)


add_executable(game_text game_text.c)

add_executable(game_test_rguerroudj game_test_rguerroudj.c)
add_executable(game_test_leseydi game_test_leseydi.c)
add_executable(game_test_piepierre game_test_piepierre.c)
add_executable(game_ext_test game_ext_test.c)
add_executable(game_tools_test game_tools_test.c)
add_executable(game_random game_random.c)
add_executable(game_solve game_solve.c)

target_link_libraries(${PROJECT_NAME} game)
target_link_libraries(game_test_rguerroudj game)
target_link_libraries(game_test_leseydi game)
target_link_libraries(game_test_piepierre game)
target_link_libraries(game_ext_test game)
target_link_libraries(game_ext_test queue)
target_link_libraries(game_tools_test game)
target_link_libraries(game_random game)
target_link_libraries(game_solve game)

add_library(game STATIC ${SOURCES})
add_library(queue STATIC queue.c)

add_test(test_piepierre_dummy ./game_test_piepierre dummy)
add_test(test_piepierre_game_is_connected ./game_test_piepierre game_is_connected)
add_test(test_piepierre_game_check_edge ./game_test_piepierre game_check_edge)
add_test(test_piepierre_game_get_ajacent_square ./game_test_piepierre game_get_ajacent_square)
add_test(test_piepierre_game_default ./game_test_piepierre game_default)
add_test(test_piepierre_game_reset_orientation ./game_test_piepierre game_reset_orientation)
add_test(test_piepierre_game_play_move ./game_test_piepierre game_play_move)
add_test(test_piepierre_game_get_piece_orientation ./game_test_piepierre game_get_piece_orientation)

add_test(test_rguerroudj_dummy ./game_test_rguerroudj dummy)
add_test(test_rguerroudj_game_new_empty ./game_test_rguerroudj game_new_empty)
add_test(test_rguerroudj_game_new ./game_test_rguerroudj game_new)
add_test(test_rguerroudj_game_copy ./game_test_rguerroudj game_copy)
add_test(test_rguerroudj_game_equal ./game_test_rguerroudj game_equal)
add_test(test_rguerroudj_game_delete ./game_test_rguerroudj game_delete)
add_test(test_rguerroudj_game_set_piece_shape ./game_test_rguerroudj game_set_piece_shape)
add_test(test_rguerroudj_game_set_piece_orientation ./game_test_rguerroudj game_set_piece_orientation)

add_test(test_leseydi_dummy ./game_test_leseydi dummy)
add_test(test_leseydi_game_print ./game_test_leseydi game_print)
add_test(test_leseydi_game_won ./game_test_leseydi game_won)
add_test(test_leseydi_game_get_piece_shape ./game_test_leseydi game_get_piece_shape)
add_test(test_leseydi_game_shuffle_orientation ./game_test_leseydi game_shuffle_orientation)
add_test(test_leseydi_game_default_solution ./game_test_leseydi game_default_solution)
add_test(test_leseydi_game_is_well_paired  ./game_test_leseydi game_is_well_paired)
add_test(test_leseydi_game_has_half_edge ./game_test_leseydi game_has_half_edge)

add_test(test_game_new_ext ./game_ext_test game_new_ext)
add_test(test_game_new_empty_ext ./game_ext_test game_new_empty_ext)
add_test(test_game_nb_rows ./game_ext_test game_nb_rows)
add_test(test_game_nb_cols ./game_ext_test game_nb_cols)
add_test(test_game_is_wrapping ./game_ext_test game_is_wrapping)
add_test(test_game_undo ./game_ext_test game_undo)
add_test(test_game_redo ./game_ext_test game_redo)
add_test(test_cross_piece ./game_ext_test cross_piece)

add_test(test_game_load ./game_tools_test game_load)
add_test(test_game_save ./game_tools_test game_save)


## copy useful ressources in the build directory
file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})