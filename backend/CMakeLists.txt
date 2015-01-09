include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/Ubuntu_Tox_Client/include/
)


set(
    Ubuntu_Tox_Clientbackend_SRCS
    modules/Ubuntu_Tox_Client/backend.cpp
    modules/Ubuntu_Tox_Client/toxbackend.cpp
    modules/Ubuntu_Tox_Client/cdata.cpp
    modules/Ubuntu_Tox_Client/cstring.cpp
    modules/Ubuntu_Tox_Client/bootstrapnodes.h
)

add_library(Ubuntu_Tox_Clientbackend MODULE
    ${Ubuntu_Tox_Clientbackend_SRCS}
)

set_target_properties(Ubuntu_Tox_Clientbackend PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY Ubuntu_Tox_Client)

qt5_use_modules(Ubuntu_Tox_Clientbackend Gui Qml Quick)

# Set architecture

option(FOR_ARMHF "Build for the armhf architecture" off)
option(FOR_I386 "Build for the i386 architecture" off)
option(FOR_AMD64 "Build for the amd64 architecture" off)

if(FOR_I386)
    message(STATUS "Compile for i386")
    set(ARCH_DIR_NAME i386)
elseif(FOR_AMD64)
    message(STATUS "Compile for amd64")
    set(ARCH_DIR_NAME amd64)
else(FOR_I386)
    message(STATUS "Compile for armhf")
    set(ARCH_DIR_NAME armhf)
    set(FOR_ARMHF on)
endif(FOR_I386)

# Link toxcore library
target_link_libraries(Ubuntu_Tox_Clientbackend ${CMAKE_CURRENT_SOURCE_DIR}/modules/Ubuntu_Tox_Client/libs/${ARCH_DIR_NAME}/libtoxcore.so)

# Copy qmldir file to build dir for running in QtCreator
add_custom_target(Ubuntu_Tox_Clientbackend-qmldir ALL
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/modules/Ubuntu_Tox_Client/qmldir ${CMAKE_CURRENT_BINARY_DIR}/Ubuntu_Tox_Client
    DEPENDS ${QMLFILES}
)

# Install plugin file
install(TARGETS Ubuntu_Tox_Clientbackend DESTINATION ${QT_IMPORTS_DIR}/Ubuntu_Tox_Client/)
install(FILES   modules/Ubuntu_Tox_Client/qmldir DESTINATION ${QT_IMPORTS_DIR}/Ubuntu_Tox_Client/)

# Install libsodium library
install(FILES   modules/Ubuntu_Tox_Client/libs/${ARCH_DIR_NAME}/libsodium.so        DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/Ubuntu_Tox_Client/libs/${ARCH_DIR_NAME}/libsodium.so.13     DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/Ubuntu_Tox_Client/libs/${ARCH_DIR_NAME}/libsodium.so.13.0.2 DESTINATION ${QT_IMPORTS_DIR}/)

# Install toxcore library
install(FILES   modules/Ubuntu_Tox_Client/libs/${ARCH_DIR_NAME}/libtoxcore.so       DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/Ubuntu_Tox_Client/libs/${ARCH_DIR_NAME}/libtoxcore.so.0     DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/Ubuntu_Tox_Client/libs/${ARCH_DIR_NAME}/libtoxcore.so.0.0.0 DESTINATION ${QT_IMPORTS_DIR}/)

## Use C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
