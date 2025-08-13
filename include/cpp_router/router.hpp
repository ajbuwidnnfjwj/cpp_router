#ifndef CPU_ROUTER_ROUTER_H
#define CPU_ROUTER_ROUTER_H

#include <string>
#include <exception>
#include <ranges>

#include "callable.hpp"
#include "node.hpp"


namespace Router {
	using namespace pathTree;

	class Router {
		Node root;

		const char delimiter = '/';
	public:
		Router() { }
		Router(Node&& new_root) : root(std::move(new_root)) { }
		Router(std::string path) {
			this->insert(path);
		}
		template <SupportedMethod METHOD, class CallbackType>
			requires CallbackMatches<std::decay_t<CallbackType>>
		Router(std::string path, CallbackType callback) {
			this->insert(path, callback);
		}

		void insert(std::string path) {
			auto split_view = std::views::split(path, delimiter);
			std::vector<std::string> tokens;
			for (const auto& word_range : split_view) {
				tokens.emplace_back(word_range.begin(), word_range.end());
			}

			Node* curr_node = &root;
			for (std::string& token : tokens) {
				auto [next_node, success] = curr_node->insert(token);
				curr_node = next_node->second.get();
			}
		}

		template <SupportedMethod METHOD, class CallbackType>
			requires CallbackMatches<std::decay_t<CallbackType>>
		void insert(std::string path , CallbackType&& callback) {
			auto split_view = std::views::split(path, delimiter);
			std::vector<std::string> tokens;
			for (const auto& word_range : split_view) {
				tokens.emplace_back(word_range.begin(), word_range.end());
			}

			Node* curr_node = &root;
			for (std::string& token : tokens) {
				auto [next_node, success] = curr_node->insert(token);
				if (!success) {
					throw std::exception("fail to add node");
					return;
				}
				curr_node = next_node->second.get();
			}
			curr_node->push_handler<METHOD>(callback);
		}

		Node* find(std::string path) {
			auto split_view = std::views::split(path, delimiter);
			std::vector<std::string> tokens;
			for (const auto& word_range : split_view) {
				tokens.emplace_back(word_range.begin(), word_range.end());
			}

			Node* curr_node = &root;
			for (const std::string& token : tokens) {
				auto [child_map_iter, success] = curr_node->find(token);
				
				if (!success) return nullptr;
				curr_node = child_map_iter->second.get();
			}
			return curr_node;
		}

		template<SupportedMethod METHOD>
		void execute(std::string path) {
			Node* node = find(path);
			if (!node) return;

			node->execute_handler<METHOD>();
		}
	};
}

#endif // CPP_ROUTER_ROUTER_H