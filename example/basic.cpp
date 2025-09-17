#include "../include/cpp_router/cpp_router.h"
#include <iostream>

int main() {
	Router::Router r;
	r.insert<HTTP_METHODS::GET>("/example", []() {
		std::cout << "example get" << std::endl;
		});
	r.insert<HTTP_METHODS::GET>("/example/detail", []() {
		std::cout << "example/detail get" << std::endl;
		});

	r.execute<HTTP_METHODS::GET>("/example?get=get&post=post");
}