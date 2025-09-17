#ifndef CPU_ROUTER_ROUTER_H
#define CPU_ROUTER_ROUTER_H

#include <string>
#include <stdexcept> 
#include <regex>
#include <ranges>

#include "callable.hpp"
#include "node.hpp"

#include <iostream>

namespace Router {
	using namespace pathTree;

	class Router {
		Node root;

		std::vector<std::string> tokenize_path(const std::string& path) {
            const auto cut = path.find_first_of("?#");
            const std::string path_only = (cut == std::string::npos) ? path : path.substr(0, cut);

            static const std::regex seg_re(R"([^/]+)");
            std::vector<std::string> tokens;
            for (std::sregex_iterator it(path_only.begin(), path_only.end(), seg_re), end; it != end; ++it) {
                tokens.emplace_back(it->str());
            }
            return tokens;
        }
	public:
        Router() { }
        Router(Node&& new_root) : root(std::move(new_root)) { }

        explicit Router(std::string path) {
            this->insert(std::move(path));
        }

        template <SupportedMethod METHOD, class CallbackType>
            requires CallbackMatches<std::decay_t<CallbackType>>
        Router(std::string path, CallbackType callback) {
            this->insert<METHOD>(std::move(path), std::forward<CallbackType>(callback));
        }

        void insert(std::string path) {
            auto tokens = tokenize_path(path);

            Node* curr_node = &root;
            for (std::string& token : tokens) {
                auto [next_node, /*success*/_] = curr_node->insert(token);
                curr_node = next_node->second.get();
            }
        }

        template <SupportedMethod METHOD, class CallbackType>
            requires CallbackMatches<std::decay_t<CallbackType>>
        void insert(std::string path, CallbackType&& callback) {
            auto tokens = tokenize_path(path);

            Node* curr_node = &root;
            for (std::string& token : tokens) {
                auto [next_node, success] = curr_node->insert(token);
                if (!success) {
                    throw std::runtime_error("fail to add node");
                }
                curr_node = next_node->second.get();
            }
            curr_node->push_handler<METHOD>(std::forward<CallbackType>(callback));
        }

        Node* find(std::string path) {
            auto tokens = tokenize_path(path);

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
            if (Node* node = find(std::move(path))) {
                node->execute_handler<METHOD>();
            }
        }

        void parse_url_params(const std::string& path) {
            const auto cut = path.find_first_of("?#");
            const std::string args = (cut == std::string::npos) ? "" : path.substr(cut + 1);

            static const std::regex regex_raw(R"([^&]+)");
            static const std::regex regex_kv(R"([^=]+)");
            for (std::sregex_iterator raw(args.begin(), args.end(), regex_raw), pair_end; raw != pair_end; ++raw) {
                std::string key_val_raw = raw->str();
                auto pos = key_val_raw.find('=');
                if (pos != std::string::npos) {
                    std::string key = key_val_raw.substr(0, pos);
                    std::string value = key_val_raw.substr(pos + 1);

                    std::cout << "key: " << key << " " << "value: " << value << std::endl;
                }
                else {

                }
            }
        }
    };
}


#endif // CPP_ROUTER_ROUTER_H