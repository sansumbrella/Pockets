#pragma once

#include <stdint.h>
#include "entityx/config.h"

namespace entityx {

static const uint64_t MAX_COMPONENTS = 64;

}  // namespace entityx

#include <memory>

namespace entityx {

template <typename T>
using ptr = std::shared_ptr<T>;
template <typename T>
using weak_ptr = std::weak_ptr<T>;
template <typename T, typename U>
ptr<U> static_pointer_cast(const ptr<T> &ptr) {
  return std::static_pointer_cast<U>(ptr);
}
template <typename T>
using enable_shared_from_this = std::enable_shared_from_this<T>;

}  // namespace entityx


namespace entityx {

template <typename T>
bool operator == (const weak_ptr<T> &a, const weak_ptr<T> &b) {
  return a.lock() == b.lock();
}

template <typename T>
int use_count(const ptr<T> &ptr) {
  return ptr.use_count();
}

}  // namespace entityx
