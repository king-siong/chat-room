#ifndef _CODEC_H_
#define _CODEC_H_

#include <string>
#include <vector>
#include <bitset>

class Codec {
  private:
    std::string _msg;

  public:
    Codec();
    ~Codec();

    std::string encode(const std::string &);
    std::vector<std::string> decode(const std::string &);
};

#endif