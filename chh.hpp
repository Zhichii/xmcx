#pragma once

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <codecvt>
#include <vector>
#include <map>
#include <any>
#include <variant>
#include <functional>
#include <exception>
#include <stdexcept>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#define CHH_WINDOWS
#define getch _getch
#define kbhit _kbhit
#define sleep Sleep
#undef max
#undef min
#endif

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#define CHH_LINUX
#define sleep usleep
int getch() {
	termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}
int kbhit() {
	termios oldt, newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if (ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}
#endif
#include <json/json.h>

/**
 * @brief 一个轻量级的的 C++ 工具箱。
 */
namespace chh {

	/**
	 * @brief 将字符向量转换为字符串。
	 * @param vec 字符向量。
	 * @return 转换后的字符串。
	 */
	std::string toString(const std::vector<char>& vec) {
		return std::string(vec.begin(), vec.end());
	}

	/**
	 * @brief 将宽字符串转换为窄字符串。
	 * @param wstr 宽字符串。
	 * @return 转换后的窄字符串。
	 * @throw std::runtime_error 如果转换失败。
	 */
	std::string toString(const std::wstring& wstr) {
		if (wstr.empty()) return std::string();
		size_t size_needed = std::wcstombs(nullptr, wstr.c_str(), 0);
		if (size_needed == static_cast<size_t>(-1)) {
			throw std::runtime_error("Invalid wide character encountered during conversion.");
		}
		std::string str(size_needed, 0);
		std::wcstombs(str.data(), wstr.c_str(), size_needed);
		return str;
	}

	/**
	 * @brief 将窄字符串转换为宽字符串。
	 * @param str 宽字符串。
	 * @return 转换后的窄字符串。
	 * @throw std::runtime_error 如果转换失败。
	 */
	std::wstring toWString(const std::string& str) {
		if (str.empty()) return std::wstring();
		size_t size_needed = std::mbstowcs(nullptr, str.c_str(), 0);
		if (size_needed == static_cast<size_t>(-1)) {
			throw std::runtime_error("Invalid character encountered during conversion.");
		}
		std::wstring wstr(size_needed, 0);
		std::mbstowcs(wstr.data(), str.c_str(), size_needed);
		return wstr;
	}

	/**
	 * @brief 从文件中读取字符向量。
	 * @param file_name 文件名。
	 * @return 文件内容。
	 * @throw std::runtime_error 如果文件读取失败。
	 */
	std::vector<char> readFile(const std::string& file_name) {
		try {
			std::ifstream file(file_name, std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Unable to open file: " + file_name + ".");
			}
			std::uintmax_t size = std::filesystem::file_size(file_name);
			if (size > std::numeric_limits<std::streamsize>::max()) {
				throw std::runtime_error("File too large to read into a vector.");
			}
			std::vector<char> vec(static_cast<std::size_t>(size));
			if (!file.read(vec.data(), static_cast<std::streamsize>(size))) {
				throw std::runtime_error("Unable to read file: " + file_name + ".");
			}
			return vec;
		}
		catch (const std::filesystem::filesystem_error& e) {
			throw std::runtime_error("Filesystem error: " + std::string(e.what()) + ".");
		}
	}

#ifdef CHH_WINDOWS
	/**
	 * @brief 从 Windows 资源中读取字符向量。
	 * @note 该函数仅在 Windows 平台上可用。
	 * @param name 资源 ID 。
	 * @param type 资源类型。
	 * @return 资源内容。
	 * @throw std::runtime_error 如果资源读取失败。
	 */
	std::vector<char> readResource(const size_t& name, const std::string& type) {
		HRSRC hRsrc = FindResourceA(NULL, MAKEINTRESOURCEA(name), type.c_str());
		if (!hRsrc) {
			throw std::runtime_error("Failed to find resource: " + std::to_string(name) + ".");
		}
		HGLOBAL IDR = LoadResource(NULL, hRsrc);
		if (!IDR) {
			throw std::runtime_error("Failed to load resource: " + std::to_string(name) + ".");
		}
		DWORD size = SizeofResource(NULL, hRsrc);
		if (size == 0) {
			FreeResource(IDR);
			throw std::runtime_error("Resource size is 0: " + std::to_string(name) + ".");
		}
		char* arr = (char*)LockResource(IDR);
		if (!arr) {
			FreeResource(IDR);
			throw std::runtime_error("Failed to lock resource: " + std::to_string(name) + ".");
		}
		std::vector<char> vec(arr, arr + size);
		FreeResource(IDR);
		return vec;
	}
#endif
#ifdef CHH_LINUX
	/**
	 * @brief 从 Windows 资源中读取字符向量。
	 * @warning 该函数仅在 Windows 平台上可用。
	 * @param name 资源 ID 。
	 * @param type 资源类型。
	 * @return 资源内容。
	 * @throw std::runtime_error 如果资源读取失败。
	 */
	std::vector<char> readResource(const size_t& name, const std::string& type) = delete;
#endif

	/**
	 * @brief 从 JSON 字符串中解析对象。
	 * @param content JSON 字符串。
	 * @return JSON 对象。
	 * @throws std::runtime_error 如果 JSON 解析失败。
	 */
	Json::Value parseJson(const std::string& content) {
		Json::Value root;
		Json::CharReaderBuilder reader;
		std::unique_ptr<Json::CharReader> parser(reader.newCharReader());
		std::string errs;
		if (!parser->parse(content.c_str(), content.c_str() + content.size(), &root, &errs)) {
			throw std::runtime_error("Failed to parse JSON: \n" + errs);
		}
		return root;
	}

}