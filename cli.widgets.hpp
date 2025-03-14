#pragma once

// Widgets of a lightweight CLI framework.

#include "chh.hpp"
#include "cli.localize.hpp"

namespace CLI {

    // Move the console cursor to (0,0).
    void cls() {
#ifdef _WIN32
        COORD coord;
        coord.X = 0; coord.Y = 0;
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(console, coord);
#endif
#ifdef _linux_
#endif
    }

	struct Widget;

	typedef void _Function(Widget*);
	typedef std::function<_Function> Function;

	// The base class for widgets.
	struct Widget {
		Widget(Widget* parent);
		~Widget() {
			for (auto& i : this->_children) delete i;
		}
		virtual std::string onRender(bool focus) { return ""; }
		virtual bool onKeyPress(char key) { return false; }
		Application* app() const { return this->_app; }
		Widget* parent() const { return this->_parent; }
		const Text& text() const { return this->_text; }
		void setText(Text text) { this->_text = text; }
		const std::vector<Widget*>& children() { return this->_children; }
		void addChild(Widget* widget) {
			this->_children.push_back((Widget*)widget);
		}
	private:
		Application* _app;
		Widget* _parent;
		std::vector<Widget*> _children;
		Text _text;
		bool _focus = false;
	};

	// The main entry for the whole application.
	struct Application : Widget {
		Application() : Widget(nullptr) {
#ifdef _WIN32
			this->_console = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(this->_console, &cci);
			cci.bVisible = false;
			SetConsoleCursorInfo(this->_console, &cci);
			SetConsoleOutputCP(CP_UTF8);
#endif
#ifdef _linux_
#endif
		}
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
				cls();
				printf("%s", this->onRender(true).c_str());
				if (_kbhit()) {
					char key = _getch();
					this->onKeyPress(key);
					if (key == 'z') break;
				}
				Sleep(1000 / 60);
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
		const LanguageManager& languages() { return this->_languages; }
		void loadLanguage(std::string name, std::string content) {
			this->_languages.loadLanguage(name, content);
		}
		void loadLanguageFromFile(std::string name, std::string file_name) {
			this->_languages.loadLanguageFromFile(name, file_name);
		}
		void loadLanguageFromResource(std::string name, size_t res_name, std::string res_type) {
			this->_languages.loadLanguageFromResource(name, res_name, res_type);
		}
		void switchLanguage(std::string language) {
			this->_languages.switchLanguage(language);
		}
		void exit() { this->_should_exit = true; }
	private:
		std::vector<Widget*> _widgets;
		size_t _focus = 0;
		size_t _displaying = 0;
		HANDLE _console;
		LanguageManager _languages;
		bool _should_exit = false;
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

	// The button class.
	struct Button : Widget {
		Button(Widget* parent, Text label) : Widget(parent) {
			this->setText(label);
		}
		std::string onRender(bool focus) {
			if (focus) return "[" + this->text().localize(this->app()->languages()) + "]";
			else return "." + this->text().localize(this->app()->languages()) + ".";
		}
		void bind(Function activate) {
			this->_on_activate = activate;
		}
		bool onKeyPress(char key) {
			if (key == ' ') {
				if (_on_activate) this->_on_activate(this);
				return true;
			}
			else return false;
		}
	private:
		Function _on_activate = nullptr;
	};

	// Vertical or horizontal layout.
	struct List : Widget {
		const static size_t STYLE_VERTICAL = 0x0;
		const static size_t STYLE_HORIZONTAL = 0x1;
		size_t style = STYLE_VERTICAL;
		size_t selected = 0;
		List(Widget* parent, Text title = {}) : Widget(parent) {
			this->setText(title);
		}
		~List() {}
		std::string onRender(bool focus) {
			std::string o;
			if (this->text().parts_size() != 0) {
				if (focus) o += "<" + this->text().localize(this->app()->languages()) + ">\n";
				else o += " " + this->text().localize(this->app()->languages()) + " \n";
			}
			for (size_t j = 0; j < this->children().size(); j++) {
				auto& i = this->children().at(j);
				o += i->onRender(focus && (this->selected == j));
				if (j != this->children().size() - 1) {
					if (this->style == STYLE_VERTICAL) o += "\n";
					if (this->style == STYLE_HORIZONTAL) o += " ";
				}
			}
			return o;
		}
		bool onKeyPress(char key) {
			bool did = this->children().at(this->selected)->onKeyPress(key);
			if (did) return true;
			if (key == 'w') {
				if (this->selected > 0) {
					this->selected--;
					return true;
				}
			}
			if (key == 's') {
				if (this->selected < this->children().size()-1) { 
					this->selected++;
					return true;
				}
			}
			return false;
		}
	};

}