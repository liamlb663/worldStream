// src/Core/Types.hpp

#pragma once

#include <cstdint>
#include <cstddef>
#include <expected>
#include <optional>

// Unsigned integer types
using U8  = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;

// Signed integer types
using I8  = int8_t;
using I16 = int16_t;
using I32 = int32_t;
using I64 = int64_t;

// Floating-point types
using F32 = float;
using F64 = double;

// Platform-specific pointer and size types
using Size = size_t;

// Result type
template <typename T, typename E>
using Result = std::expected<T, E>;

// Option type
template <typename T>
using Option = std::optional<T>;

