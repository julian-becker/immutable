#include <catch.hpp>
#include <wrapper/wrapper.h>
#include <wrapper/value_semantics.h>
#include <iostream>

using namespace wrapper;

struct some_type { double value; };
static constexpr double some_value = 123.45;

template <typename Cast, typename T>
struct mixin_conversion_to_sometype {
    operator some_type() const {
        auto value = Cast::self(this).get();
        return some_type{ value };
    }
};

template <typename Cast, typename T>
struct mixin_conversion_to_int {
    operator int() const {
        auto value = Cast::self(this).get();
        return static_cast<int>(value.value);
    }
};

TEST_CASE("test wrapper<•> construction", "[wrapper]") {
    using wrapped = wrapper<some_type, struct SomeTypeTag>;

    GIVEN("Some type `some_type`") {
        auto raw_instance = some_type{some_value};

        THEN("wrapper<some_type> can be copy-constructed from a value of some_type") {
            wrapped instance{raw_instance};
            REQUIRE(instance.get().value == some_value);
        }

        THEN("wrapper<some_type> can be move-constructed from a value of some_type") {
            wrapped instance{std::move(raw_instance)};
            REQUIRE(instance.get().value == some_value);
        }

    }

    GIVEN("An instance of `wrapper<some_type>`") {
        wrapped instance{some_type{some_value}};

        THEN("Another instance can be copy-constructed") {
            wrapped copy{instance};
            REQUIRE(copy.get().value == instance.get().value);
        }

        THEN("Another instance can be move-constructed") {
            wrapped copy{instance};
            REQUIRE(copy.get().value == instance.get().value);
        }
    }
}

TEST_CASE("test wrapper<•> conversion", "[wrapper-conversion]") {
    GIVEN("Some type `some_type` and a mixin implementing a conversion operator to `some_type`") {
        /// cannot write templates here :-( ... see definition of mixin_conversion_to_sometype above.
        using wrapped = wrapper<some_type, struct SomeTypeTag, mixin_conversion_to_sometype>;

        WHEN("a `wrapped` instance with `mixin_conversion_to_sometype` mixed in is instantiated") {
            auto instance = wrapped{ some_type{some_value} };
            THEN("the instance can be converted to `some_type`") {
                some_type converted = instance;
                REQUIRE(converted.value == some_value);
            }
        }
    }

    GIVEN("A wrapped instance with mixins implementing conversion to `some_type` and conversion to `int`") {
        using wrapped = wrapper<some_type, struct SomeTypeTag, mixin_conversion_to_sometype, mixin_conversion_to_int>;
        auto instance = wrapped{ some_type{some_value} };

        THEN("the instance can be converted to `some_type`") {
            some_type converted = instance;
            REQUIRE(converted.value == some_value);
        }

        THEN("the instance can be converted to `int`") {
            int converted = instance;
            REQUIRE(converted == static_cast<int>(some_value));
        }
    }

    GIVEN("A copyable class") {
        struct some_type {
            int* m_destructed;
            int* m_copied;
            int* m_moved;

            some_type(int* destructed, int* copied, int* moved)
                : m_destructed{std::move(destructed)}
                , m_copied{std::move(copied)}
                , m_moved{std::move(moved)}
            {}

            some_type(some_type const& other)
                : m_destructed{other.m_destructed}
                , m_copied{std::move(other.m_copied)}
                , m_moved{std::move(other.m_moved)}
            {
                ++*(m_copied);
            }

            some_type& operator= (some_type const& other) {
                ++*(m_copied);
                ++*(other.m_copied);
                return *this;
            }

            some_type& operator= (some_type const&& other) {
                ++*m_moved;
                ++*(other.m_moved);
                return *this;
            }

            some_type(some_type&& other)
                : m_destructed{other.m_destructed}
                , m_copied{std::move(other.m_copied)}
                , m_moved{std::move(other.m_moved)}
            {
                ++*m_moved;
            }

            ~some_type() {
                ++*m_destructed;
            }
        };

        WHEN("A wrapper of this copyable type is copied, the copy-constructor is called") {
            using t = wrapper<some_type, struct SomeTag>;
            int moved = 0, copied = 0, destructed = 0;
            {
                t instance{some_type{&destructed, &copied, &moved}};
                t copy = instance;
            }
            REQUIRE(copied == 1);
        }
    }


    GIVEN("A movable class") {
        struct some_type {
            int* m_destructed;
            int* m_copied;
            int* m_moved;

            some_type(int* destructed, int* copied, int* moved)
                : m_destructed{std::move(destructed)}
                , m_copied{std::move(copied)}
                , m_moved{std::move(moved)}
            {
            }

            some_type(some_type const& other)
                : m_destructed{other.m_destructed}
                , m_copied{std::move(other.m_copied)}
                , m_moved{std::move(other.m_moved)}
            {
                ++*(m_copied);
            }

            some_type& operator= (some_type const& other) {
                ++*(m_copied);
                ++*(other.m_copied);
                return *this;
            }

            some_type& operator= (some_type&& other) {
                ++*m_moved;
                ++*(other.m_moved);
                return *this;
            }

            some_type(some_type&& other)
                : m_destructed{other.m_destructed}
                , m_copied{other.m_copied}
                , m_moved{other.m_moved}
            {
                ++(*m_moved);
            }

            ~some_type() {
                ++*m_destructed;
            }
        };

        WHEN("A wrapper of this movable type is moved, the move-constructor is called") {
            using t = wrapper<some_type, struct SomeTag>;
            int moved = 0, copied = 0, destructed = 0;
            {
                t instance{some_type{&destructed, &copied, &moved}};
                moved = 0;
                copied = 0;
                t other(std::move(instance));
                CHECK(copied == 0);
                REQUIRE(moved == 1);
            }
        }
    }









}
