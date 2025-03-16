#pragma once

#include "hti.widgets.widget.hpp"

/**
 * @brief 控件。
 */
namespace hti::widgets {

	/**
	 * @brief 整个程序的入口。
	 */
	struct Application : Widget {
#ifdef CHH_WINDOWS
		Application() : Widget(nullptr) {
			this->_console = GetStdHandle(STD_OUTPUT_HANDLE);
			HWND hWnd = GetConsoleWindow();
			char buffer[129]; GetClassNameA(hWnd, buffer, 128);
			if (std::string(buffer) == "ConsoleWindowClass") {
				printf("Unsupported console. Please use Windows Terminal.");
				this->_should_exit = true;
				return;
			}
			CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(this->_console, &cci);
			cci.bVisible = false;
			SetConsoleCursorInfo(this->_console, &cci);
			setlocale(LC_ALL, "zh_cn.utf8");
			SetConsoleOutputCP(CP_UTF8);
		}
#endif
#ifdef CHH_LINUX
		Application() : Widget(nullptr) {
			setlocale(LC_ALL, "zh_cn.utf8");
		}
#endif
		~Application() = default;
		std::string onRender(bool focus) {
			if (this->_displaying < this->children().size())
				return this->children()[_displaying]->onRender(true);
			else return "";
		}
		bool onKeyPress(char key) {
			auto widget = this->_widgets[this->_focus];
			return widget->onKeyPress(key);
		}
		void mainloop() {
			while (!this->_should_exit) {
				this->cls();
				printf("%s", this->onRender(true).c_str());
				if (kbhit()) {
					char key = getch();
					this->onKeyPress(key);
				}
				sleep(1000 / 60);
			}
		}
		const std::vector<Widget*>& widgets() { return this->_widgets; }
		void addWidget(Widget* widget) { this->_widgets.push_back(widget); }
		Widget& focus() { return *this->_widgets.at(this->_focus); }
		void setFocus(Widget* widget) {
			for (size_t i = 0; i < this->_widgets.size(); i++) {
				if (this->_widgets[i] == widget) {
					this->_focus = i;
					break;
				}
			}
		}
		const i18n::LanguageManager& languages() { return this->_languages; }
		void loadLanguage(const std::string& name, const std::string& content) {
			this->_languages.load(name, content);
		}
		void loadLanguageFromFile(const std::string& name, const std::string& file_name) {
			this->_languages.loadFile(name, file_name);
		}
#ifdef CHH_WINDOWS
		void loadLanguageFromResource(const std::string& name, const size_t& res_name, const std::string& res_type) {
			this->_languages.loadResource(name, res_name, res_type);
		}
#endif
#ifdef CHH_LINUX
		void loadLanguageFromResource(const std::string& name, const size_t& res_name, const std::string& res_type) = delete;
#endif
		void switchLanguage(const std::string& name) {
			this->_languages.switchLanguage(name);
		}
		void exit() {
			this->_should_exit = true;
		}
	private:
#ifdef CHH_WINDOWS
		HANDLE _console;
#endif
		std::vector<Widget*> _widgets;
		size_t _focus = 0;
		size_t _displaying = 0;
		i18n::LanguageManager _languages;
		bool _should_exit = false;
		// Move the console cursor to (0,0).
		void cls() {
			printf("\033[H");
		}
	};

	Widget::Widget(Widget* parent) {
		this->_parent = parent;
		if (this->_parent) {
			this->_app = this->_parent->_app;
			this->_app->addWidget(this);
			this->_parent->_children.push_back(this);
		}
		else this->_app = (Application*)this;
	}

}