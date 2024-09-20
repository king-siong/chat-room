#include "./codec.hh"

#include <iostream>
#include <cassert>

Codec::Codec() {}

Codec::~Codec() {}

// encode ``str'' into res and return res
std::string Codec::encode(const std::string &str) {
    std::string res = "";

    // TODO: your code here
    res = str;

    return res;
}

// online decoding, and now take ``str'' as input
// return a vector showing the decoded msgs
std::vector<std::string> Codec::decode(const std::string &str) {
    std::vector<std::string> res;

    // TODO: your code here
    res.push_back(str);

    return res;
}