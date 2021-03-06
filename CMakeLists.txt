# This file is part of Noggit3, licensed under GNU General Public License (version 3).

cmake_minimum_required(VERSION 3.3)
cmake_policy (SET CMP0057 NEW) # "Support new IN_LIST if() operator."
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Project name
project(noggit)

# Giperion: Set Edit and Continue by default for all windows debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib )
set( CMAKE_CONFIGURATION_TYPES Debug;RelWithDebInfo )

include (CheckCXXCompilerFlag)
macro (add_compiler_flag_if_supported _VAR _FLAG)
  string (MAKE_C_IDENTIFIER "CXX_COMPILER_SUPPORTS_${_FLAG}" _test_variable)
  check_cxx_compiler_flag ("${_FLAG}" ${_test_variable})
  if (${_test_variable})
    if ("${${_VAR}}" STREQUAL "")
      set (${_VAR} "${_FLAG}")
    else()
      set (${_VAR} "${${_VAR}} ${_FLAG}")
    endif()
  endif()
endmacro()

# we can live with the compilation unit containing the vtable not being fixed
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS -Wno-weak-vtables)
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS -Wno-weak-template-vtables)

# we don't care for a few bytes
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS -Wno-padded)

# msvc++ mangles struct/class into name, thus symbols may be called differently
# with a bad forward-decl. we want compilation to fail, not linking.
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /we4099)
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS -Werror=mismatched-tags)

# yes, we intend to use multi-character character constants
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS -Wno-multichar)


OPTION(NAME_REUSE_AS_ERROR "Make name reuse warnings errors ?" OFF)
if(NAME_REUSE_AS_ERROR)
# declaration of 'identifier' hides previous: 
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /we4456) # local declaration
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /we4457) # function parameter
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /we4458) # class members
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /we4459) # global declaration
endif()

# better exception handling for visual studio, particularly for the asynchronous stuff
add_compiler_flag_if_supported(CMAKE_CXX_FLAGS /EHa)
# multi core building for visual studio
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /MP)


# Disable opengl error log
OPTION(ADDITIONAL_OPTIMIZATION_FLAGS "Enable OpenGL error check ?" OFF)
IF(ADDITIONAL_OPTIMIZATION_FLAGS)
MESSAGE( STATUS "Enabled additional optimization flags for msvc.")
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /Ob2) # inline any suitable functions
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /Oi)  # enable intrasic functions
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /Ot)  # favor fast code
add_compiler_flag_if_supported (CMAKE_CXX_FLAGS /GL)  # whole program optimization
ENDIF()

add_definitions(-D_AMD64_)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBARY_OUTPUT_PATH bin)

