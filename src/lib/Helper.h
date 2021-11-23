//
// Created by tomas on 31.10.21.
//

#pragma once

template<typename T>
struct identity {
    typedef T type;
};

template<typename A, typename B>
inline bool mapContainsKey(const std::map <A, B> &m, const std::string &str) {
    return m.find(str) != m.end();
}

template<typename A, typename B>
inline bool mapContainsKeyAndValue(const std::map <A, B> &m, const std::string& a, const std::string &b) {
    return m.find(a) != m.end() && m.find(a)->second == b;
}

template<typename A, typename B>
inline bool mapContainsKeyAndValue(const std::map <A, B> &m, const A& a, const B& b) {
    return m.find(a) != m.end() && m.find(a)->second == b;
}