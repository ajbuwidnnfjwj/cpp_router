#ifndef CPP_ROUTER_METHOD_H
#define CPP_ROUTER_METHOD_H

namespace wrapper {
	template <int N>
	struct Int {
		static const int num = N;
	};
}

struct HTTP_METHODS {
	using GET = wrapper::Int<1>;
	using POST = wrapper::Int<2>;
	using UPDATE = wrapper::Int<3>;
	using DEL = wrapper::Int<4>;
};

#endif // CPP_ROUTER_METHOD_H