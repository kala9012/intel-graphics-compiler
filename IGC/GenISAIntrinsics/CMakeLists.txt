
set(IGC_BUILD__PROJ__GenISAIntrinsics       "${IGC_BUILD__PROJ_NAME_PREFIX}GenISAIntrinsics")
set(IGC_BUILD__PROJ__GenISAIntrinsics       "${IGC_BUILD__PROJ__GenISAIntrinsics}" PARENT_SCOPE)

set(IGC_BUILD__PROJ_LABEL__GenISAIntrinsics "${IGC_BUILD__PROJ__GenISAIntrinsics}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

# IntrinsicGenISA.gen is generated by python script and will be included
# by GenIntrinsics.h
set(_fileOutput "${IGC_OPTION__OUTPUT_DIR}/${CMAKE_CFG_INTDIR}/IntrinsicGenISA.gen")

set(IGC_BUILD__SRC__GenISAIntrinsics
    "${CMAKE_CURRENT_SOURCE_DIR}/GenIntrinsics.cpp"
  )

set(IGC_BUILD__HDR__GenISAIntrinsics
    "${CMAKE_CURRENT_SOURCE_DIR}/GenIntrinsicInst.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/GenIntrinsics.h"
    "${_fileOutput}"
  )

add_library("${IGC_BUILD__PROJ__GenISAIntrinsics}" STATIC
    ${IGC_BUILD__SRC__GenISAIntrinsics}
    ${IGC_BUILD__HDR__GenISAIntrinsics}
  )
set_property(TARGET "${IGC_BUILD__PROJ__GenISAIntrinsics}" PROPERTY
    PROJECT_LABEL "${IGC_BUILD__PROJ_LABEL__GenISAIntrinsics}")

# ===========================================================================
# ===================================== BUILD STEP CONFIGURATION ============
# ===========================================================================


set(_intrinsicScript "${IGC_SOURCE_DIR}/GenISAIntrinsics/Intrinsics.py")

set(_InstCmdline)
    set(_fileInput "Intrinsic_definitions.py")

list(APPEND _InstCmdline ${_fileInput})
add_custom_command(OUTPUT "${_fileOutput}"
    COMMAND "${PYTHON_EXECUTABLE}"
    ARGS ${_intrinsicScript} ${_InstCmdline} ${_fileOutput}
    DEPENDS "${_intrinsicScript}" ${_fileInput}
    COMMENT "Generating Gen intrinsic definition file: ${_fileOutput}"
  )

add_dependencies("${IGC_BUILD__PROJ__GenISAIntrinsics}" "${_fileOutput}")

#set up standard defines from the common WDK path.
if(MSVC)
    bs_set_wdk(${IGC_BUILD__PROJ__GenISAIntrinsics})
    set_target_properties(${IGC_BUILD__PROJ__GenISAIntrinsics} PROPERTIES VS_PLATFORM_TOOLSET WindowsApplicationForDrivers10.0)
    set_target_properties(${IGC_BUILD__PROJ__GenISAIntrinsics} PROPERTIES VS_GLOBAL_DriverTargetPlatform Universal)
    set_target_properties(${IGC_BUILD__PROJ__GenISAIntrinsics} PROPERTIES VS_TARGET_PLATFORM_VERSION "")
    windows_libs_props_universal(${IGC_BUILD__PROJ__GenISAIntrinsics})
endif()


# add dependency for generating llvm intrinsics
if(TARGET intrinsics_gen)
    add_dependencies("${IGC_BUILD__PROJ__GenISAIntrinsics}" intrinsics_gen)
endif()
win_static_runtime()
