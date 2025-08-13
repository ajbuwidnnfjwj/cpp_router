#ifndef CPP_ROUTER_CALLABLE_H
#define CPP_ROUTER_CALLABLE_H

#include <type_traits>
#include <concepts>
#include <string>

#include "HTTP.hpp"

template<class T>
concept VoidCallable = std::invocable<T>;
template<class T>
concept StringCallable = std::invocable<T, const std::string&>;
template<class T>
concept CallbackMatches = VoidCallable<T> || StringCallable<T>;

template<class M>
concept SupportedMethod = std::same_as<M, HTTP_METHODS::GET>
    || std::same_as<M, HTTP_METHODS::POST>
    || std::same_as<M, HTTP_METHODS::UPDATE>
    || std::same_as<M, HTTP_METHODS::DEL>;
#endif // CPP_ROUTER_CALLABLE_H