#pragma once

#include "chh.hpp"
#include "hti.i18n.hpp"

/**
 * @brief 控件。
 */
namespace hti::widgets {

	struct Application;

	/**
	 * @brief 控件基类。
	 */
	struct Widget {
		Widget(Widget* parent);
		~Widget() {
			for (auto& i : this->_children) delete i;
		}
		virtual std::string onRender(bool focus) { return ""; }
		virtual bool onKeyPress(char key) { return false; }
		Application* app() const { return this->_app; }
		Widget* parent() const { return this->_parent; }
		const i18n::Text& text() const { return this->_text; }
		void setText(i18n::Text text) { this->_text = text; }
		const std::vector<Widget*>& children() { return this->_children; }
		void addChild(Widget* widget) {
			this->_children.push_back(widget);
		}
	private:
		Application* _app;
		Widget* _parent;
		std::vector<Widget*> _children;
		i18n::Text _text;
		bool _focus = false;
	};

}