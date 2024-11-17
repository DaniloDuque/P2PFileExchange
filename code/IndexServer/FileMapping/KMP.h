#include <iostream>
#include <vector>
#include <string>
using namespace std;

void computeLPSArray(const string& pattern, vector<int>& lps) {
    int m = pattern.size();
    int length = 0;
    lps[0] = 0; 
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) length = lps[length - 1];
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

bool KMPIsSubstring(const string& text, const string& pattern) {
    int n = text.size(), m = pattern.size();
    if (m == 0) return true; 
    if (n == 0) return false; 
    vector<int> lps(m);
    computeLPSArray(pattern, lps);
    int i = 0, j = 0; 
    while (i < n) {
        if (pattern[j] == text[i]) i++, j++;
        if (j == m) return true;
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }return false;
}
