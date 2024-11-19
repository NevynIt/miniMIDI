#include "opModWave.h"

namespace dsp
{
    SampleType sample_dump = 0; //pointers will point here if undefined
    SampleType *lookupBuffers[256] = {nullptr};

} // namespace dsp