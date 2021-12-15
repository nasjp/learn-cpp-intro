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

template <typename Func>
struct scope_exit {
  Func cleanup;

  scope_exit(Func&& f) : cleanup(std::forward<Func>(f)) {}
  ~scope_exit() { cleanup(); }
};

template <typename T, typename Allocator = std::allocator<T>>
class vector {
 public:
  using value_type = T;
  using pointer = T*;
  using pointer_const = const T*;
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
  pointer first = nullptr;
  // 最後の要素の1つ後方のポインター
  pointer last = nullptr;
  // 確保したストレージの終端
  pointer reserved_last = nullptr;
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
  void destroy(pointer ptr) { traits::destroy(alloc, ptr); }
  void destroy_until(reverse_iterator rend) {
    for (auto riter = rbegin(); riter != rend; ++riter, --last) {
      destroy(&*riter);
    }
  }
  void clear() noexcept { destroy_until(rend()); }

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

  vector(const allocator_type& alloc = allocator_type()) noexcept
      : alloc(alloc) {}
  vector(size_type size, const allocator_type& alloc = allocator_type())
      : vector(alloc) {
    resize(size);
  }
  vector(size_type size, const_reference value,
         const allocator_type& alloc = allocator_type())
      : vector(alloc) {
    resize(size, value);
  }

  ~vector() {
    // 1. 要素を末尾から先頭に向かう順番で破棄
    clear();
    // 2. 生のメモリーを解放する
    deallocate();
  }

  void reserve(size_type sz) {
    // すでに指定された要素数以上に予約されているなら何もしない
    if (sz <= capacity()) return;

    // 動的メモリー確保をする
    auto ptr = allocate(sz);

    // 古いストレージの情報を保存
    auto old_first = first;
    auto old_last = last;
    auto old_capacity = capacity();

    // 新しいストレージに差し替え
    first = ptr;
    last = first;
    reserved_last = first + sz;

    // 例外安全のため
    // 関数を抜けるときに古いストレージを破棄する
    scope_exit s([&] { traits::deallocate(alloc, old_first, old_capacity); });

    // 古いストレージから新しいストレージに要素をコピー構築
    // 実際にはムーブ構築
    for (auto old_iter = old_first; old_iter != old_last; ++old_iter, ++last) {
      // このコピーの理解にはムーブセマンティクスの理解が必要
      construct(last, std::move(*old_iter));
    }

    // 新しいストレージにコピーし終えたので
    // 古いストレージの値は破棄
    for (auto riter = reverse_iterator(old_last),
              rend = reverse_iterator(old_first);
         riter != rend; ++riter) {
      destroy(&*riter);
    }
    // scope_exitによって自動的にストレージが破棄される
  }

  void resize(size_type sz) {
    // 現在の要素数より少ない
    if (sz < size()) {
      auto diff = size() - sz;
      destroy_until(rbegin() + diff);
      last = first + sz;
    }
    // 現在の要素数より大きい
    else if (sz > size()) {
      reserve(sz);
      for (; last != reserved_last; ++last) {
        construct(last);
      }
    }
  }

  void resize(size_type sz, const_reference value) {
    // 現在の要素数より少ない
    if (sz < size()) {
      auto diff = size() - sz;
      destroy_until(rbegin() + diff);
      last = first + sz;
    }
    // 現在の要素数より大きい
    else if (sz > size()) {
      reserve(sz);
      for (; last != reserved_last; ++last) {
        construct(last, value);
      }
    }
  }

  void push_back(const_reference value) {
    // 予約メモリーが足りなければ拡張
    if (size() + 1 > capacity()) {
      // 現在のストレージサイズ
      auto c = size();
      // 0の場合は1に
      if (c == 0) {
        c = 1;
      } else {
        // それ以外の場合は2倍する
        c *= 2;
      }

      reserve(c);
    }
    construct(last, value);
    ++last;
  }

  void shrink_to_fit() {
    // 何もする必要がない
    if (size() == capacity()) return;

    // 新しいストレージを確保
    auto ptr = allocate(size());
    // コピー
    auto current_size = size();
    for (auto raw_ptr = ptr, iter = begin(), iter_end = end(); iter != iter_end;
         ++iter, ++raw_ptr) {
      construct(raw_ptr, *iter);
    }
    // 破棄
    clear();
    deallocate();
    // 新しいストレージを使う
    first = ptr;
    last = ptr + current_size;
    reserved_last = last;
  }

