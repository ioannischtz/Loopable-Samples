cmake_minimum_required(VERSION 3.1.0)

project(iir VERSION 1.9.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wconversion -Wextra -pedantic)
endif()

set(LIBSRC
        iir/Biquad.cpp
        iir/Butterworth.cpp
        iir/Cascade.cpp
        iir/ChebyshevI.cpp
        iir/ChebyshevII.cpp
        iir/Custom.cpp
        iir/PoleFilter.cpp
        iir/RBJ.cpp)

set(LIBINCLUDE
        iir/Biquad.h
        iir/Butterworth.h
        iir/Cascade.h
        iir/ChebyshevI.h
        iir/ChebyshevII.h
        iir/Common.h
        iir/Custom.h
        iir/Layout.h
        iir/MathSupplement.h
        iir/PoleFilter.h
        iir/RBJ.h
        iir/State.h
        iir/Types.h)

add_library(iir SHARED ${LIBSRC})
add_library(iir::iir ALIAS iir)


target_include_directories(iir
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>      # for Iir.h
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/iir>  # everything else
        )

add_library(iir_static STATIC ${LIBSRC})
add_library(iir::iir_static ALIAS iir_static)

target_include_directories(iir_static
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>      # for Iir.h
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/iir>  # everything else
        )