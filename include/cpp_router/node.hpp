#ifndef CPP_ROUTER_NODE_H
#define CPP_ROUTER_NODE_H

#include <string>

#include <map>
#include <functional>
#include <variant>
#include <memory>

#include "callable.hpp"

namespace pathTree {
	template<class M>
	struct handler {
		using method = M;
		using VOID_HANDLER_TYPE = std::function<void()>;
		using STRING_HANDLER_TYPE = std::function<void(std::string)>;

		std::variant<std::monostate, VOID_HANDLER_TYPE, STRING_HANDLER_TYPE> active_handler;

		template<typename T> requires CallbackMatches<T>
		void setHandler(T&& callback) {
			if constexpr (std::is_invocable_v<T>) {
				active_handler = VOID_HANDLER_TYPE(callback);
			}
			else if constexpr (std::is_invocable_v<T, std::string>) {
				active_handler = STRING_HANDLER_TYPE(callback);
			}
			else {
				static_assert(!sizeof(T), "지원하지 않는 핸들러 타입입니다.");
			}
		}

		void execute() const {
			if (auto* h = std::get_if<VOID_HANDLER_TYPE>(&active_handler)) {
				(*h)(); // 맞다면 함수 호출
			}
		}

		void execute(const std::string& s) const {
			if (auto* h = std::get_if<STRING_HANDLER_TYPE>(&active_handler)) {
				(*h)(s); // 맞다면 인자를 전달하며 함수 호출
			}
		}
	};
	


	struct Node {
		using GET_HANDLER = handler<HTTP_METHODS::GET>;
		using POST_HANDLER = handler<HTTP_METHODS::POST>;
		using UPDATE_HANDLER = handler<HTTP_METHODS::UPDATE>;
		using DELETE_HANDLER = handler<HTTP_METHODS::DEL>;
		using HANDLERS = std::variant<GET_HANDLER, POST_HANDLER, UPDATE_HANDLER, DELETE_HANDLER>;

		using CHILD_MAP_TYPES = std::map<std::string, std::unique_ptr<Node>>;
		using CHILD_MAP_ITER_TYPES = CHILD_MAP_TYPES::iterator;

		std::string name;
		CHILD_MAP_TYPES childs;

		std::vector<HANDLERS> handlers;

	public:
		Node() {}
		Node(std::string name) :name(name) {}

		Node(Node& other) = delete;
		Node(Node&& other) noexcept {
			name = std::move(other.name);
			childs = std::move(other.childs);
			handlers = std::move(other.handlers);
		}

		std::pair<CHILD_MAP_ITER_TYPES, bool> insert(std::string sub_path) {
			CHILD_MAP_ITER_TYPES iter = childs.find(sub_path);

			if (iter != childs.end()) {
				return { iter, true };
			}

			auto [iter_new, success] = childs.insert({ sub_path, std::make_unique<Node>(sub_path) });
			return { iter_new, success };
		}

		void pop(std::string sub_path) {
			childs.erase(sub_path);
		}

		std::pair<CHILD_MAP_ITER_TYPES, bool> find(const std::string& sub_path) noexcept {
			auto iter = childs.find(sub_path);
			return {
				iter, iter != childs.end()
			};
		}

		template <SupportedMethod METHOD, typename CallbackType>
			requires CallbackMatches<std::decay_t<CallbackType>>
		void push_handler(CallbackType&& callback) {
			handler<METHOD> temp;
			temp.setHandler(callback);

			auto it = std::find_if(handlers.begin(), handlers.end(), [](const HANDLERS& h) {
				return std::holds_alternative<handler<METHOD>>(h);
				});

			if (it != handlers.end()) {

			}

			handlers.push_back(std::move(temp));
		}

		template <SupportedMethod METHOD>
		void execute_handler() {
			auto it = std::find_if(handlers.begin(), handlers.end(),
				[](const HANDLERS& h) { return std::holds_alternative<handler<METHOD>>(h); });

			if (it != handlers.end()) {
				std::visit([&](const auto& h_in_variant){
					h_in_variant.execute();
				}, *it);
			}
		}
	};
}

#endif // CPP_ROUTER_NODE_H