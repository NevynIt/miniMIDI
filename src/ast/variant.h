#pragma once

#include "helpers.h"

namespace variant
{
    class variant
    {
    public:
        using signature = signing::sign<signing::cs("variant")>;
        static const signing::signature_id static_get_typeid() { return &signature::print; }
        virtual const signing::signature_id get_typeid() const { return static_get_typeid(); }
        virtual bool is_same_as(signing::signature_id s) const { return get_typeid() == s; }
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
            return this->is_same_as(&T::signature::print);
        }
    };
}