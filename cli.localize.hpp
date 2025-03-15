#pragma once

// Internationalization (i18n) for the lightweight CLI framework.

#include "chh.hpp"

namespace cli {

    // Manages different languages.
    class LanguageManager {
    public:
        class Language {
        public:
            Language() = default;
            Language(const std::string& content) {
                Json::Value root = chh::parseJson(content);
                if (!root.isObject()) {
                    throw std::runtime_error("`root` is not an object.");
                }
                for (const auto& i : root.getMemberNames()) {
                    this->_content[i] = root[i].asString();
                }
            }
            std::string localize(const std::string& key) const {
                if (this->_content.count(key)) {
                    return this->_content.at(key);
                }
                else {
                    return key;
                }
            }
        private:
            std::map<std::string, std::string> _content;
        };
        LanguageManager() = default;
        void load(const std::string& name, const std::string& content) {
            this->_languages[name] = Language(content);
        }
        void loadFile(const std::string& name, const std::string& file_name) {
            this->_languages[name] = Language(chh::toString(chh::readFile(file_name)));
        }
#ifdef CHH_WINDOWS
        void loadResource(std::string name, size_t res_name, std::string res_type) {
            this->_languages[name] = Language(chh::toString(chh::readResource(res_name, res_type)));
        }
#endif
#ifdef CHH_LINUX
        void loadResource(std::string name, size_t res_name, std::string res_type) = delete;
#endif
        const Language& switchLanguage(std::string language) {
            if (this->_languages.count(language)) {
                this->_current = language;
                return this->_languages.at(this->_current);
            }
            else {
                throw std::runtime_error("No language called " + language);
            }
        }
        std::string localize(std::string key) const {
            return this->_languages.at(this->_current).localize(key);
        }
        // Returns the currently selected language.
        std::string current() const {
            return this->_current;
        }
    private:
        std::map<std::string, Language> _languages;
        std::string _current;
    };

    // Localizes one string by key.
    class LocalizingString {
    public:
        LocalizingString(std::string key) : _key(key) {}
        std::string localize(const LanguageManager& languages) const {
            return languages.localize(this->_key);
        }
    private:
        std::string _key;
    }; 

    // Concatenates LocalizingStrings and strings.
    class Text {
    public:
        Text() = default;
        Text(const Text& text) = default;
        Text(const LocalizingString& string) : _parts({ string }) {}
        Text(const std::string& string) : _parts({ string }) {}
        Text(const char* string) : _parts({ string }) {}
        Text operator+(const Text& that) const {
            Text result = *this;
            result._parts.insert(result._parts.end(), that._parts.begin(), that._parts.end());
            return result;
        }
        Text& operator+=(const Text& that) {
            this->_parts.insert(this->_parts.end(), that._parts.begin(), that._parts.end());
            return *this;
        }
        // Localizes the text based on the current language settings.
        std::string localize(const LanguageManager& languages) const {
            if (this->_cache_language != languages.current()) {
                this->_cache_language = languages.current();
                std::ostringstream oss;
                for (auto& part : this->_parts) {
                    std::visit([&oss, &languages](auto& arg) {
                        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, LocalizingString>) {
                            oss << arg.localize(languages);
                        }
                        else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                            oss << arg;
                        }
                        }, part);
                }
                this->_cache = oss.str();
            }
            return this->_cache;
        }
        // Returns the number of parts in the text.
        size_t size() const {
            return this->_parts.size();
        }
    private:
        std::vector<std::variant<LocalizingString, std::string>> _parts;
        mutable std::string _cache;
        mutable std::string _cache_language;
    };

}