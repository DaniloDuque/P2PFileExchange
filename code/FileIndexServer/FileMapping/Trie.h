#include "../../util.h"
#include "File.h"
#include <unordered_map>

template<typename T>
class Trie {
private:

    struct node{
        char data;
        File<T> * file;
        unordered_map<char, node*> nxt;
        bool isEnd;
        node(char d) : data(d) {} 
    };

    node* root;
    vector<File<T>*> matches;

    void findMatches(node* cur, string &name){
        if(cur==nullptr) return;
        if(cur.data==name[0]){
            node* nw = findNode(cur, name.substr(1, name.size()));
            if(nw!=nullptr) matches.push_back(nw.file);
        }
        for(auto &[_, next] : cur->nxt) findMatches(next, name);
    }

    node* findNode(node* cur, string &name){
        for(char &l : name){
            if(!cur->nxt.count(l)) return nullptr;
            cur = cur->nxt[l];
        }return cur;
    }

public:

    Trie() {root = new node('\0');}

    void insert(File<T> * f) {
        node* cur = root;
        for(char &l : f.filename) {
            if(!cur->nxt.count(l)) cur->nxt[l] = new node(l);
            cur = cur->nxt[l];
        }cur->isEnd = 1;
        cur->file = f;
    }

    vector<File<T>*> search(string &word) {
        matches.clear(); node* cur = root;
        findMatches(cur, word);
        return matches;
    }

};
