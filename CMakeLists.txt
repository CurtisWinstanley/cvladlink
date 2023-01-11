cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(link)

#add_executable(cvlad_link_testSend1 "cvlad_link_testSend.cpp")
#add_executable(cvlad_link_testSend "cvlad_link_testRecv.cpp")
#add_subdirectory(src)


############################
###  Configured Headers  ###
############################


#####################################
###  Global Compiler Definitions  ###
#####################################
# Cadmium


# Boost
# Enables thread pools


##########################
###  Dependency Setup  ###
##########################


##########################
###  Global Variables  ###
##########################
set(INCLUDES_LIST "${CMAKE_SOURCE_DIR}/deps")
set(SOCKET_SOURCES_LIST     "${CMAKE_SOURCE_DIR}/deps/socket.c")
set(LINK_PACKET_PROCESS     "${CMAKE_SOURCE_DIR}/deps/cvladlinkproc.cpp")
set(CVLAD_LINK    "${CMAKE_SOURCE_DIR}/deps/cvladlink.cpp")

#################################
###  Compiler Specific Fixes  ###
#################################


########################
###  Subdirectories  ###
########################
add_subdirectory(src)
#add_subdirectory(test)