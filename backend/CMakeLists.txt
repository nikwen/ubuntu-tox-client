include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/UbuntuToxClient/include/
)


set(
    UbuntuToxClientbackend_SRCS
    modules/UbuntuToxClient/backend.cpp
    modules/UbuntuToxClient/toxbackend.cpp
    modules/UbuntuToxClient/cdata.cpp
    modules/UbuntuToxClient/cstring.cpp
    modules/UbuntuToxClient/contactsmodel.cpp
    modules/UbuntuToxClient/friend.cpp
    modules/UbuntuToxClient/message.cpp
    modules/UbuntuToxClient/typeidbaseclass.cpp
    modules/UbuntuToxClient/bootstrapnodes.h
)

add_library(UbuntuToxClientbackend MODULE
    ${UbuntuToxClientbackend_SRCS}
)

set_target_properties(UbuntuToxClientbackend PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY UbuntuToxClient)

qt5_use_modules(UbuntuToxClientbackend Gui Qml Quick)

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
target_link_libraries(UbuntuToxClientbackend ${CMAKE_CURRENT_SOURCE_DIR}/modules/UbuntuToxClient/libs/${ARCH_DIR_NAME}/libtoxcore.so)

# Copy qmldir file to build dir for running in QtCreator
add_custom_target(UbuntuToxClientbackend-qmldir ALL
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/modules/UbuntuToxClient/qmldir ${CMAKE_CURRENT_BINARY_DIR}/UbuntuToxClient
    DEPENDS ${QMLFILES}
)

# Install plugin file
install(TARGETS UbuntuToxClientbackend DESTINATION ${QT_IMPORTS_DIR}/UbuntuToxClient/)
install(FILES   modules/UbuntuToxClient/qmldir DESTINATION ${QT_IMPORTS_DIR}/UbuntuToxClient/)

# Install libsodium library
install(FILES   modules/UbuntuToxClient/libs/${ARCH_DIR_NAME}/libsodium.so        DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/UbuntuToxClient/libs/${ARCH_DIR_NAME}/libsodium.so.13     DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/UbuntuToxClient/libs/${ARCH_DIR_NAME}/libsodium.so.13.0.2 DESTINATION ${QT_IMPORTS_DIR}/)

# Install toxcore library
install(FILES   modules/UbuntuToxClient/libs/${ARCH_DIR_NAME}/libtoxcore.so       DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/UbuntuToxClient/libs/${ARCH_DIR_NAME}/libtoxcore.so.0     DESTINATION ${QT_IMPORTS_DIR}/)
install(FILES   modules/UbuntuToxClient/libs/${ARCH_DIR_NAME}/libtoxcore.so.0.0.0 DESTINATION ${QT_IMPORTS_DIR}/)

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
