// -*- coding: utf-8 -*-
// typedefs.hh
// \brief type aliases
#pragma once

#include <string>
#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <thread>
#include <utility>

// convenience
using std::string;
using std::set;
template <class T> using v = std::vector<T>;
template <class K,class V> using m = std::map<K,V>;
using std::pair;
template <class K,class V> using table = std::unordered_map<K,V>;
using std::thread;

template <class T> using ptr = T*; // can write "T*" or "ptr<T>", maybe remove
template <class T> using uptr = std::unique_ptr<T>;
template <class T> using sptr = std::shared_ptr<T>;
template <class T> using wptr = std::weak_ptr<T>;
