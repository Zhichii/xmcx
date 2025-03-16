#pragma once

#include "chh.hpp"

/**
 * @brief 国际化。
 */
namespace hti::i18n {

    /**
     * @brief 管理所有的语言。
     */
    class LanguageManager {
    public:
        /**
         * @brief 单个语言。
         */
        class Language {
        public:
            /**
             * @brief 默认构造器。
             */
            Language() = default;
            /**
             * @brief 从 JSON 字符串中加载语言。
             * @param content JSON 字符串
             * @throw std::runtime_error 如果 JSON 解析失败。
             */
            Language(const std::string& content) {
                Json::Value root = chh::parseJson(content);
                if (!root.isObject()) {
                    throw std::runtime_error("`root` is not an object.");
                }
                for (const auto& i : root.getMemberNames()) {
                    this->_content[i] = root[i].asString();
                }
            }
            /**
             * @brief 从键名本地化。
             * @param key 键名。
             * @return 本地化后的字符串。
             * @return 如果键名不存在，直接返回键值。
             */
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
        /**
         * @brief 默认初始化一个叫做 default 的语言。
         */
        LanguageManager() {
            this->_languages["default"] = Language("{}");
        }
        /**
         * @brief 从 JSON 字符串中加载语言。
         * @param name 语言名称。
         * @param content JSON 字符串。
         * @throw std::runtime_error 如果 JSON 解析失败。
         */
        void load(const std::string& name, const std::string& content) {
            this->_languages[name] = Language(content);
        }
        /**
         * @brief 从 JSON 文件中加载语言。
         * @param name 语言名称。
         * @param file_name 文件名。
         * @throw std::runtime_error 如果文件读取或 JSON 解析失败。
         */
        void loadFile(const std::string& name, const std::string& file_name) {
            this->_languages[name] = Language(chh::toString(chh::readFile(file_name)));
        }
#ifdef CHH_WINDOWS
        /**
         * @brief 从 Windows 资源中加载语言。
	     * @note 该函数仅在 Windows 平台上可用。
         * @param name 语言名称。
         * @param res_name 资源 ID 。
         * @param res_type 资源类型。
         * @throw std::runtime_error 如果资源读取或 JSON 解析失败。
         */
        void loadResource(std::string name, size_t res_name, std::string res_type) {
            this->_languages[name] = Language(chh::toString(chh::readResource(res_name, res_type)));
        }
#endif
#ifdef CHH_LINUX
        /**
         * @brief 从 Windows 资源中加载语言。
         * @warning 该函数仅在 Windows 平台上可用。
         * @param name 语言名称。
         * @param res_name 资源 ID 。
         * @param res_type 资源类型。
         * @throw std::runtime_error 如果资源读取或 JSON 解析失败。
         */
        void loadResource(std::string name, size_t res_name, std::string res_type) = delete;
#endif
        /**
         * @brief 切换语言。
         * @brief 如果该语言不存在，则不执行操作。
         * @param name 语言名称。
         */
        void switchLanguage(std::string name) {
            if (this->_languages.count(name)) {
                this->_current = name;
            }
        }
        /**
         * @brief 从键名本地化。
         * @param key 键名。
         * @return 本地化后的字符串。
         * @return 如果键名不存在，直接返回键值。
         */
        std::string localize(std::string key) const {
            return this->_languages.at(this->_current).localize(key);
        }
        /**
         * @brief 获取当前选中的语言。
         * @return 当前选中的语言的名称。
         */
        std::string current() const {
            return this->_current;
        }
    private:
        std::map<std::string, Language> _languages;
        std::string _current;
    };

    /**
     * @brief 本地化单个键名。
     * @note 请使用 Text 将多个 LocalizingString 与 std::string 拼接。
     */
    class LocalizingString {
    public:
        /**
         * @brief 初始化键名。
         */
        LocalizingString(std::string key) : _key(key) {}
        /**
         * @brief 从键名本地化。
         * @param languages 语言管理器。
         * @return 本地化后的字符串。
         * @return 如果键名不存在，直接返回键值。
         */
        std::string localize(const LanguageManager& languages) const {
            return languages.localize(this->_key);
        }
    private:
        std::string _key;
    };

    /**
     * @brief 将多个 LocalizingString 与 std::string 拼接。
     */
    class Text {
    public:
        /**
         * @brief 默认构造器。
         */
        Text() = default;
        /**
         * @brief 拷贝构造器。
         */
        Text(const Text& text) = default;
        /**
         * @brief 从 LocalizingString 构造。
         */
        Text(const LocalizingString& string) : _parts({ string }) {}
        /**
         * @brief 从 std::string 构造。
         */
        Text(const std::string& string) : _parts({ string }) {}
        /**
         * @brief 从 C 风格字符串构造。
         */
        Text(const char* string) : _parts({ string }) {}
        /**
         * @brief 与另一个 Text 拼接。
         * @note LocalizingString、std::string 和 C 风格字符串会自动从 Text 构造器转换。
         */
        Text operator+(const Text& that) const {
            Text result = *this;
            result._parts.insert(result._parts.end(), that._parts.begin(), that._parts.end());
            return result;
        }
        /**
         * @brief 追加另一个 Text 。
         * @note LocalizingString、std::string 和 C 风格字符串会自动从 Text 构造器转换。
         */
        Text& operator+=(const Text& that) {
            this->_parts.insert(this->_parts.end(), that._parts.begin(), that._parts.end());
            return *this;
        }
        /**
         * @brief 从键名本地化。
         * @param languages 语言管理器。
         * @return 本地化后的字符串。
         * @return 如果键名不存在，直接返回键值。
         */
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
        /**
         * @brief 获取部件的数量。
         * @return 部件的数量。
         */
        size_t size() const {
            return this->_parts.size();
        }
    private:
        std::vector<std::variant<LocalizingString, std::string>> _parts;
        mutable std::string _cache;
        mutable std::string _cache_language;
    };

}