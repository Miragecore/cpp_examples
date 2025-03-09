#include <iostream>
#include <variant>
#include <string>

// Define a variant type that can hold either an int or a string
using MyVariant = std::variant<int, std::string>;

// Define a visitor class that will handle different types in the variant and accept extra parameters
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

    // Extra argument
    std::string extra = "Extra information";

    // Use std::visit to apply the visitor to the variant and pass the extra parameter
    MyVisitor visitor;
    std::visit([&visitor, &extra](auto&& value) {
        visitor(std::forward<decltype(value)>(value), extra);  // Pass the extra argument to the visitor
    }, v1);  // Visiting the int variant

    std::visit([&visitor, &extra](auto&& value) {
        visitor(std::forward<decltype(value)>(value), extra);  // Pass the extra argument to the visitor
    }, v2);  // Visiting the string variant

    return 0;
}

