# This file is part of Noggit3, licensed under GNU General Public License (version 3).

# Set this to more paths you windows guys need.
# this shouldnt be here....steff pls try to use the vars *_DIR or LIB_ROOT

if(LIB_ROOT)
  set(FREETYPE_DIR "${LIB_ROOT}/freetype")
  set(STORMLIB_DIR "${LIB_ROOT}/stormlib")
  set(GLEW_DIR "${LIB_ROOT}/glew")
  set(BOOST_ROOT "${LIB_ROOT}/boost")
endif(LIB_ROOT)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${FREETYPE_DIR} ${STORMLIB_DIR} ${GLEW_DIR})