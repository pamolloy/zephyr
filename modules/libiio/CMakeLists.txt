# Copyright (c) 2024 Analog Devices, Inc.
# Copyright (c) 2024 BayLibre SAS

if(CONFIG_LIBIIO)

option(BUILD_SHARED_LIBS OFF)
option(COMPILE_WARNING_AS_ERROR OFF)
option(CPP_BINDINGS OFF)
option(CSHARP_BINDINGS OFF)
option(ENABLE_IPV6 OFF)
option(ENABLE_PACKAGING OFF)
option(HAVE_DNS_SD OFF)
option(LIBIIO_COMPAT OFF)
# TODO(PM) Why set() rather than option()?
#option(NO_THREADS ON)
set(NO_THREADS ON)
option(PYTHON_BINDINGS OFF)
option(WITH_DOC OFF)
option(WITH_EXAMPLES OFF)
option(WITH_EXTERNAL_BACKEND ON)
option(WITH_GCOV OFF)
option(WITH_LIBTINYIIOD ON)
option(WITH_MAN OFF)
option(WITH_MODULES OFF)
option(WITH_NETWORK_BACKEND OFF)
option(WITH_SERIAL_BACKEND OFF)
option(WITH_USB_BACKEND OFF)
option(WITH_UTILS OFF)
option(WITH_XML_BACKEND OFF)
option(WITH_ZSTD OFF)

# For compatibility with older versions of CMake, option is clearing the normal variable 'NO_THREADS'.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# TODO(PM): `clock_gettime() implicitly defined` and temporarily fixed by
# including `<zephyr/posix/time.h>` and passing `.../zephyr/include` to
# `gcc -I`
# See https://github.com/zephyrproject-rtos/zephyr/issues/28124

# Copied from macro(zephyr_library_named name), but using add_subdirectory()
# rather than add_library() since libiio/CMakeLists.txt already calls
# add_library()
set(ZEPHYR_CURRENT_LIBRARY iio)
add_subdirectory(${ZEPHYR_CURRENT_MODULE_DIR} build)
zephyr_append_cmake_library(iio)
target_link_libraries(iio PUBLIC zephyr_interface)
target_include_directories(zephyr_interface INTERFACE
	${ZEPHYR_CURRENT_MODULE_DIR}/include)

# TODO(PM): These appear to be the standard way to include a library, but in
# this case we don't want to list all the files explicitly. We should be able
# to use the `add_library()` in libiio/CMakeLists.txt
#zephyr_library_named(iio)
#zephyr_include_directories()
#zephyr_library_sources()

endif()
