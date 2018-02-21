#include <immutable-optional/export.h>
#include <memory>
#include <type_traits>
#include <iostream>

namespace imm {

    enum class NothingT {
        NOTHING
    };

    static constexpr auto nothing = NothingT::NOTHING;

    template <typename T>
    class optional {
    public:
        optional()
            : m_empty{true}
        {}

        optional(T value)
            noexcept(std::is_nothrow_move_constructible<T>::value)
            : m_empty{false}
        {
            new (&m_value) T(std::move(value));
        }

        template <typename ...Args>
        optional(Args&&... args)
            noexcept(std::is_nothrow_constructible<T,Args...>::value)
            : m_empty{false}
        {
            new (&m_value) T(std::forward<Args>(args)...);
        }

        optional(optional& other)
            noexcept(std::is_nothrow_copy_constructible<T>::value)
            : m_empty{other.m_empty}
        {
            if(!m_empty)
                new (&m_value) T(other.get());
        }

        optional(optional const& other)
            noexcept(std::is_nothrow_copy_constructible<T>::value)
            : m_empty{other.m_empty}
        {
            if(!m_empty)
                new (&m_value) T(other.get());
        }

        optional(optional&& other)
            noexcept(std::is_nothrow_move_constructible<T>::value)
            : m_empty{true}
        {
            using std::swap;
            swap(m_empty, other.m_empty);

            if(!m_empty)
                new (&m_value) T(std::move(other.value()));
        }

        ~optional() noexcept(std::is_nothrow_destructible<T>::value) {
            value().~T();
        }

        optional& operator= (optional const& other) = delete;

        optional& operator= (optional&& other) = delete;

        T const& get(T const& fallback) const {
            if(!m_empty)
                return get();

            return fallback;
        }

        T const& get() const {
            return *reinterpret_cast<T const*>(&m_value);
        }

        friend bool operator == (optional const& opt, NothingT) {
            return opt.m_empty;
        }

        friend bool operator == (NothingT, optional const& opt) {
            return opt.m_empty;
        }

        friend bool operator == (optional const& a, optional const& b) {
            return a.get() == b.get();
        }

        friend bool operator != (optional const& opt, NothingT) {
            return !opt.m_empty;
        }

        friend bool operator != (NothingT, optional const& opt) {
            return !opt.m_empty;
        }

        friend bool operator == (optional const& opt, T const& val) {
            return opt.get() == val;
        }

    private:

        T& value() {
            return *reinterpret_cast<T*>(&m_value);
        }

        std::aligned_storage_t<sizeof(T), alignof(T)> m_value;
        bool m_empty;
    };





}
