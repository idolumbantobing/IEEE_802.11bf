# Install script for directory: /home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.40-core-optimized.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.40-core-optimized.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.40-core-optimized.so"
         RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/build/lib/libns3.40-core-optimized.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.40-core-optimized.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.40-core-optimized.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.40-core-optimized.so"
         OLD_RPATH "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/build/lib:"
         NEW_RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.40-core-optimized.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/int64x64-128.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/example-as-test.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/helper/csv-reader.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/helper/event-garbage-collector.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/helper/random-variable-stream-helper.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/abort.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/ascii-file.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/ascii-test.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/assert.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/attribute-accessor-helper.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/attribute-construction-list.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/attribute-container.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/attribute-helper.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/attribute.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/boolean.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/breakpoint.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/build-profile.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/calendar-scheduler.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/callback.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/command-line.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/config.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/default-deleter.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/default-simulator-impl.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/deprecated.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/des-metrics.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/double.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/enum.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/event-id.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/event-impl.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/fatal-error.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/fatal-impl.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/fd-reader.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/environment-variable.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/global-value.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/hash-fnv.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/hash-function.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/hash-murmur3.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/hash.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/heap-scheduler.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/int-to-type.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/int64x64-double.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/int64x64.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/integer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/length.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/list-scheduler.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/log-macros-disabled.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/log-macros-enabled.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/log.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/make-event.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/map-scheduler.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/math.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/names.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/node-printer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/nstime.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/object-base.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/object-factory.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/object-map.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/object-ptr-container.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/object-vector.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/object.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/pair.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/pointer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/priority-queue-scheduler.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/ptr.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/random-variable-stream.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/rng-seed-manager.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/rng-stream.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/scheduler.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/show-progress.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/simple-ref-count.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/simulation-singleton.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/simulator-impl.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/simulator.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/singleton.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/string.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/synchronizer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/system-path.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/system-wall-clock-ms.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/system-wall-clock-timestamp.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/test.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/time-printer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/timer-impl.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/timer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/trace-source-accessor.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/traced-callback.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/traced-value.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/trickle-timer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/tuple.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/type-id.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/type-name.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/type-traits.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/uinteger.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/unused.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/valgrind.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/vector.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/warnings.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/watchdog.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/realtime-simulator-impl.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/wall-clock-synchronizer.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/val-array.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/src/core/model/matrix-array.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/build/include/ns3/config-store-config.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/build/include/ns3/core-config.h"
    "/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/build/include/ns3/core-module.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/idomanuel/workspace3.40pure_cobakan/ns-3-allinone/ns-3.40/cmake-cache/src/core/examples/cmake_install.cmake")

endif()

