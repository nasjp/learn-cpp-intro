[![CI](https://github.com/nasjp/learn-cpp-intro/actions/workflows/ci.yml/badge.svg)](https://github.com/nasjp/learn-cpp-intro/actions/workflows/ci.yml)

# 江添亮のC++入門

Refs: <https://ezoeryou.github.io/cpp-intro/>

続き: <https://ezoeryou.github.io/cpp-intro/#文字列-1>

## boost

```sh
wget https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.tar.bz2
tar -jxvf boost_1_77_0.tar.bz2
```

## bazel

<https://blog.envoyproxy.io/external-c-dependency-management-in-bazel-dd37477422f5>

## メモ

### 5原則

> - コピーコンストラクター
> - コピー代入演算子
> - ムーブコンストラクター
> - ムーブ代入演算子
> - デストラクター
> 
> このうちの1つを独自に定義したならば、残りの4つも定義すべきである。