set(BIN_DIR ${CMAKE_INSTALL_PREFIX})
set(CONF_DIR ${CMAKE_INSTALL_PREFIX})
set(LIBS_DIR ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INSTALL_RPATH ${LIBS_DIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)

OPTION(USE_SQL "Enable sql uid save ? (require mysql installed)" OFF)

macro(includePlatform SUFFIX)
  include("${CMAKE_SOURCE_DIR}/cmake/win32_${SUFFIX}.cmake")
  #storm lib
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/../Noggit3libs/StormLib/include/")
endmacro(includePlatform)

OPTION(VALIDATE_OPENGL_PROGRAMS "Validate Opengl programs" ON)


if(VALIDATE_OPENGL_PROGRAMS)
  add_definitions ( -DVALIDATE_OPENGL_PROGRAMS)
endif()

includePlatform("prefind")

FIND_PACKAGE( OpenGL REQUIRED )
FIND_PACKAGE( StormLib REQUIRED )
find_package (Qt5 COMPONENTS Widgets OpenGL OpenGLExtensions)


if (USE_SQL)
  find_library(MYSQL_LIBRARY
    NAMES libmysql
    HINTS "${CMAKE_SOURCE_DIR}/../Noggit3libs/mysql")
  find_library(MYSQLCPPCONN_LIBRARY
    NAMES mysqlcppconn
    HINTS "${CMAKE_SOURCE_DIR}/../Noggit3libs/mysql/connector")
  find_path (MYSQLCPPCONN_INCLUDE
    NAMES cppconn/driver.h
    HINTS "${CMAKE_SOURCE_DIR}/../Noggit3libs/mysql/connector")

  if (MYSQL_LIBRARY AND MYSQLCPPCONN_LIBRARY AND MYSQLCPPCONN_INCLUDE)
    add_definitions ( -DUSE_MYSQL_UID_STORAGE )

    set ( mysql_sources
          src/mysql/mysql.cpp
        )

    set ( mysql_headers
          src/mysql/mysql.h
        )
    source_group("mysql"  FILES ${mysql_sources} ${mysql_headers})
  else()
     message (FATAL_ERROR "MySQL lib or connector not found")
  endif()
endif()

add_subdirectory (src/external/qt-color-widgets)

# Add the found include directories to our include list.
include_directories (SYSTEM "${CMAKE_SOURCE_DIR}/include/")

OPTION(NOGGIT_ALL_WARNINGS "Enable all warnings?" OFF)

# Log to console for easier debugging.
OPTION( NOGGIT_LOGTOCONSOLE "Log to console instead of log.txt?" OFF )
IF( NOGGIT_LOGTOCONSOLE )
  MESSAGE( STATUS "And writing log to console instead of log.txt" )
  ADD_DEFINITIONS( -DDEBUG__LOGGINGTOCONSOLE )
ENDIF( NOGGIT_LOGTOCONSOLE )

# Disable opengl error log
OPTION(NOGGIT_OPENGL_ERROR_CHECK "Enable OpenGL error check ?" ON)
IF(NOT NOGGIT_OPENGL_ERROR_CHECK )
  MESSAGE( STATUS "OpenGL error check disabled." )
  ADD_DEFINITIONS( -DNOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS )
ENDIF()

includePlatform("postfind")

include_directories ("${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tmp")

# And do the job.
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src" )

set ( noggit_root_sources
      src/noggit/AsyncLoader.cpp
      src/noggit/Brush.cpp
      src/noggit/ChunkWater.cpp
      src/noggit/cursor_render.cpp
      src/noggit/DBC.cpp
      src/noggit/DBCFile.cpp
      src/noggit/Log.cpp
      src/noggit/MPQ.cpp
      src/noggit/MapChunk.cpp
      src/noggit/MapTile.cpp
      src/noggit/MapView.cpp
      src/noggit/Misc.cpp
      src/noggit/Model.cpp
      src/noggit/ModelInstance.cpp
      src/noggit/ModelManager.cpp
      src/noggit/Particle.cpp
      src/noggit/Sky.cpp
      src/noggit/TextureManager.cpp
      src/noggit/TileWater.cpp
      src/noggit/WMO.cpp
      src/noggit/WMOInstance.cpp
      src/noggit/World.cpp
      src/noggit/alphamap.cpp
      src/noggit/application.cpp
      src/noggit/camera.cpp
      src/noggit/error_handling.cpp
      src/noggit/liquid_layer.cpp
      src/noggit/liquid_render.cpp
      src/noggit/map_horizon.cpp
      src/noggit/map_index.cpp
      src/noggit/texture_set.cpp
      src/noggit/uid_storage.cpp
      src/noggit/wmo_liquid.cpp
      src/noggit/world_model_instances_storage.cpp
      src/noggit/world_tile_update_queue.cpp
	  src/noggit/Selection.cpp
    )

set ( noggit_ui_sources
      src/noggit/ui/About.cpp
      src/noggit/ui/clickable_label.cpp
      src/noggit/ui/CurrentTexture.cpp
      src/noggit/ui/CursorSwitcher.cpp
      src/noggit/ui/DetailInfos.cpp
      src/noggit/ui/FlattenTool.cpp
      src/noggit/ui/font_awesome.cpp
      src/noggit/ui/font_noggit.cpp
      src/noggit/ui/Help.cpp
      src/noggit/ui/HelperModels.cpp
      src/noggit/ui/ModelImport.cpp
      src/noggit/ui/ObjectEditor.cpp
      src/noggit/ui/RotationEditor.cpp
      src/noggit/ui/TexturePicker.cpp
      src/noggit/ui/texture_swapper.cpp
      src/noggit/ui/TexturingGUI.cpp
      src/noggit/ui/texturing_tool.cpp
      src/noggit/ui/Toolbar.cpp
      src/noggit/ui/SettingsPanel.cpp
      src/noggit/ui/Water.cpp
      src/noggit/ui/ZoneIDBrowser.cpp
      src/noggit/ui/main_window.cpp
      src/noggit/ui/minimap_widget.cpp
      src/noggit/ui/shader_tool.cpp
      src/noggit/ui/terrain_tool.cpp
      src/noggit/ui/uid_fix_window.cpp
      src/noggit/ui/texture_palette_small.cpp
      src/noggit/ui/TextureList.cpp
    )

set ( math_sources
      src/math/bounding_box.cpp
      src/math/frustum.cpp
      src/math/matrix_4x4.cpp
      src/math/ray.cpp
      src/math/vector_2d.cpp
    )

set ( opengl_sources
      src/opengl/context.cpp
      src/opengl/primitives.cpp
      src/opengl/shader.cpp
      src/opengl/texture.cpp
    )

set ( util_sources
      src/util/exception_to_string.cpp
    )

set ( noggit_root_headers
      src/noggit/Animated.h
      src/noggit/AsyncLoader.h
      src/noggit/AsyncObject.h
      src/noggit/Brush.h
      src/noggit/camera.hpp
      src/noggit/ChunkWater.hpp
      src/noggit/cursor_render.hpp
      src/noggit/DBC.h
      src/noggit/DBCFile.h
      src/noggit/Log.h
      src/noggit/MPQ.h
      src/noggit/map_enums.hpp
      src/noggit/MapChunk.h
      src/noggit/MapHeaders.h
      src/noggit/MapTile.h
      src/noggit/MapView.h
      src/noggit/Misc.h
      src/noggit/Model.h
      src/noggit/ModelHeaders.h
      src/noggit/ModelInstance.h
      src/noggit/ModelManager.h
      src/noggit/Particle.h
      src/noggit/Selection.h
      src/noggit/Sky.h
      src/noggit/TextureManager.h
      src/noggit/TileWater.hpp
      src/noggit/WMO.h
      src/noggit/WMOInstance.h
      src/noggit/World.h
      src/noggit/alphamap.hpp
      src/noggit/errorHandling.h
      src/noggit/liquid_layer.hpp
      src/noggit/liquid_render.hpp
      src/noggit/map_horizon.h
      src/noggit/map_index.hpp
      src/noggit/multimap_with_normalized_key.hpp
      src/noggit/texture_set.hpp
      src/noggit/tile_index.hpp
      src/noggit/tool_enums.hpp
      src/noggit/uid_storage.hpp
      src/noggit/wmo_liquid.hpp
      src/noggit/world_model_instances_storage.hpp
      src/noggit/world_tile_update_queue.hpp
	  src/pch/pch.h
    )

set ( noggit_ui_headers
      src/noggit/ui/About.h
      src/noggit/ui/clickable_label.hpp
      src/noggit/ui/CurrentTexture.h
      src/noggit/ui/CursorSwitcher.h
      src/noggit/ui/DetailInfos.h
      src/noggit/ui/FlattenTool.hpp
      src/noggit/ui/font_awesome.hpp
      src/noggit/ui/font_noggit.hpp
      src/noggit/ui/Help.h
      src/noggit/ui/HelperModels.h
      src/noggit/ui/ModelImport.h
      src/noggit/ui/ObjectEditor.h
      src/noggit/ui/RotationEditor.h
      src/noggit/ui/TexturePicker.h
      src/noggit/ui/texture_swapper.hpp
      src/noggit/ui/TexturingGUI.h
      src/noggit/ui/texturing_tool.hpp
      src/noggit/ui/Toolbar.h
      src/noggit/ui/SettingsPanel.h
      src/noggit/ui/Water.h
      src/noggit/ui/ZoneIDBrowser.h
      src/noggit/ui/main_window.hpp
      src/noggit/ui/minimap_widget.hpp
      src/noggit/ui/shader_tool.hpp
      src/noggit/ui/terrain_tool.hpp
      src/noggit/ui/uid_fix_window.hpp
      src/noggit/ui/texture_palette_small.hpp
      src/noggit/ui/TextureList.hpp
    )

set ( math_headers
      src/math/bounding_box.hpp
      src/math/constants.hpp
      src/math/frustum.hpp
      src/math/interpolation.hpp
      src/math/matrix_4x4.hpp
      src/math/projection.hpp
      src/math/quaternion.hpp
      src/math/ray.hpp
      src/math/trig.hpp
      src/math/vector_2d.hpp
      src/math/vector_3d.hpp
      src/math/vector_4d.hpp
    )

set ( opengl_headers
      src/opengl/context.hpp
      src/opengl/primitives.hpp
      src/opengl/scoped.hpp
      src/opengl/shader.fwd.hpp
      src/opengl/shader.hpp
      src/opengl/texture.hpp
      src/opengl/types.hpp
    )

set ( shaders
      src/glsl/m2_vert.glsl
      src/glsl/m2_frag.glsl
      src/glsl/m2_box_vert.glsl
      src/glsl/m2_box_frag.glsl
      src/glsl/particle_vert.glsl
      src/glsl/particle_frag.glsl
      src/glsl/ribbon_vert.glsl
      src/glsl/ribbon_frag.glsl
      src/glsl/terrain_vert.glsl
      src/glsl/terrain_frag.glsl
      src/glsl/wmo_vert.glsl
      src/glsl/wmo_frag.glsl
      src/glsl/liquid_vert.glsl
      src/glsl/liquid_frag.glsl
      src/glsl/mfbo_vert.glsl
      src/glsl/mfbo_frag.glsl
      src/glsl/cursor_vert.glsl
      src/glsl/cursor_frag.glsl
      src/glsl/horizon_vert.glsl
      src/glsl/horizon_frag.glsl
    )

IF(WIN32)
  set ( os_sources
        include/win/StackWalker.cpp
      )

  set ( os_headers
        include/win/StackWalker.h
      )
ENDIF(WIN32)


list (APPEND headers_to_moc
  src/noggit/MapView.h
  src/noggit/bool_toggle_property.hpp
  src/noggit/ui/terrain_tool.hpp
  src/noggit/ui/TexturePicker.h
  src/noggit/ui/TexturingGUI.h
  src/noggit/ui/Water.h
  src/noggit/ui/ZoneIDBrowser.h
  src/noggit/ui/clickable_label.hpp
  src/noggit/ui/minimap_widget.hpp
  src/noggit/ui/uid_fix_window.hpp
  src/noggit/ui/widget.hpp
  src/noggit/ui/texture_palette_small.hpp
  src/noggit/ui/TextureList.hpp
  src/noggit/ui/main_window.hpp
  src/noggit/unsigned_int_property.hpp
  src/noggit/ui/CurrentTexture.h
)
qt5_wrap_cpp (moced ${headers_to_moc} ${headers_to_moc})

source_group("noggit"  FILES ${noggit_root_sources} ${noggit_root_headers})
source_group("noggit\\ui"  FILES ${noggit_ui_sources} ${noggit_ui_headers})
source_group("opengl"  FILES ${opengl_sources} ${opengl_headers})
source_group("math"  FILES ${math_sources} ${math_headers})
source_group("external"  FILES ${external_sources} ${external_headers})
source_group("os"  FILES ${os_sources} ${os_headers})
source_group("util"  FILES ${util_sources})
source_group("glsl"  FILES ${shaders})

qt5_add_resources (compiled_resource_files "resources/resources.qrc")

ADD_EXECUTABLE ( noggit
                  WIN32
                  MACOSX_BUNDLE
                  ${noggit_root_sources}
                  ${noggit_ui_sources}
                  ${opengl_sources}
                  ${math_sources}
                  ${external_sources}
                  ${mysql_sources}
                  ${os_sources}
                  ${util_sources}
                  ${noggit_root_headers}
                  ${noggit_ui_headers}
                  ${opengl_headers}
                  ${math_headers}
                  ${external_headers}
                  ${mysql_headers}
                  ${os_headers}
                  ${ResFiles}
                  ${moced}
                  ${compiled_resource_files}
                  ${shaders}
                  ${force_update_file}
                )
                
TARGET_LINK_LIBRARIES (noggit
  ${OPENGL_LIBRARIES}
  StormLib
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::OpenGLExtensions
  ColorWidgets-qt5
)

set (_noggit_revision_output_dir "${CMAKE_BINARY_DIR}/revision_output")
set (_noggit_revision_template_file "${CMAKE_SOURCE_DIR}/cmake/revision.h.in")
set (_noggit_revision_output_file "${_noggit_revision_output_dir}/revision.h")
set (_noggit_revision_state_file "${CMAKE_BINARY_DIR}/revision.state")
set (_noggit_revision_script_file "${CMAKE_SOURCE_DIR}/cmake/GenerateRevision.cmake")

include_directories ("${_noggit_revision_output_dir}")

find_package (Git)
if (GIT_FOUND)
  add_custom_target (update_git_revision
    ALL
    DEPENDS "${_noggit_revision_template_file}"
            "${_noggit_revision_script_file}"
    BYPRODUCTS "${_noggit_revision_output_file}"
               "${_noggit_revision_state_file}"
    COMMAND ${CMAKE_COMMAND}
              -D_noggit_revision_template_file="${_noggit_revision_template_file}"
              -D_noggit_revision_output_file="${_noggit_revision_output_file}"
              -D_noggit_revision_state_file="${_noggit_revision_state_file}"
              -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
              -P "${_noggit_revision_script_file}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

  add_dependencies (noggit update_git_revision)
else()
  message (WARNING "Failed to find a Git executable, will NOT produce a "
    "useful version string. Crash logs will be useless. Do NOT distribute.")

  set (NOGGIT_GIT_VERSION_STRING "UNKNOWN")
  configure_file ("${_noggit_revision_template_file}"
                  "${_noggit_revision_output_file}" @ONLY)
endif()

if (MYSQL_LIBRARY AND MYSQLCPPCONN_LIBRARY AND MYSQLCPPCONN_INCLUDE)
  target_link_libraries (noggit ${MYSQL_LIBRARY} ${MYSQLCPPCONN_LIBRARY})
  target_include_directories (noggit SYSTEM PRIVATE ${MYSQLCPPCONN_INCLUDE})
endif()

if (NOGGIT_LOGTOCONSOLE)
  set_property (TARGET noggit APPEND PROPERTY LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
  set_property (TARGET noggit APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:"_CONSOLE">)
endif()

target_precompile_headers(noggit PRIVATE src/pch/pch.h)

includePlatform("pack")

add_library (noggit-math STATIC
  "src/math/matrix_4x4.cpp"
  "src/math/vector_2d.cpp"
)
add_library (noggit::math ALIAS noggit-math)

