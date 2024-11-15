//------------------------------------------------------------------------------
// Copyright (c) 2024 NevynIT
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------
//
// \brief Auto-generated by "C:\Stuff\miniMIDI\src\dsp\Table.py"
//

#pragma once

#include <cstdint>

namespace dsp::tables {

static const unsigned DSP_SIZE_LOG2 = 10;
static const unsigned DSP_SIZE      = 1 << DSP_SIZE_LOG2;
static const unsigned DSP_MASK      = DSP_SIZE - 1;
static const unsigned DSP_COUNT      = 4;
using DSP_TYPE = int16_t;

extern const int16_t sinWave[DSP_SIZE];
extern const int16_t triangleWave[DSP_SIZE];
extern const int16_t sawtoothWave[DSP_SIZE];
extern const int16_t squareWave[DSP_SIZE];
extern const int16_t *DSP[DSP_COUNT];

}
