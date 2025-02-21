#include <iostream>
#include <variant>
#include <string>
#include <memory>

struct Storage{
  int iv;
  double dv;
  std::string sv;

    void print(){
    std::cout << iv << dv << sv << std::endl; 
  }
};

// 타입별 처리를 위한 함수 객체
struct Visitor {

    Storage& storage;

    Visitor(Storage& s) : storage(s){}

    void operator()(const int& i) const {
        storage.iv = i;
        std::cout << "Handling int: " << i << std::endl;
    }
    
    void operator()(const double d) const {
        storage.dv = d;
        std::cout << "Handling double: " << d << std::endl;
    }

    void operator()(const std::string& s) const {
        storage.sv = s;
        std::cout << "Handling string: " << s << std::endl;
    }
};

int main() {

    std::shared_ptr<Storage> storage = std::make_shared<Storage>();
    std::shared_ptr<Storage> storage1 = std::make_shared<Storage>();

    // std::variant에 다양한 타입 저장
    std::variant<int, double, std::string> var;

    Visitor vs(*storage1);

    var = 42;  // int 타입

    // std::visit을 사용하여 variant 값에 대한 처리를 자동으로 실행
    std::visit(Visitor(*storage), var);  // 타입에 맞게 자동으로 처리
    std::visit(vs, var);
     var = 3.14;  // double 타입
    std::visit(Visitor(*storage), var);  // 타입에 맞게 자동으로 처리
    std::visit(vs, var);
     var = "Hello, Variant!";  // string 타입
    std::visit(Visitor(*storage), var);  // 타입에 맞게 자동으로 처리
    std::visit(vs, var);
  
    storage->print();
    std::cout << "-\n";
    storage1->print();
    std::cout << "-\n";

    return 0;
}

