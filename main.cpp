#include "all.h"

int f(int x) {
  std::cout << x << std::endl;
  return x;
}

int g(int x) { return x; }

using f_ptr = int (*)(int);
f_ptr h(f_ptr p) {
  p(0);
  return p;
}

struct Object {
  int data_member;
  void member_function() { std::cout << data_member; }
};

struct C {
  int f(int) { return 0; }
  int f2(int, int) { return 2; }
};

template <typename T>
void print_raw_address(T ptr) {
  std::cout << reinterpret_cast<void*>(ptr) << std::endl;
}

void* my_memcpy(void* dest, const void* src, std::size_t n) {
  // destをstd::byte *型に変換
  auto dest_byte = static_cast<std::byte*>(dest);
  // srcをstd::byte const *型に変換する
  auto src_byte = static_cast<const std::byte*>(src);

  // srcからnバイトコピーするのでnバイト先のアドレスを得る
  auto last = src_byte + n;

  // nバイトコピーする
  // while (src_byte != last) {
  //   *dest_byte = *src_byte;
  //   ++dest_byte;
  //   ++src_byte;
  // }
  for (std::size_t i = 0; i != n; ++i) {
    dest_byte[i] = src_byte[i];
  }

  // destを返す
  return dest;
}

struct Object2 {
  int x;
  int y;
  int z;
};

struct Logger {
  std::string name;
  Logger(std::string name) : name(name) {
    std::cout << name << " is constructed.\n"s;
  }
  ~Logger() { std::cout << name << " is destructed.\n"s; }
};

template <typename T, typename Allocator = std::allocator<T> >
class vector {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const pointer;
  using reference = value_type&;
  using const_reference = const value_type&;

  using allocator_type = Allocator;

  using size_type = std::size_t;

  using difference_type = std::ptrdiff_t;

  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 private:
  // 先頭の要素へのポインター
  pointer first;
  // 最後の要素の1つ後方のポインター
  pointer last;
  // 確保したストレージの終端
  pointer reserved_last;
  // アロケーターの値
  allocator_type alloc;

  using traits = std::allocator_traits<allocator_type>;
  pointer allocate(size_type n) { return traits::allocate(alloc, n); }
  void deallocate() { traits::deallocate(alloc, first, capacity()); }
  void construct(pointer ptr) { traits::construct(alloc, ptr); }
  void construct(pointer ptr, const_reference value) {
    traits::construct(alloc, ptr, value);
  }
  // ムーブ用
  void construct(pointer ptr, value_type&& value) {
    traits::construct(alloc, ptr, std::move(value));
  }

 public:
  iterator begin() noexcept { return first; }
  iterator end() noexcept { return last; }
  iterator begin() const noexcept { return first; }
  iterator end() const noexcept { return last; }
  const_iterator cbegin() const noexcept { return first; }
  const_iterator cend() const noexcept { return last; }

  reverse_iterator rbegin() noexcept { return reverse_iterator{last}; }
  reverse_iterator rend() noexcept { return reverse_iterator{first}; }
  reverse_iterator rbegin() const noexcept { return reverse_iterator{last}; }
  reverse_iterator rend() const noexcept { return reverse_iterator{first}; }
  const_reverse_iterator crbegin() const noexcept {
    return reverse_iterator{last};
  }
  const_reverse_iterator crend() const noexcept {
    return reverse_iterator{first};
  }

  size_type size() const noexcept { return end() - begin(); }
  bool empty() const noexcept { return size() == 0; }

  size_type capacity() const noexcept { return reserved_last - first; }

  reference operator[](size_type i) { return first[i]; }
  const_reference operator[](size_type i) const { return first[i]; }

  reference at(size_type i) {
    if (i >= size()) throw std::out_of_range("index is out of range.");

    return first[i];
  }
  const_reference at(size_type i) const {
    if (i >= size()) throw std::out_of_range("index is out of range.");

    return first[i];
  }

  reference front() { return first; }
  const_reference front() const { return first; }
  reference back() { return last - 1; }
  const_reference back() const { return last - 1; }

  pointer data() noexcept { return first; }
  const_pointer data() const noexcept { return first; }

  vector(const allocator_type& alloc = allocator_type()) : vector(alloc) {}
  vector(size_type size, const allocator_type& alloc = allocator_type())
      : vector(alloc) {
    resize(size);
  }
  vector(size_type size, const_reference value,
         const allocator_type& alloc = allocator_type())
      : vector(alloc) {
    resize(size, value);
  }
};

