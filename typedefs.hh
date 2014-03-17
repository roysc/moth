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
#include <functional>

#include <cstdint>

// convenience
using std::string;
using std::set;
using std::vector;
using std::map;
using std::pair;
template <class K,class V> using table = std::unordered_map<K,V>;
using std::thread;
template <class F> using fn = std::function<F>;

template <class T> using ptr = T*; // can write "T*" or "ptr<T>", maybe remove
template <class T> using uptr = std::unique_ptr<T>;
template <class T> using sptr = std::shared_ptr<T>;
template <class T> using wptr = std::weak_ptr<T>;
