#include <cstdint>
#include <string>
#include <iostream>


uint16_t crc16arc_bit(uint16_t crc, const std::string& data){
	if(data.size() == 0){
		return 0;
	}
	for(const char& c : data){
		crc ^= c;	
		for(uint8_t k = 0; k < 8; k++){
      //poly : 8005
			crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		}
	}
	return crc;
}

std::string toHex(uint16_t value) {
    char buffer[5];  // 4자리 16진수 + NULL 문자
    std::sprintf(buffer, "%04X", value);  // 16진수 형식으로 포맷, 4자리로 맞춤
    return std::string(buffer);
}

int main(){

  std::string str = "123456789";

  uint16_t crc = crc16arc_bit(0, str);


  std::cout << toHex(crc)<< std::endl;
}
