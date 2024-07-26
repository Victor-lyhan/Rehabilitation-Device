// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2022, Benoit BLANCHON
// MIT License

#pragma once

#include "../Namespace.hpp"
#include "../Polyfills/pgmspace.hpp"
#include "../Polyfills/type_traits.hpp"

namespace ARDUINOJSON_NAMESPACE {

template <typename T>
typename enable_if<is_pointer<T>::value, T>::type pgm_read(const void* p) {
  return reinterpret_cast<T>(pgm_read_ptr(p));
}

template <typename T>
typename enable_if<is_same<T, uint32_t>::value, T>::type pgm_read(
    const void* p) {
  return pgm_read_dword(p);
}

}  // namespace ARDUINOJSON_NAMESPACE