  // vector(const_iterator first, const_iterator last,
  vector(pointer_const first, pointer_const last,
         const allocator_type& alloc = allocator_type())
      : vector(alloc) {
    reserve(std::distance(first, last));
    for (auto i = first; i != last; ++i) {
      push_back(*i);
    }
  }

  vector(std::initializer_list<value_type> init,
         const allocator_type& alloc = allocator_type())
      : vector(std::begin(init), std::end(init), alloc) {}

  vector(const vector& r)
      // アロケーターのコピー
      : alloc(traits::select_on_container_copy_construction(r.alloc)) {
    // コピー元の要素数を保持できるだけのストレージを確保
    reserve(r.size());
    // コピー元の要素をコピー構築
    // destはコピー先
    // [src, last)はコピー元
    for (auto dest = first, src = r.begin(), last = r.end(); src != last;
         ++dest, ++src) {
      construct(dest, *src);
    }
    last = first + r.size();
  }

  vector& operator=(const vector& r) {
    // 1. 自分自身への代入なら何もしない
    if (this == &r) {
      return *this;
    }

    // 2. 要素数が同じならば
    if (size() == r.size()) {
      // 要素ごとにコピー代入
      std::copy(r.begin(), r.end(), begin());

      return *this;
    }

    // 3. それ以外の場合で
    // 予約数が十分ならば、
    if (capacity() >= r.size()) {
      // 有効な要素はコピー
      std::copy(r.begin(), r.begin() + r.size(), begin());
      // 残りはコピー構築
      for (auto src_iter = r.begin() + r.size(), src_end = r.end();
           src_iter != src_end; ++src_iter, ++last) {
        construct(last, *src_iter);
      }

      return *this;
    }

    // 4. 予約数が不十分ならば
    // 要素をすべて破棄
    clear();
    // 予約
    reserve(r.size());
    // コピー構築
    for (auto src_iter = r.begin(), src_end = r.end(), dest_iter = begin();
         src_iter != src_end; ++src_iter, ++dest_iter, ++last) {
      construct(dest_iter, *src_iter);
    }

    return *this;
  }
};

struct X {
  ~X() { std::cout << "destructed. => "s << this << std::endl; }
};

template <typename T>
class own {
 private:
  T* ptr;

 public:
  own() : ptr(new T) {}
  ~own() { delete ptr; }
  own(const own&) = default;
  own& operator=(const own& r) {
    // 自分自身への代入でなければ
    if (this != &r) {
      // コピー処理
      *ptr = *r.ptr;
    }

    return *this;
  }

  T* get() const { return ptr; }
};

int&& gg() { return 0; }

template <typename T>
void ggg(T&& t) {}

