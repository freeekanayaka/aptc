file(GLOB SRCS *.c)
add_library(aptc-dpkg SHARED ${SRCS})
set_target_properties(aptc-dpkg PROPERTIES VERSION 0.1)
set_target_properties(aptc-dpkg PROPERTIES SOVERSION 0)
