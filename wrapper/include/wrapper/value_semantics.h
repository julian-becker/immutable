
#pragma once
#include <memory> // for std::move
#include <iostream>

namespace wrapper {

    template <typename Cast, typename T>
    struct value_semantics {
        value_semantics() = default;

        value_semantics(value_semantics const& other) {
            std::cout << "VS: copy construct\n";
            Cast::self(this).construct(Cast::self(&other).get());
        }

        value_semantics(value_semantics&& other) {
            std::cout << "VS: move construct\n";
            Cast::self(this).construct(std::move(Cast::self(&other).get()));
        }

        value_semantics& operator=(value_semantics const& other) {
            std::cout << "VS: copy assign\n";
            Cast::self(this).get() = Cast::self(&other).get();
            return Cast::self(this);
        }

        value_semantics& operator=(value_semantics&& other) {
            std::cout << "VS: move assign\n";
            Cast::self(this).get() = std::move(Cast::self(&other).get());
            return Cast::self(this);
        }
    };
}
