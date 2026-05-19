#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#undef min
#undef max

#define _CONCAT(A, B) A ## B
#define CONCAT(A, B) _CONCAT(A, B)

#define _CONCAT3(A, B, C) A ## B ## C
#define CONCAT3(A, B, C) _CONCAT3(A, B, C)

#define PORT(x) CONCAT(PORT, x)
#define PIN(x) CONCAT(PIN, x)
#define DDR(x) CONCAT(DDR, x)
#define PCMSKx(x) CONCAT(PCMSK, x)
#define PCINTx_vect(x) CONCAT3(PCINT, x, _vect)

struct Fraction {
  uint16_t num = 0;
  uint16_t den = 1;

  Fraction() {}
  Fraction(uint16_t num) : num(num) {}
  Fraction(uint16_t num, uint16_t den) : num(num), den(den) {}
};

template<typename T, typename U>
struct Pair { T first; U second; };

template<typename T> struct Type { typedef T type; };
template<typename T> inline constexpr Type<T> type{};

template<size_t size> struct _Uint {};
template<> struct _Uint<1> { using type = uint8_t; };
template<> struct _Uint<2> { using type = uint16_t; };
template<> struct _Uint<4> { using type = uint32_t; };
template<> struct _Uint<8> { using type = uint64_t; };
template<size_t size> using Uint = typename _Uint<size>::type;

template<typename... Ts> struct _First { using type = void; };
template<typename T, typename... Ts> struct _First<T, Ts...> { using type = T; };
template<typename... Ts> using First = typename _First<Ts...>::type;

template<size_t i, typename... Ts>
struct _Index { static_assert(false, "index out of range"); using type = void; };
template<typename T, typename... Ts>
struct _Index<0, T, Ts...> { using type = T; };
template<size_t i, typename T, typename... Ts>
struct _Index<i, T, Ts...> { using type = typename _Index<i - 1, Ts...>::type; };
template<size_t i, typename... Ts> using Index = typename _Index<i, Ts...>::type;

template<size_t N, typename T = void>
struct ConstValue { static constexpr size_t value = N; };
template<size_t N, typename T = void>
inline constexpr size_t const_value = ConstValue<N, T>::value;

template<typename... Ts>
constexpr size_t count() {
  return (0 + ... + const_value<1, Ts>);
}

template<size_t... size>
constexpr size_t sum() {
  return (0 + ... + size);
}

template<size_t i, typename T, typename... Ts>
constexpr auto index(T arg, Ts... args) {
  if constexpr (i == 0) {
    return arg;
  } else {
    return index<i - 1>(args...);
  }
}

template<typename T>
constexpr T max(const T& x, const T& y) {
  return x > y ? x : y;
}

template<typename T>
constexpr T min(const T& x, const T& y) {
  return x < y ? x : y;
}

template<typename T>
constexpr T clamp(const T& x, const T& min_, const T& max_) {
  return x < min_ ? min_ : x > max_ ? max_ : x;
}

template<typename T>
constexpr uint8_t ilog2(T x) {
  uint8_t res = 0;
  while (x >>= 1) {
    ++res;
  }
  return res;
}

template<typename T>
constexpr size_t minsizeof(const T& value) {
  return value == uint8_t(value) ? 1
    : value == uint16_t(value) ? 2
    : value == uint32_t(value) ? 4
    : sizeof(value);
}

template<typename T>
T mix(const T& x, const T& y, const Fraction& f) {
  return (x * uint32_t(f.den - f.num) + y * uint32_t(f.num)) / f.den;
}

template<typename T, typename U>
constexpr auto make_pair(const T& first, const U& second) {
  return Pair<T, U>{first, second};
}

template<typename T>
T operator*(const T& x, const Fraction& f) { return (x * uint32_t(f.num)) / f.den; }
template<typename T>
T operator*=(T& x, const Fraction& f) { x = x * f; return x; }

template<typename T>
T operator/(const T& x, const Fraction& f) { return (x * uint32_t(f.den)) / f.num; }
template<typename T>
T operator/=(const T& x, const Fraction& f) { x = x / f; return x; }

#endif // COMMON_H_INCLUDED
