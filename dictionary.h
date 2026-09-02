#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <unordered_map>

class Trie {
private:
    std::unordered_map<char, Trie*> Childs;
    std::string Answer;

public:
    Trie();
    ~Trie();

    bool Add(const std::string_view& question, const std::string_view& answer, const size_t pos = 0);
    std::string FindPattern(const std::string& question, const size_t pos = 0) const;
};

class PatternDictionary {
private:
    Trie Root;

public:
    explicit PatternDictionary(const std::string& csvPath);
    ~PatternDictionary();

    std::string FindCommand(const std::string& question);
};

#endif // DICTIONARY_H
