#pragma once

#include "wave_builtin.h"
#include "iir.h"

namespace dsp
{
    template <typename Base = sawtoothWave>
    class RBJFilterWave : public Base, public RBJ
    {
    public:
        static constexpr auto signature = uti::STR2A("RBJFilterWave<") + Base::signature + uti::STR2A(">");
        WAVE_OPERATOR_OVERRIDE
        RBJFilterWave() = default;

        inline SampleType getSample()
        {
            //assume getSample is called only once and for every sample, as it is done in fillBuffer
            //the alternative implementation would be wasting a few cycles per sample
            // if (!cacheValid)
            // {
            //     cache = RBJ::filter(Base::getSample());
            //     cacheValid = true;
            // }
            // return cache;
            return RBJ::filter(Base::getSample());
        }

        inline void advance()
        {
            // cacheValid = false;
            Base::advance();
        }

    public:
        // SampleType cache = 0;
        // bool cacheValid = false;

        virtual int getParamCount() const  override { return 6 + Base::getParamCount(); }
        virtual SampleType* getParam(uint16_t index) override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParam(index);
            }
            index -= baseCount;
            return nullptr;
        }
        virtual const char *getParamName(uint16_t index) const override
        {
            const int baseCount = Base::getParamCount();
            if (index<baseCount)
            {
                return Base::getParamName(index);
            }
            index -= baseCount;
            if (index==0) return "RBJFilterWave::Type [0:11]";
            if (index==1) return "RBJFilterWave::Normalized Frequency [0:unsigned Max] --> [0:1[ normalized frequency";
            if (index==2) return "RBJFilterWave::Q [0:unsigned Max] --> [0:16[ Q factor";
            if (index==3) return "RBJFilterWave::Gain [Min:Max] --> [-32:32[ db gain";
            if (index==4) return "RBJFilterWave::Bandwidth [0:unsigned Max] --> [0:8[ octaves";
            if (index==5) return "RBJFilterWave::Slope [0:unsigned Max] --> [0:1] slope";
            return nullptr;
        }
    };
}