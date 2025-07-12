#pragma once
#include "Encoder.h"
using namespace std;

class Base64Encoder final : public Encoder {
public:
    string encode(const string& data) override {
        const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        string result;
        int val = 0, valb = -6;
        for (unsigned char c : data) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                result.push_back(chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) result.push_back(chars[(val << (8 + valb)) & 0x3F]);
        while (result.size() % 4) result.push_back('=');
        return result;
    }

    string decode(const string& data) override {
        const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        string result;
        int val = 0, valb = -8;
        for (char c : data) {
            if (c == '=') break;
            const auto pos = chars.find(c);
            if (pos == string::npos) continue;
            val = (val << 6) + static_cast<int>(pos);
            valb += 6;
            if (valb >= 0) {
                result.push_back(static_cast<char>((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return result;
    }

};