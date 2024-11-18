#include "opModWave.h"

namespace dsp
{
    SampleType opModWave_global_1[bank_size] = {0}; // global scratchpad 0
    SampleType opModWave_global_2[bank_size] = {0}; // global scratchpad 1
    SampleType opModWave_dump = 0; //pointers will point here if undefined
    SampleType *lookupBuffers[256] = {nullptr};

} // namespace dsp