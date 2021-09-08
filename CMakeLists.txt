cmake_minimum_required(VERSION 3.13)

set(easyrtmp_VERSION 0.1)
project(easyrtmp VERSION ${easyrtmp_VERSION})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_OPENSSL "Enable OpenSSL" OFF)

set (SOURCES amf.cpp framework.cpp rtmp_endpoint.cpp rtmp_exception.cpp rtmp_proto.cpp rtmp_server_session.cpp rtmp_client_session.cpp utils.cpp data_layers/tcp_network.cpp)

if (${USE_OPENSSL})
	find_package(OpenSSL REQUIRED COMPONENTS SSL)
	set(LIBS ${LIBS} OpenSSL::SSL)
	set(SOURCES ${SOURCES} data_layers/openssl_tls.cpp)
	add_definitions(-DUSE_OPENSSL)
endif()
if (WIN32)
	set(LIBS ${LIBS} Ws2_32.lib Crypt32.lib)
else()
	set(LIBS ${LIBS} pthread)
endif()

add_library(easyrtmp STATIC ${SOURCES})
target_link_libraries(easyrtmp PUBLIC ${LIBS})
target_include_directories(easyrtmp PRIVATE include)

set(INCLUDE_DIRS "include")
if (WIN32)
	set(LIB_DIRS "lib")
	set(RUNTIME_DIRS "bin")
else()
	set(LIB_DIRS "lib/easyrtmp")
	set(RUNTIME_DIRS "bin/easyrtmp")
endif()
	
install(DIRECTORY include/
	DESTINATION ${INCLUDE_DIRS}/easyrtmp)
	
install(TARGETS easyrtmp
	EXPORT easyrtmp_targets
	ARCHIVE DESTINATION ${LIB_DIRS}
	LIBRARY DESTINATION ${LIB_DIRS}
	RUNTIME DESTINATION ${RUNTIME_DIRS})
	
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