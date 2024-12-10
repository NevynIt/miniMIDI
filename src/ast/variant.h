#pragma once

#include "helpers.h"

namespace variant
{
    class variant
    {
    public:
        static constexpr auto signature = STR2A("variant");
        virtual const char *get_signature() const { return signature.data(); }
        virtual bool is(const char *s) const { return get_signature() == s; }
        virtual ~variant() {}

        template<typename T>
        T *as()
        {
            if (this->template is<T>())
                return static_cast<T *>(this);
            return nullptr;
        }

        template<typename T>
        const T *as() const
        {
            if (this->template is<T>())
                return static_cast<const T *>(this);
            return nullptr;
        }

        template<typename T>
        bool is() const
        {
            return this->is(T::signature.data());
        }
    };
}