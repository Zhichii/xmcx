#pragma once

// C++ HillQiu's Helper, a lightweight helping header. 

#include <cstdio>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <format>
#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef _linux_
#endif
#include <json/json.h>

#define CHH_THROW_ERROR(CONTENT) CHH::error(CONTENT, __LINE__, __FILE__)

namespace CHH {

	typedef void CleanFunc(void*);
	std::vector<void*> glb_clean_schedule_data;
	std::vector<CleanFunc*> glb_clean_schedule;

	void error(std::string content, size_t line, std::string file_name) {
		std::cerr << "Error at [" << file_name << "," << line << "] because: \n" << content;
		for (size_t i = 0; i < glb_clean_schedule.size(); i++) {
			glb_clean_schedule[i](glb_clean_schedule_data[i]);
		}
		std::exit(EXIT_FAILURE);
	}

	std::string to_string(const std::vector<char>& vec) {
		return std::string(vec.begin(), vec.end());
	}

	std::vector<char> readFile(std::string file_name) {
		std::ifstream file(file_name, std::ios::binary);
		if (!file) {
			CHH_THROW_ERROR("unable to open file: " + file_name);
		}
		file.seekg(0, std::ios::end);
		std::streampos size = file.tellg();
		if (size <= 0) {
			CHH_THROW_ERROR("file is invalid: " + file_name);
		}
		file.seekg(0, std::ios::beg);
		std::vector<char> vec(size);
		if (!file.read(vec.data(), size)) {
			CHH_THROW_ERROR("unable to read file: " + file_name);
		}
		return vec;
	}

	std::vector<char> readFromResource(size_t name, std::string type) {
		HRSRC hRsrc = FindResourceA(NULL, MAKEINTRESOURCEA(name), type.c_str());
		if (!hRsrc) CHH_THROW_ERROR("Failed to find resource: " + std::to_string(name) + ".\n");
		HGLOBAL IDR = LoadResource(NULL, hRsrc);
		if (!IDR) CHH_THROW_ERROR("Failed to load resource: " + std::to_string(name) + ".\n");
		DWORD size = SizeofResource(NULL, hRsrc);
		if (size == 0) {
			FreeResource(IDR);
			CHH_THROW_ERROR("Resource size is 0: " + std::to_string(name) + ".\n");
		}
		char* arr = (char*)LockResource(IDR);
		if (!arr) {
			FreeResource(IDR);
			CHH_THROW_ERROR("Failed to lock resource: " + std::to_string(name) + ".\n");
		}
		std::vector<char> vec(arr, arr + size);
		FreeResource(IDR);
		return vec;
	}

	bool parseJson(const std::string& content, Json::Value& root) {
		Json::CharReaderBuilder reader;
		std::unique_ptr<Json::CharReader> parser(reader.newCharReader());
		std::string errs;
		bool parsingSuccessful = parser->parse(content.c_str(), content.c_str() + content.size(), &root, &errs);
		if (!parsingSuccessful) {
			CHH_THROW_ERROR("Failed to parse JSON: \n" + errs);
			return false;
		}
		return true;
	}

}