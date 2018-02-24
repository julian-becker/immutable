#pragma once

namespace wrapper {

    namespace detail {
        template <typename Derived>
        struct Cast {
            using type = Derived;

            template <typename T>
            static constexpr Derived& self(T* self) { return *static_cast<Derived*>(self); }

            template <typename T>
            static constexpr Derived const& self(T const* self) { return *static_cast<Derived const*>(self); }
        };

        template <typename Cast, typename T,template <typename...> class...Mixins>
        struct wrapper_impl;

        template <typename Cast, typename T,template <typename...> class First, template <typename...> class...Rest>
        struct wrapper_impl<Cast, T, First, Rest...>
            : First<Cast, T>
            , wrapper_impl<Cast, T, Rest...>
        {
        };

        template <typename Cast, typename T, template <typename...> class First>
        struct wrapper_impl<Cast, T, First>
            : First<Cast, T>
            , wrapper_impl<Cast, T>
        {
        };

        template <typename Cast, typename T>
        struct wrapper_impl<Cast, T>  {
        };
    }


    template <typename T, typename Tag, template <typename...> class...Mixins>
    class wrapper : public detail::wrapper_impl<detail::Cast<wrapper<T,Tag,Mixins...>>, T, Mixins...> {
    public:
        using value_type = T;

        template <typename X, typename = std::enable_if_t<std::is_same<std::decay_t<X>,T>::value>>
        explicit wrapper(X&& value) {
            construct(std::forward<X>(value));
        }

        wrapper(wrapper const& other) {
            construct(other.get());
        }

        wrapper(wrapper&& other) {
            construct(std::move(other.get()));
        }

        // make it immutable
        wrapper& operator=(wrapper const& other) = delete;
        wrapper& operator=(wrapper&& other) = delete;

        T& get() { return *reinterpret_cast<T*>(&m_buffer); }
        T const& get() const { return *reinterpret_cast<T const*>(&m_buffer); }

        ~wrapper() {
            destruct();
        }

    private:

        template <typename...Args>
        void construct(Args&&...args) {
            new (&m_buffer) T(std::forward<Args>(args)...);
        }

        void destruct() {
            get().~T();
        }

        std::aligned_storage_t<sizeof(T), alignof(T)> m_buffer;;
    };

}
