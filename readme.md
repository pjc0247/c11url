c11url
====
__11bcur1__<br>
a simple `libcurl` wrapper for modern c++

```cpp
try {
  // GET
  std::cout << curl::http_get("http://www.naver.com/").get();

  // POST
  std::cout << curl::http_post("http://www.naver.com/",
    post_data, post_data_len).get();
}
catch(const curl::http_exception &e){
  std::cout << e.status_code() << std::endl;
  std::cout << e.what();
}
```

with __await syntax
----
```cpp
std::future<void> test() {
	std::cout << __await curl::http_get("http://www.naver.com/");
}
void main() {
  test().get();
}
```
`__await` 키워드를 사용하기 위해서는 VisualStudio2015이상 버전과 `/await` 컴파일러 옵션이 필요합니다.
