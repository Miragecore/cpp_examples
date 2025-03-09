#include <iostream>
#include <variant>
#include <unordered_map>

using DATA_VAR = std::variant<int, float, std::string>;
using DATA_MAP = std::unordered_map<std::string, DATA_VAR>;


struct Visitor {

    Visitor(DATA_MAP& map) :
      map_(map){
    }

    DATA_MAP& map_;
    void operator()(int i) const { 
        std::cout << "int: " << i << std::endl;
        int ret = operator[](i);  // Call operator[] from operator()
        auto str = map_.find("string");
        if(str != map_.end()){
          std::visit(*this, str->second);
        }
    }
    //how to handle new message from multi source
    //if there is no updated ROS msg, take a data from protobuf map
    //it is problem, if there is not updated indiviual message -> how can I check it?

    void operator()(float f) const { 
        std::cout << "float: " << f << std::endl;
        operator[](f);  // Call operator[] from operator()
    }

    void operator()(const std::string& s) const { 
        std::cout << "string: " << s << std::endl;
        operator[](s);  // Call operator[] from operator()
    }

    // Custom operator[] for extra logic
    int operator[](int value) const {
        std::cout << "Accessing int through operator[]: " << value << std::endl;
        return 1;
    }

    void operator[](float value) const {
        std::cout << "Accessing float through operator[]: " << value << std::endl;
    }

    void operator[](const std::string& value) const {
        std::cout << "Accessing string through operator[]: " << value << std::endl;
    }
};

int main() {
    DATA_VAR v = 42;
    DATA_MAP map_;

    map_["int"] = 1;
    map_["float"] = 0.1f;
    map_["string"] = "string";

    Visitor vst(map_);

    std::visit(vst, map_["int"]);

  /*
    // Using std::visit with the Visitor
    std::visit(Visitor{}, v);  // Will call operator()(int)

    v = 3.14f;
    std::visit(Visitor{}, v);  // Will call operator()(float)

    v = "Hello, Variant!";
    std::visit(Visitor{}, v);  // Will call operator()(std::string)
  */
    return 0;
}

