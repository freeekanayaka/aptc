add_executable(aptc aptc.cc)
target_link_libraries(aptc apt-pkg aptc-private "${APT_PRIVATE_LIB}")
