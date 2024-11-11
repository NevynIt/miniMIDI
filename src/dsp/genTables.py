#!/usr/bin/env python3
#------------------------------------------------------------------------------
# Copyright (c) 2023 John D. Haughton
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#------------------------------------------------------------------------------

# adapted from John D. Haughton use of platform library in picoX7
import os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

import math
import Table

SAMPLE_WIDTH = 32
BUFFER_SIZE_BITS = 10
BUFFER_SIZE = 1024

Table.gen('sinWave',
          bits       = SAMPLE_WIDTH,
          func       = lambda i,x : int(math.sin((i + 0.5) * 2 * math.pi / (1<<BUFFER_SIZE_BITS)) * (1 << (SAMPLE_WIDTH - 1)) + 0.5002),
          log2_size  = BUFFER_SIZE_BITS,
          typename   = "int" + str(SAMPLE_WIDTH)+ "_t",
          include    = "<stdint.h>",
          prefix     = '',
          namespace  = 'dsp',
          fmt        = '12d')
