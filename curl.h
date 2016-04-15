#pragma once

#include <future>
#include <string>
#include <iostream>

#include <curl/curl.h>
#include <curl/easy.h>

#pragma comment (lib, "libcurldll.a")

namespace curl {
	class http_exception : public std::runtime_error {
	public:
		http_exception(
			int code, const std::string &msg) :
			code(code),
			std::runtime_error(msg) {
		}

		int status_code() const {
			return code;
		}

	protected:
		const int code;
	};

	// 처리 중간에 익셉션 던져도 정리되게 하기 위해서
	// RAII로 관리
	class http_context {
	public:
		http_context(const std::string &uri) {
			curl = curl_easy_init();

			curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		}
		virtual ~http_context() {
			if (curl != nullptr)
				curl_easy_cleanup(curl);
			curl = nullptr;
		}

		void set_postdata(void *ptr, long len) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ptr);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len);
		}
		CURLcode perform() {
			auto res = curl_easy_perform(curl);

			curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &code);

			return res;
		}

	private:
		static size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *str) {
			str->append((char*)ptr);

			return size * nmemb;
		}

	public:
		CURL *curl;
		std::string body;
		long code;
	};
	
	std::future<std::string> http_get(const std::string &uri) {
		return std::async([uri]() {
			http_context ctx(uri);

			ctx.perform();
			if (ctx.perform() != CURLE_OK)
				throw http_exception(ctx.code, "");
			if (ctx.code != 200)
				throw http_exception(ctx.code, ctx.body);

			return std::move(ctx.body);
		});
	}
	std::future<std::string> http_post(const std::string &uri, void *data, long len) {
		return std::async([uri, data, len]() {  
			http_context ctx(uri);

			ctx.set_postdata(data, len);
			if (ctx.perform() != CURLE_OK)
				throw http_exception(ctx.code, "");
			if (ctx.code != 200)
				throw http_exception(ctx.code, ctx.body);

			return std::move(ctx.body);
		});
	}
	std::future<std::string> http_post(const std::string &uri, const std::string &data) {
		return http_post(uri, (void*)data.c_str(), data.length());
	}
};