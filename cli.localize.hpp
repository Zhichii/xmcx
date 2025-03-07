#pragma once

#include "cli.basic.hpp"

namespace CLI {

    struct LanguageManager {
        struct Language {
            std::map<std::string, std::string> content;
            Language() {}
            Language(std::string _content) {
                Json::Value root; CHH::parseJson(_content, root);
                if (!root.isObject()) CHH_THROW_ERROR("`root` is not an object.\n");
                for (auto& i : root.getMemberNames()) {
                    this->content[i] = root[i].asString();
                }
            }
        };
        std::map<std::string, Language> languages;
        std::string current;
        void loadLanguage(std::string name, std::string content) {
            this->languages[name] = Language(content);
        }
        void loadLanguageFromFile(std::string name, std::string file_name) {
            this->languages[name] = Language(CHH::to_string(CHH::readFile(file_name)));
        }
        void loadLanguageFromResource(std::string name, size_t res_name, std::string type) {
            this->languages[name] = Language(CHH::to_string(CHH::readFromResource(res_name, type)));
        }
    };

    struct LocalizingText {
        std::string key;
        std::format_args args = std::make_format_args();
        LocalizingText() {}
        template <typename... Ts>
        LocalizingText(std::string key, Ts... args) { this->key = key;  this->args = std::make_format_args(args...); }
        std::string localize(LanguageManager& languages) {
            if (languages.languages.contains(languages.current) &&
                languages.languages[languages.current].content.contains(this->key)) {
                return std::vformat(languages.languages[languages.current].content[this->key], this->args);
            }
            else return this->key;
        }
    };

}