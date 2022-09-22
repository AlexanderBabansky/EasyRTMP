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
    rtmp_exception.cpp include/rtmp_exception.h
    rtmp_proto.cpp include/rtmp_proto.h
    rtmp_server_session.cpp include/rtmp_server_session.h
    rtmp_client_session.cpp include/rtmp_client_session.h
    utils.cpp include/utils.h
    data_layers/tcp_network.cpp include/data_layers/tcp_network.h
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE EARYRTMP_DLL_EXPORTS)
target_compile_definitions(${PROJECT_NAME} PUBLIC EARYRTMP_DLL)

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

target_include_directories(${PROJECT_NAME} PRIVATE include)

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

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/easyrtmp/EasyRTMPConfigVersion.cmake"
    VERSION ${easyrtmp_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

export(EXPORT easyrtmp_targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/easyrtmp/EasyRTMPTargets.cmake"
    NAMESPACE EasyRTMP::
)
configure_file(cmake/EasyRTMPConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/easyrtmp/EasyRTMPConfig.cmake"
)

set(ConfigPackageLocation lib/cmake/EasyRTMP)
install(EXPORT easyrtmp_targets
    FILE
    EasyRTMPTargets.cmake
    NAMESPACE
    EasyRTMP::
    DESTINATION
    ${ConfigPackageLocation}
    )
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/easyrtmp/EasyRTMPConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/easyrtmp/EasyRTMPConfigVersion.cmake"
    DESTINATION
    ${ConfigPackageLocation}
    )
