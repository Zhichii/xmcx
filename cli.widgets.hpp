#pragma once

#include "cli.basic.hpp"
#include "cli.localize.hpp"

namespace CLI {

	struct Widget {
		Application& app;
		bool focus = false;
		std::vector<Widget*> children;
		LocalizingText text;
		Widget(Application& app) : app(app) {
			app.widgets.push_back(*this);
		}
		~Widget() {
			for (auto& i : this->children) delete i;
		}
		void addChild(Widget* widget) {
			this->children.push_back((Widget*)widget);
		}
		virtual std::string onRender() { return {}; }
		virtual void onKeyPress(char key) {}
	};

	struct Application : Widget {
		std::vector<Widget&> widgets;
		size_t selected = 0;
		HANDLE console;
		LanguageManager languages;
		Application() : Widget(*this) {
#ifdef _WIN32
			this->console = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(this->console, &cci);
			cci.bVisible = false;
			SetConsoleCursorInfo(this->console, &cci);
			SetConsoleOutputCP(CP_UTF8);
#endif
#ifdef _linux_
#endif
		}
		~Application() {}
		std::string onRender() {
			if (this->selected < this->children.size())
				return this->children[selected]->onRender();
			else return "";
		}
		void onKeyPress(char key) {
			for (auto& i : this->children) {
				i->onKeyPress(key);
			}
		}
		void mainloop() {
			while (1) {
				cls();
				printf("%s", this->onRender().c_str());
				for (size_t j = 0; j < this->children.size(); j++) {
					auto& i = this->children[j];
					if (this->selected == j) i->focus = true;
					else i->focus = false;
				}
				if (_kbhit()) {
					char key = _getch();
					this->onKeyPress(key);
					if (key == 'z') break;
				}
				Sleep(1000 / 60);
			}
		}
	};

	struct Button : Widget {
		Button(Application& app, LocalizingText label) : Widget(app) {
			this->text = label;
		}
		std::string onRender() {
			if (this->focus) return "[" + this->text.localize(this->app.languages) + "]";
			else return "." + this->text.localize(this->app.languages) + ".";
		}
		void onKeyPress(char key) {
			if (this->focus) {
				if (key == ' ');
			}
		}
	};

	struct List : Widget {
		const static size_t STYLE_VERTICAL = 0x0;
		const static size_t STYLE_HORIZONTAL = 0x1;
		size_t style = STYLE_VERTICAL;
		size_t selected = 0;
		List(Application& app, LocalizingText title = {}) : Widget(app) {
			this->text = title;
		}
		~List() {}
		std::string onRender() {
			std::string o;
			if (this->text.key != "") o += this->text.localize(this->app.languages) + "\n";
			for (size_t j = 0; j < this->children.size(); j++) {
				auto& i = this->children[j];
				if (this->focus && this->selected == j) i->focus = true;
				else i->focus = false;
				o += i->onRender();
				if (this->style == STYLE_VERTICAL) o += "\n";
				if (this->style == STYLE_HORIZONTAL) o += " ";
			}
			return o;
		}
		void onKeyPress(char key) {
			if (this->focus) {
				if (key == 'w') if (this->selected > 0) this->selected--;
				if (key == 's') if (this->selected < this->children.size() - 1) this->selected++;
				for (auto& i : this->children) {
					i->onKeyPress(key);
				}
			}
		}
	};

}