template <typename T>
void can_lvalu_rvalue_arg(T&& t) {
  // 無条件にxvalueが渡される
  ggg(std::move(t));
  // tがlvalueならばlvalueとして、rvalueならばxvalueとして渡される
  ggg(std::forward<T>(t));
}

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
    } catch (const std::bad_alloc&) {
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
    } catch (const std::bad_alloc&) {
      std::cout << "failed" << std::endl;
    }
  }

  {
    try {
      auto logger_ptr = new Logger{"Alice"s};
      delete logger_ptr;
      std::cout << "succeeded" << std::endl;
    } catch (const std::bad_alloc&) {
      std::cout << "failed" << std::endl;
    }
  }

  {
    try {
      int* int_array_ptr = new int[5]{1, 2, 3, 4, 5};
      delete[] int_array_ptr;
      std::cout << "succeeded" << std::endl;
    } catch (const std::bad_alloc&) {
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

  {
    std::vector<X> v(5);
    v.resize(2);
    std::cout << "resized.\n"s;
  }

  auto print_all = [](auto first, auto last) {
    // ループ
    for (auto iter = first; iter != last; ++iter) {
      // 重要な処理
      std::cout << "v[]: " << *iter << std::endl;
    }
  };

  // my vector
  {
    vector<int> v1;

    std::cout << "first" << std::endl;
    print_all(v1.cbegin(), v1.cend());

    std::cout << "second" << std::endl;
    v1.resize(1);
    print_all(v1.cbegin(), v1.cend());

    std::cout << "third" << std::endl;
    v1.resize(5);
    v1[3] = 300;
    v1[4] = 800;
    print_all(v1.cbegin(), v1.cend());

    std::cout << "forth" << std::endl;
    v1.resize(8, 100);
    print_all(v1.cbegin(), v1.cend());
  }

  {
    vector<int> v(10, 1);
    v[2] = 99;
    v.resize(5);

    print_all(v.cbegin(), v.cend());
  }

  {
    // 要素数10000
    vector<int> v(1000);
    // 10001個分のメモリーを確保する
    // 10000個の既存の要素をコピーする
    v.push_back(0);
    // 10002個分のメモリーを確保する
    // 10001個の既存の要素をコピーする
    v.push_back(0);
  }

  {
    vector<int> v;
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    std::cout << (v.size() == v.capacity()) << std::endl;
    v.shrink_to_fit();
    std::cout << (v.size() == v.capacity()) << std::endl;
  }

  {
    std::array<int, 5> a{1, 2, 3, 4, 5};
    vector<int> v(std::begin(a), std::end(a));

    print_all(v.cbegin(), v.cend());
  }

  {
    std::array<int, 5> a1{1, 2, 3, 4, 5};
    vector<int> v1(std::begin(a1), std::end(a1));
    vector<int> v2(std::cbegin(a1), std::cend(a1));
    const std::array<int, 5> a2{1, 2, 3, 4, 5};
    vector<int> v3(std::begin(a2), std::end(a2));
    vector<int> v4(std::cbegin(a2), std::cend(a2));
  }

  {
    vector<int> v = {1, 2, 3};
    print_all(v.cbegin(), v.cend());
  }

  {
    own<int> a;
    a = a;
  }

  {
    int object = 1;
    // xvalue
    int&& r = static_cast<int&&>(object);

    std::cout << "r: " << r << std::endl;
  }

  {
    int lvalue{};
    int&& r1 = std::move(lvalue);
    int&& r2 = static_cast<int&&>(lvalue);

    std::boolalpha(std::cout);
    std::cout << "r1 == r2: " << (r1 == r2) << std::endl;
  }

  // value category
  {
    int object = 12;
    auto f = [&]() -> int& { return object; };

    // int&& gg = []() -> int&& { return 0; };
    auto h = [] { return 0; };

    // lvalue reference
    int& lvalue_reference1 = object;
    int& lvalue_reference2 = f();

    // rvalue reference
    int&& rvalue_reference1 = 0;
    int&& rvalue_reference2 = 1 + 1;
    int&& rvalue_reference3 = gg();
    int&& rvalue_reference4 = h();

    // const lvalue reference
    const int& const_lvalue_reference1 = 0;
    const int& const_lvalue_reference2 = 1 + 1;
    const int& const_lvalue_reference3 = gg();
    const int& const_lvalue_reference4 = h();

    // lvalue reference to rvalue reference
    int& lvalue_reference_to_rvalue_reference = rvalue_reference1;

    // pvalue
    0;
    1 + 1;
    f();

    // xvalue
    int&& xvalue1 = gg();
    int&& xvalue2 = static_cast<int&&>(object);
    int&& xvalue3 = std::move(object);
    int a[3] = {1, 2, 3};
    int&& xvalue4 = static_cast<int(&&)[3]>(a)[0];
    struct X {
      int data_member;
    };
    X x{};
    int&& xvalue5 = static_cast<X&&>(x).data_member;
  }

  {
    using A = std::remove_reference_t<int>;
    // int
    using B = std::remove_reference_t<int&>;
    // int
    using C = std::remove_reference_t<int&&>;

    std::cout << "A == B: " << (std::is_same_v<A, B>) << std::endl;
    std::cout << "A == C: " << (std::is_same_v<A, C>) << std::endl;
    std::cout << "B == C: " << (std::is_same_v<A, C>) << std::endl;
  }

  {
    int lvalue{};
    can_lvalu_rvalue_arg(lvalue);  // can use lvalue
    can_lvalu_rvalue_arg(0);       // can use rvalue
  }

  {
    auto p = std::make_unique<std::vector<int>>();
    p->push_back(0);
  }

  // { std::cout << "こんにちは"s << std::endl; }
}
