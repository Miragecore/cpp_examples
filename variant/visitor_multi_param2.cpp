#include <iostream>
#include <variant>
#include <string>

// Define a variant type that can hold either an int or a string
using MyVariant = std::variant<int, std::string>;

// Define a visitor class that will handle different types in the variant
class MyVisitor {
public:
    // This method will be called if the variant holds an int
    void operator()(int i, const std::string& extra) const {
        std::cout << "Processing int: " << i << " with extra: " << extra << std::endl;
    }

    // This method will be called if the variant holds a std::string
    void operator()(const std::string& str, const std::string& extra) const {
        std::cout << "Processing string: " << str << " with extra: " << extra << std::endl;
    }
};

int main() {
    // Create a variant holding an int
    MyVariant v1 = 42;
    // Create a variant holding a string
    MyVariant v2 = std::string("Hello, world!");

    // Extra argument to be passed to the visitor
    std::string extra = "Extra information";

    // Use std::visit with a lambda that passes the extra argument
    std::visit([&extra](auto&& value) {
        MyVisitor{}(std::forward<decltype(value)>(value), extra);  // Pass the extra argument
    }, v1);  // Visiting the int variant

    std::visit([&extra](auto&& value) {
        MyVisitor{}(std::forward<decltype(value)>(value), extra);  // Pass the extra argument
    }, v2);  // Visiting the string variant

    return 0;
}


