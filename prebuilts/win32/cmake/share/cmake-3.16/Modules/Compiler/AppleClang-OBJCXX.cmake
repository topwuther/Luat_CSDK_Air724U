include(Compiler/Clang-OBJCXX)

set(CMAKE_OBJCXX_COMPILE_OPTIONS_VISIBILITY_INLINES_HIDDEN "-fvisibility-inlines-hidden")

if(NOT CMAKE_OBJCXX_COMPILER_VERSION VERSION_LESS 4.0)
  set(CMAKE_OBJCXX98_STANDARD_COMPILE_OPTION "-std=c++98")
  set(CMAKE_OBJCXX98_EXTENSION_COMPILE_OPTION "-std=gnu++98")
  set(CMAKE_OBJCXX98_STANDARD__HAS_FULL_SUPPORT ON)

  set(CMAKE_OBJCXX11_STANDARD_COMPILE_OPTION "-std=c++11")
  set(CMAKE_OBJCXX11_EXTENSION_COMPILE_OPTION "-std=gnu++11")
endif()

if(NOT CMAKE_OBJCXX_COMPILER_VERSION VERSION_LESS 6.1)
  set(CMAKE_OBJCXX14_STANDARD_COMPILE_OPTION "-std=c++14")
  set(CMAKE_OBJCXX14_EXTENSION_COMPILE_OPTION "-std=gnu++14")
  set(CMAKE_OBJCXX14_STANDARD__HAS_FULL_SUPPORT ON)
elseif(NOT CMAKE_OBJCXX_COMPILER_VERSION VERSION_LESS 5.1)
  # AppleClang 5.0 knows this flag, but does not set a __cplusplus macro greater than 201103L
  set(CMAKE_OBJCXX14_STANDARD_COMPILE_OPTION "-std=c++1y")
  set(CMAKE_OBJCXX14_EXTENSION_COMPILE_OPTION "-std=gnu++1y")
  set(CMAKE_OBJCXX14_STANDARD__HAS_FULL_SUPPORT ON)
endif()

if (NOT CMAKE_OBJCXX_COMPILER_VERSION VERSION_LESS 6.1)
  set(CMAKE_OBJCXX17_STANDARD_COMPILE_OPTION "-std=c++1z")
  set(CMAKE_OBJCXX17_EXTENSION_COMPILE_OPTION "-std=gnu++1z")
endif()

if (NOT CMAKE_OBJCXX_COMPILER_VERSION VERSION_LESS 8.0)
    set(CMAKE_OBJCXX11_STANDARD__HAS_FULL_SUPPORT ON)
endif()

if (NOT CMAKE_OBJCXX_COMPILER_VERSION VERSION_LESS 10.0)
  set(CMAKE_OBJCXX20_STANDARD_COMPILE_OPTION "-std=c++2a")
  set(CMAKE_OBJCXX20_EXTENSION_COMPILE_OPTION "-std=gnu++2a")
endif()

__compiler_check_default_language_standard(OBJCXX 4.0 98)
