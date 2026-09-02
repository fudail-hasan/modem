#include "dictionary.h"
#include <cstdio>
#include <vector>
#include <string_view>
#include <iostream>

Trie::Trie() : Answer("") {}

Trie::~Trie() {
    for (auto& pair : Childs) {
        if (pair.second) delete pair.second; // delete calls destructor of Childs' nodes
    }
    Childs.clear();
}

bool Trie::Add(const std::string_view& question, const std::string_view& answer, const size_t pos) {
    if (pos == question.length()) {
        Answer = std::string(answer);
        return true;
    }

    char ch = question[pos];
    auto it = Childs.find(ch);
    if ( it == Childs.end()) {
        bool inserted = false;
        std::tie(it, inserted) = Childs.try_emplace(ch, new Trie());
        if (!inserted || it == nullptr || it->second == nullptr) {
            return false;
        }
    }
    return Childs[ch]->Add(question, answer, pos + 1);
}

std::string Trie::FindPattern(const std::string& question, const size_t pos) const {
    if (pos == question.length()) {
        if (!Answer.empty()) {
            return Answer;
        }
        auto it = Childs.find('*');
        if (it != Childs.end() && it->second) {
            return it->second->Answer;
        }
        return "";
    }

    // chars match
    char currentChar = question[pos];
    auto it = Childs.find(currentChar);
    std::string result = "";
    if (it != Childs.end()) {
        result = it->second->FindPattern(question, pos + 1);
        if (!result.empty()) return result;
    }

    // replace . by char
    it = Childs.find('.');
    if (it != Childs.end()) {
        result = it->second->FindPattern(question, pos + 1);
        if (!result.empty()) return result;
    }

    // replace * by 0 or more chars
    it = Childs.find('*');
    if (it != Childs.end()) {
        for (auto newPos = pos; newPos <= question.length(); ++newPos) {
            result = it->second->FindPattern(question, newPos);
            if (!result.empty()) return result;
        }
    }

    return "";
}

PatternDictionary::PatternDictionary(const std::string& csvPath) {
    FILE* file = std::fopen(csvPath.c_str(), "rb");
    if (!file) {
        std::cerr << "Error: Could not open configuration file " << csvPath << std::endl;
        return;
    }

    std::fseek(file, 0, SEEK_END);
    long fileSize = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);

    if (fileSize <= 0) {
        std::fclose(file);
        return;
    }

    std::vector<char> buffer(fileSize);
    std::size_t bytesRead = std::fread(buffer.data(), 1, fileSize, file);
    std::fclose(file);

    std::string_view fileContent(buffer.data(), bytesRead);
    size_t start = 0;

    while (start < fileContent.length()) {
        size_t end = fileContent.find('\n', start);
        if (end == std::string_view::npos) {
            end = fileContent.length();
        }

        std::string_view line = fileContent.substr(start, end - start);
        if (!line.empty() && line.back() == '\r') {
            line.remove_suffix(1);
        }
        if (!line.empty()) {
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string_view::npos) {
                std::string_view question = line.substr(0, delimiterPos);
                std::string_view answer = line.substr(delimiterPos + 1);
                if (!Root.Add(question, answer)) {
                    std::cerr << "failed to add command '" << question << "=" << answer << "'.\n";
                }
            }
        }
        start = end + 1;
    }
}

PatternDictionary::~PatternDictionary() {}

std::string PatternDictionary::FindCommand(const std::string& question) {
    return Root.FindPattern(question);
}
