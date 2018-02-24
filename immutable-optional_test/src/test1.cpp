#include <catch.hpp>
#include <immutable-optional/immutable-optional.h>

using namespace imm;

TEST_CASE("optional can be instantiated for <int>","[optional-instantiation]") {
    REQUIRE_NOTHROW(imm::optional<int> {});
}

TEST_CASE("optional default","[optional-instantiation]") {
    GIVEN("a default-initialized optional") {
        optional<int> opt;
        THEN("it is in the empty state by default") {
            CHECK(nothing == opt);
            REQUIRE(opt == nothing);
        }
    }
}


TEST_CASE("optional default ineq","[optional-instantiation]") {
    GIVEN("a default-initialized optional") {
        optional<int> opt;
        THEN("it is not different from the empty state") {
            CHECK(!(nothing != opt));
            REQUIRE(!(opt != nothing));
        }
    }
}


TEST_CASE("optional init","[optional-instantiation]") {
    GIVEN("a value-initialized optional") {
        optional<int> opt{42};
        THEN("it is not in the empty state") {
            CHECK(nothing != opt);
            REQUIRE(opt != nothing);
        }
    }
}



TEST_CASE("optional init value","[optional-instantiation]") {
    GIVEN("a value-initialized optional") {
        optional<int> opt{42};
        THEN("contains the value given upon initialization") {
            CHECK(*opt == 42);
            REQUIRE(42 == *opt);
        }
    }
}

TEST_CASE("optional value inequality","[optional-instantiation]") {
    GIVEN("a value-initialized optional") {
        optional<int> opt{42};
        THEN("is not equal to a value different from the initialization value") {
            CHECK(!(43 == *opt));
            REQUIRE(!(*opt == 43));
        }
    }
}

TEST_CASE("optional value get", "[optional-access]") {
    GIVEN("A value-initialized optional") {
        optional<int> opt{42};
        THEN("The value it contains may be accessed via .get()") {
            REQUIRE(42 == opt.get());
        }
    }
}

TEST_CASE("optional value get 2", "[optional-access]") {
    GIVEN("A value-initialized optional") {
        optional<int> opt{42};
        THEN("The value it contains may not be different from the initialization value") {
            REQUIRE(44 != opt.get());
        }
    }
}

TEST_CASE("optional initialization", "[optional-instantiation]") {
    GIVEN("A non-default initializable type") {
        struct has_no_default_constructor {
            has_no_default_constructor() = delete;
            has_no_default_constructor(has_no_default_constructor&&) = default;
            has_no_default_constructor(int){}
        };

        THEN("An optional may still be default-initialized") {
            optional<has_no_default_constructor> instance;
        }
    }
}

TEST_CASE("optional copy constructor", "[optional-construction]") {
    GIVEN("A properly initialized optional") {
        optional<int> orig{42};
        WHEN("a copy is constructed") {
            auto copy{orig};

            THEN("the copy equals the original") {
                REQUIRE(copy == orig);
            }
        }
    }
}


TEST_CASE("optional move constructor", "[optional-construction]") {
    GIVEN("A properly initialized optional") {
        optional<int> orig{42};
        WHEN("a copy is move-constructed") {
            auto move_copy{std::move(orig)};

            THEN("the copy contains the value from the original and the original is empty") {
                CHECK(*move_copy == 42);
                REQUIRE(orig == nothing);
            }
        }
    }
}

TEST_CASE("optional default value for get", "[optional-access]") {
    GIVEN("An empty optional") {
        optional<int> empty;
        WHEN("get is called with fallback value") {
            auto result = empty.get(/*default*/ 42);
            THEN("the fallback value is returned") {
                REQUIRE(42 == result);
            }
        }
    }
}


TEST_CASE("optional destructor", "[optional-destruct]") {
    struct some_type {
        bool* m_destructed;

        some_type(bool* destructed)
            : m_destructed{std::move(destructed)}
        {}

        ~some_type() {
            std::cout << "destroyed" << std::endl;
            *m_destructed = true;
        }
    };

    WHEN("An a non-empty optional goes out of scope") {
        bool destructed = false;
        {
            optional<some_type> opt{&destructed};
        }

        THEN("the destructor is called") {
            REQUIRE(destructed);
        }
    }
}


TEST_CASE("optional move from temporary", "[optional-move]") {
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

    WHEN("An a temporary optional is moved from") {

        int destructed = 0;
        int copied = 0;
        int moved = 0;
        {
            optional<some_type> opt{&destructed, &copied, &moved};
            some_type x = *std::move(opt);
        }

        THEN("the no copy is created, but the contained value is moved") {
            CHECK(moved == 1);
            REQUIRE(copied == 0);
        }
    }

    WHEN("An a temporary optional is moved from via get()") {
        int destructed = 0;
        int copied = 0;
        int moved = 0;
        {
            optional<some_type> opt{&destructed, &copied, &moved};
            some_type x = std::move(opt).get();
        }

        THEN("the no copy is created, but the contained value is moved") {
            CHECK(moved == 1);
            REQUIRE(copied == 0);
        }
    }

    WHEN("An a temporary optional is moved from via get()") {
        int destructed = 0;
        int copied = 0;
        int moved = 0;
        {
            some_type x = optional<some_type>{&destructed, &copied, &moved}.get();
        }

        THEN("the no copy is created, but the contained value is moved") {
            CHECK(moved == 1);
            REQUIRE(copied == 0);
        }
    }
}
