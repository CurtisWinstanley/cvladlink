

add_executable(cvlad_link_testSend  "cvlad_link_testSend.cpp" "${SOCKET_SOURCES_LIST}" "${CVLAD_LINK}" "${LINK_PACKET_PROCESS}")
#message(message"${INCLUDES_LIST}")
include_directories(cvlad_link_testSend "${INCLUDES_LIST}")
add_executable(cvlad_link_testRecv  "cvlad_link_testRecv.cpp" "${SOCKET_SOURCES_LIST}" "${CVLAD_LINK}" "${LINK_PACKET_PROCESS}" )
include_directories(cvlad_link_testRecv "${INCLUDES_LIST}")