#pragma once

#include "hti.widgets.application.hpp"

/**
 * @brief 控件。
 */
namespace hti::widgets {

	/**
	 * @brief 按钮按下的回调函数。
	 */
	typedef std::function<void(Widget*)> Function;

	/**
	 * @brief 按钮。
	 */
	struct Button : Widget {
		Button(Widget* parent, i18n::Text label) : Widget(parent) {
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

}