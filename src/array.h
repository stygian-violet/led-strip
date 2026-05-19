#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#include "common.h"

template<typename T, size_t N>
class StaticArray {
protected:
  T data[N];

public:
  constexpr StaticArray(const T data_[N]);

  constexpr size_t size() const { return N; }
  constexpr const T& operator[](size_t i) const { return data[i]; }

  void read_P(size_t i, T *dst) const;
  T read_P(size_t i) const { T res; read_P(i, &res); return res; }
};

template<typename T, size_t N, size_t K>
class StaticArray2D {
protected:
  T data[N];
  size_t offset[K];

public:
  constexpr StaticArray2D(const StaticArray<T, N>& arr, const StaticArray<size_t, K>& off);

  constexpr size_t size() const { return N; };
  constexpr size_t rows() const { return K; };
  constexpr size_t min_row_size() const;
  constexpr size_t max_row_size() const;

  size_t read_P(size_t row, size_t max_row_size, T *dst) const;
  Pair<const T*, size_t> read_P(size_t row) const;
};

template<typename T> struct _ArrayType {};
template<typename T, size_t N> struct _ArrayType<T[N]> { typedef T type; };
template<typename T, size_t N> struct _ArrayType<StaticArray<T, N> > { typedef T type; };
template<typename T> using ArrayType = _ArrayType<T>::type;

template<typename T, size_t N>
constexpr size_t array_size(Type<T[N]>) { return N; }
template<typename T, size_t N>
constexpr size_t array_size(Type<StaticArray<T, N> >) { return N; }
template<typename T, size_t N>
constexpr size_t array_size(Type<const StaticArray<T, N> >) { return N; }

template<typename T, size_t N>
constexpr StaticArray<T, N>::StaticArray(const T data_[N]) : data() {
  for (size_t i = 0; i < N; ++i) {
    data[i] = data_[i];
  }
}

template<typename T, size_t N>
void StaticArray<T, N>::read_P(size_t i, T *dst) const {
  if constexpr (N) {
    i = min(i, N - 1);
    memcpy_P(dst, &data[i], sizeof(T));
  }
}

template<typename T, size_t N, size_t K>
constexpr StaticArray2D<T, N, K>::StaticArray2D(
  const StaticArray<T, N>& arr,
  const StaticArray<size_t, K>& off
) {
  for (size_t i = 0; i < N; ++i) {
    data[i] = arr[i];
  }
  for (size_t i = 0; i < K; ++i) {
    offset[i] = off[i];
  }
}

template<typename T, size_t N, size_t K>
constexpr size_t StaticArray2D<T, N, K>::min_row_size() const {
  size_t res = -1;
  for (size_t i = 0; i < K; ++i) {
    res = min(res, offset[i] - (i ? offset[i - 1] : 0));
  }
  return res;
}

template<typename T, size_t N, size_t K>
constexpr size_t StaticArray2D<T, N, K>::max_row_size() const {
  size_t res = 0;
  for (size_t i = 0; i < K; ++i) {
    res = max(res, offset[i] - (i ? offset[i - 1] : 0));
  }
  return res;
}

template<typename T, size_t N, size_t K>
Pair<const T*, size_t> StaticArray2D<T, N, K>::read_P(size_t row) const {
  if constexpr (!K) {
    return make_pair(nullptr, 0);
  } else {
    row = min(row, K - 1);
    size_t start = row ? pgm_read_word(&offset[row - 1]) : 0;
    size_t end = pgm_read_word(&offset[row]);
    return make_pair(&data[start], end - start);
  }
}

template<typename T, size_t N, size_t K>
size_t StaticArray2D<T, N, K>::read_P(
  size_t row,
  size_t max_row_size,
  T *dst
) const {
  auto p = read_P(row);
  size_t size = min(p.second, max_row_size);
  memcpy_P(dst, p.first, size * sizeof(T));
  return size;
}

template<typename... Ts> constexpr auto make_static_array(Ts... xs) {
  using T = First<Ts...>;
  return StaticArray<T, count<Ts...>()>((T[]){xs...});
}

template<typename... Ts>
constexpr auto concat(Ts... arrs) {
  using T = ArrayType<First<Ts...> >;
  constexpr size_t N = sum<array_size(type<Ts>)...>();
  T res[N];
  size_t i = 0;
  ([&](auto x) {
    for (size_t j = 0; j < array_size(type<decltype(x)>); ++j) {
      res[i++] = x[j];
    }
  }(arrs), ...);
  return StaticArray<T, N>(res);
}

template<typename... Ts>
constexpr auto partial_size_sums(Ts... arrs) {
  constexpr size_t N = count<Ts...>();
  size_t res[N];
  size_t i = 0;
  ([&](auto x) {
    size_t acc = i ? res[i - 1] : 0;
    res[i++] = acc + array_size(type<decltype(x)>);
  }(arrs), ...);
  return StaticArray<size_t, N>(res);
}

template<typename... Ts> constexpr auto make_static_array_2d(Ts... xs) {
  auto data = concat(xs...);
  auto offset = partial_size_sums(xs...);
  using T = ArrayType<decltype(data)>;
  constexpr size_t N = array_size(type<decltype(data)>);
  constexpr size_t K = count<Ts...>();
  return StaticArray2D<T, N, K>(data, offset);
}

#endif // ARRAY_H_INCLUDED