int main() {
  {
    auto hoge = 1;
    auto& hoge_ref = hoge;
    const int& hoge_ref2 = hoge;
    auto hoge_ptr = &hoge;
    int* hoge_ptr2 = &hoge;

    // read
    auto tmp1 = hoge_ref;
    auto tmp2 = hoge_ptr;

    // write
    hoge_ref = 3;
    *hoge_ptr = 8;

    std::cout << "hoge: " << hoge << std::endl;
    std::cout << "hoge_ref: " << hoge_ref << std::endl;
    std::cout << "hoge_ptr: " << hoge_ptr << std::endl;
    std::cout << "hoge_ptr_deref: " << *hoge_ptr << std::endl;

    std::string* p2 = nullptr;
  }

  // const int pointer const
  {
    const int data = 123;

    const int* const_int_ptr1 = &data;
    auto const_int_ptr2 = &data;
    const int* const const_int_ptr_const1 = &data;
    const auto const_int_ptr_const2 = &data;

    std::boolalpha(std::cout);
    std::cout << (const_int_ptr1 == const_int_ptr2) << std::endl;
    std::cout << (const_int_ptr_const1 == const_int_ptr_const2) << std::endl;
  }

  // pointer pointer
  {
    int x = 123;
    int* p = &x;
    int** pp = &p;

    int y = 999;
    // ポインターを経由した変数pの変更
    *pp = &y;

    std::cout << "**pp: " << **pp << std::endl;
  }

  // pointer reference
  {
    int x = 123;
    int* p = &x;
    int** pp = &p;

    auto& r1 = *p;

    r1 = 456;

    std::cout << "x: " << x << std::endl;

    auto& r2 = **pp;

    r2 = 789;

    std::cout << "x: " << x << std::endl;
  }

  // const pointer * 3
  { using type = int const* const* const* const; }

  // function pointer
  {
    using f_type = int(int);
    using f_pointer = f_type*;
    auto ptr = &f;
    (*ptr)(123);
    ptr(123);

    int (*ptr2)(int) = &f;
    ptr2(123);
  }

  {
    h(&g);
    auto ptr = &h;
  }

  {
    int a[5] = {1, 2, 3, 4, 5};

    auto ptr = a;

    std::cout << "*ptr: " << *ptr << std::endl;  // &a[0] (= 1)
  }

  {
    // Object::data_memberメンバーへのポインター
    int Object::*int_ptr = &Object::data_member;
    // Object::member_functionメンバーへのポインター
    void (Object::*func_ptr)() = &Object::member_function;

    // クラスのオブジェクト
    Object object;

    // objectに対するメンバーポインターを介した参照
    object.*int_ptr = 123;
    // objectに対するメンバーポインターを介した参照
    // 123
    (object.*func_ptr)();

    // 別のオブジェクト
    Object another_object;
    another_object.data_member = 456;
    // 456
    (another_object.*func_ptr)();
  }

  {
    // メンバー関数へのポインター
    int (C::*ptr)(int) = &C::f;
    auto ptr2 = &C::f;
    // クラスのオブジェクト
    C object;

    (object.*ptr)(123);
  }

  {
    C object;
    (object.*&C::f2)(1, 2);
    std::invoke(&C::f2, object, 1, 2);
  }

  {
    int data;
    int const* int_const_ptr = &data;
    void const* void_const_ptr = int_const_ptr;
    int const* original = static_cast<int const*>(void_const_ptr);
  }

  {
    int data;
    auto const int_const_ptr = &data;
    void const* void_const_ptr = int_const_ptr;
    auto original = static_cast<const int*>(void_const_ptr);
  }

  {
    int data[3] = {0, 1, 2};
    print_raw_address(&data[0]);
    print_raw_address(&data[1]);
    print_raw_address(&data[2]);
  }

  {
    auto ptr = std::malloc(5);
    std::free(ptr);
  }

  {
    auto ptr = ::operator new(5);
    ::operator delete(ptr);
  }

  {
    auto void_ptr = ::operator new(sizeof(int));
    auto int_ptr = static_cast<int*>(void_ptr);
    *int_ptr = 0;
    ::operator delete(void_ptr);
  }

  {
    auto ptr = std::malloc(1);
    if (ptr == nullptr) {
      std::cout << "malloc failed" << std::endl;
    } else {
      std::cout << "malloc succeeded" << std::endl;
      free(ptr);
    }
  }

  {
    try {
      auto ptr = ::operator new(1);
      ::operator delete(ptr);
      std::cout << "malloc succeeded" << std::endl;
    } catch (std::bad_alloc e) {
      std::cout << "malloc failed" << std::endl;
    }
  }

  {
    try {
      auto ptr = ::operator new(sizeof(Logger));
      auto logger_ptr = new (ptr) Logger{"Alice"s};
      logger_ptr->~Logger();
      ::operator delete(logger_ptr);
      std::cout << "succeeded" << std::endl;
    } catch (std::bad_alloc e) {
      std::cout << "failed" << std::endl;
    }
  }

  {
    try {
      auto logger_ptr = new Logger{"Alice"s};
      delete logger_ptr;
      std::cout << "succeeded" << std::endl;
    } catch (std::bad_alloc e) {
      std::cout << "failed" << std::endl;
    }
  }

  {
    try {
      int* int_array_ptr = new int[5]{1, 2, 3, 4, 5};
      delete[] int_array_ptr;
      std::cout << "succeeded" << std::endl;
    } catch (std::bad_alloc e) {
      std::cout << "failed" << std::endl;
    }
  }

  {
    std::allocator<std::string> a;
    auto p = a.allocate(1);
    auto s = new (p) std::string("hello");
    s->~basic_string();
    a.deallocate(p, 1);
  }

  {
    std::allocator<int> a;
    // auto p = std::allocator_traits<std::allocator<int> >::allocate(a, 1);
    // using traits = std::allocator_traits<std::allocator<int> >;
    using traits = std::allocator_traits<decltype(a)>;
    auto p = traits::allocate(a, 1);
  }

  {
    std::allocator<std::string> a;
    // allocator_traits型
    using traits = std::allocator_traits<decltype(a)>;

    // 生のメモリー確保
    auto p = traits::allocate(a, 1);
    // 構築
    traits::construct(a, p, "hello");
    // 破棄
    traits::destroy(a, p);
    // メモリー解放
    traits::deallocate(a, p, 1);
  }

  {
    auto N = 10;
    std::allocator<std::string> a;
    using traits = std::allocator_traits<decltype(a)>;
    auto p = traits::allocate(a, N);

    for (auto i = p, last = p + N; i != last; ++i) {
      traits::construct(a, i, "hello");
    }

    for (auto i = p + N, first = p; i != first; --i) {
      traits::destroy(a, i);
    }

    traits::deallocate(a, p, N);
  }
}
