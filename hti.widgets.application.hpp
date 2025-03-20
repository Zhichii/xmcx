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
#if CHH_IS_WINDOWS
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
#elif CHH_IS_LINUX
		Application() : Widget(nullptr) {
			setlocale(LC_ALL, "zh_cn.utf8");
		}
#endif
#if CHH_IS_WINDOWS
		~Application() {
			CloseHandle(this->_console);
		}
#elif CHH_IS_LINUX
		~Application() = default;
#endif
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
				printf("\033[H"); // 移动光标到 (0, 0) 。
				std::string temp = this->onRender(true) + "\n";
				int width, height;
				getConsoleSize(width, height);
				// 用空格填充每一行。
				char* buffer = new char[width+1];
				memset(buffer, ' ', width);
				buffer[width] = 0;
				int y = 0, x = 0;
				std::ostringstream oss;
				for (const auto& i : temp) {
					if (y >= height-1) break;
					switch (i) {
					default: {
						if (x >= width); // 下落到换行。
						else {
							buffer[x] = i;
							x++;
							break;
						}
						[[fallthrough]];
					}
					case '\n': {
						oss << buffer << "\n";
						memset(buffer, ' ', width);
						buffer[width] = 0;
						y++;
						// 挺有趣的：y++ 相当于 \n, x=0 相当于 \r.
						[[fallthrough]];
					}
					case '\r': {
						x = 0;
						break;
					}
					case '\b': {
						x--;
						break;
					}
					}
				}
				printf("%s", oss.str().c_str());
				if (kbhit()) {
					this->onKeyPress(getch());
				}
				sleep(30);
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
#if CHH_IS_WINDOWS
		void loadLanguageFromResource(const std::string& name, const size_t& res_name, const std::string& res_type) {
			this->_languages.loadResource(name, res_name, res_type);
		}
#elif CHH_IS_LINUX
		void loadLanguageFromResource(const std::string& name, const size_t& res_name, const std::string& res_type) = delete;
#endif
		void switchLanguage(const std::string& name) {
			this->_languages.switchLanguage(name);
		}
		void exit() {
			this->_should_exit = true;
		}
	private:
#if CHH_IS_WINDOWS
		HANDLE _console;
#endif
		std::vector<Widget*> _widgets;
		size_t _focus = 0;
		size_t _displaying = 0;
		i18n::LanguageManager _languages;
		bool _should_exit = false;
		void getConsoleSize(int& width, int& height) {
#ifdef _WIN32
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
			width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
			height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
			struct winsize w;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
			width = w.ws_col;
			height = w.ws_row;
#endif
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