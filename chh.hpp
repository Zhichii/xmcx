#pragma once

// C++ HillQiu's Helper: A lightweight C++ header with file, resource, and JSON loading.

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
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
#undef max
#undef min
#endif
#ifdef __linux__
#endif
#include <json/json.h>

namespace CHH {

	// Converts a vector of characters to a string.
	std::string toString(const std::vector<char>& vec) {
		return std::string(vec.begin(), vec.end());
	}

	// Reads the contents of a file into a vector of characters.
	std::vector<char> readFile(std::string file_name) {
		try {
			std::ifstream file(file_name, std::ios::binary);
			if (!file) {
				throw std::runtime_error("Unable to open file: " + file_name);
			}
			std::uintmax_t size = std::filesystem::file_size(file_name);
			if (size > std::numeric_limits<std::streamsize>::max()) {
				throw std::runtime_error("File too large to read into a vector.");
			}
			std::vector<char> vec(static_cast<std::size_t>(size));
			if (!file.read(vec.data(), static_cast<std::streamsize>(size))) {
				throw std::runtime_error("Unable to read file: " + file_name);
			}
			return vec;
		}
		catch (const std::filesystem::filesystem_error& e) {
			throw std::runtime_error("Filesystem error: " + std::string(e.what()));
		}
	}

	// Reads a resource from the system by its name and type.
	std::vector<char> readFromResource(size_t name, std::string type) {
#ifdef _WIN32
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
#endif
#ifdef __linux__
#endif
	}

	// Parses a JSON string into a JSON object.
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