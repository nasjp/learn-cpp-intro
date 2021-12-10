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
}
