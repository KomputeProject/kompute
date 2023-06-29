#ifndef EXAMPLE_STRUCTURE_2_HPP
#define EXAMPLE_STRUCTURE_2_HPP

#if defined(__clang__) || defined(_MSC_VER) || defined(__ICC) || defined(__INTEL_COMPILER) || defined(__GNUC__) || defined(__GNUG__)
//c++ compiler
#else
//shader comipler
#extension GL_GOOGLE_include_directive : require
#endif

#include "StructTemplate.hpp"
#include "ExampleStructure1.hpp"

struct ExampleStructure2
{
    int8_t    a;
    int16_t   b;
    int32_t   c;
    int64_t   d;

    uint8_t   e;
    uint16_t  f;
    uint32_t  g;
    uint64_t  h;

    float32_t i;
    float64_t j;
    ExampleStructure1 k;
};
#endif

