cmake_minimum_required(VERSION 3.20)

set(easyrtmp_VERSION 0.1)
project(easyrtmp VERSION ${easyrtmp_VERSION})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_OPENSSL "Enable OpenSSL" OFF)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_sources(${PROJECT_NAME} PRIVATE
    amf.cpp include/amf.h
    include/EasyRtmpDLLAPI.h
    framework.cpp include/framework.h
    rtmp_endpoint.cpp include/rtmp_endpoint.h
    include/rtmp_enums.h
    rtmp_exception.cpp include/rtmp_exception.h
    rtmp_proto.cpp include/rtmp_proto.h
    rtmp_server_session.cpp include/rtmp_server_session.h
    rtmp_client_session.cpp include/rtmp_client_session.h
    utils.cpp include/utils.h
    data_layers/tcp_network.cpp include/data_layers/tcp_network.h
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE EASYRTMP_DLL_EXPORTS)
target_compile_definitions(${PROJECT_NAME} PUBLIC EASYRTMP_DLL)

if (${USE_OPENSSL})
    find_package(OpenSSL REQUIRED COMPONENTS SSL)
    target_link_libraries(${PROJECT_NAME}
        OpenSSL::SSL
        )
    target_sources(${PROJECT_NAME} PRIVATE
        data_layers/openssl_tls.cpp include/data_layers/openssl_tls.h
        )
    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_OPENSSL)
endif()
if (WIN32)
    target_link_libraries(${PROJECT_NAME}
        Ws2_32.lib
        Crypt32.lib
        )
else()
    target_link_libraries(${PROJECT_NAME}
        pthread
        )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (WIN32)
    set(LIB_DIRS "lib")
    set(RUNTIME_DIRS "bin")
else()
    set(LIB_DIRS "lib/easyrtmp")
    set(RUNTIME_DIRS "bin/easyrtmp")
endif()

install(DIRECTORY include/
    DESTINATION include/${PROJECT_NAME}
    )

install(TARGETS ${PROJECT_NAME}
    EXPORT easyrtmp_targets
    ARCHIVE DESTINATION ${LIB_DIRS}
    LIBRARY DESTINATION ${LIB_DIRS}
    RUNTIME DESTINATION ${RUNTIME_DIRS}
    )

install(EXPORT easyrtmp_targets
  FILE EasyRTMPTargets.cmake
  NAMESPACE EasyRTMP::
  DESTINATION lib/cmake/easyrtmp
)
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/EasyRTMPConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/easyrtmp"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/EasyRTMPConfigVersion.cmake"
  VERSION "${easyrtmp_VERSION_MAJOR}.${easyrtmp_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/EasyRTMPConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/EasyRTMPConfigVersion.cmake
  DESTINATION lib/cmake/easyrtmp
  )
if ($<$<CONFIG:Debug>:true>)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin)
endif()
