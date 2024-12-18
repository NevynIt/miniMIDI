#pragma once

#include "signature.h"

namespace uti
{
    class variant
    {
    public:
        using signature = uti::sign<uti::str("variant")>;
        static const uti::signature_id static_get_typeid() { return &signature::print; }
        virtual const uti::signature_id get_typeid() const { return static_get_typeid(); }
        virtual bool is_same_as(uti::signature_id s) const { return get_typeid() == s; }
        virtual uti::variant *clone() const { return nullptr; }
        virtual ~variant() {}

        template<typename T>
        inline T *as()
        {
            if (this->template is<T>())
                return static_cast<T *>(this);
            return nullptr;
        }

        template<typename T>
        inline const T *as() const
        {
            if (this->template is<T>())
                return static_cast<const T *>(this);
            return nullptr;
        }

        template<typename T>
        inline bool is() const
        {
            return this->is_same_as(&T::signature::print);
        }

        inline void print_signature() const
        {
            get_typeid()();
        }

        static inline void print_signature_static()
        {
            static_get_typeid()();
        }
    };

    #define variant_implementation \
        static const uti::signature_id static_get_typeid() { return &signature::print; } \
        virtual const uti::signature_id get_typeid() const override { return static_get_typeid(); } \
        virtual uti::variant *clone() const override { return (uti::variant *)(new std::remove_pointer_t<decltype(this)>(*this)); } \
        virtual bool is_same_as(uti::signature_id s) const override { return static_get_typeid() == s; }
    #define variant_inherit(_BASE_) \
        static const uti::signature_id static_get_typeid() { return &signature::print; } \
        virtual const uti::signature_id get_typeid() const override { return static_get_typeid(); } \
        virtual uti::variant *clone() const override { return (uti::variant *)(new std::remove_pointer_t<decltype(this)>(*this)); } \
        virtual bool is_same_as(uti::signature_id s) const override { return static_get_typeid() == s || _BASE_::is_same_as(s); }

}