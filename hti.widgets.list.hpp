#pragma once

#include "hti.widgets.application.hpp"

namespace hti::widgets {

	/**
	 * @brief 横着或竖着的列表。
	 */
	struct List : Widget {
		const static size_t STYLE_VERTICAL = 0x0;
		const static size_t STYLE_HORIZONTAL = 0x1;
		List(Widget* parent, i18n::Text title = {}) : Widget(parent) {
			this->setText(title);
		}
		~List() {}
		std::string onRender(bool focus) {
			std::string o;
			if (this->text().size() != 0) {
				if (focus) o += "<" + this->text().localize(this->app()->languages()) + ">";
				else o += " " + this->text().localize(this->app()->languages());
			}
			for (size_t j = 0; j < this->children().size(); j++) {
				if (this->_style == STYLE_VERTICAL) o += "\n";
				if (this->_style == STYLE_HORIZONTAL) o += " ";
				auto& i = this->children().at(j);
				o += i->onRender(focus && (this->_selected == j));
			}
			return o;
		}
		bool onKeyPress(char key) {
			if (this->children().size() == 0) return false;
			bool did = this->children().at(this->_selected)->onKeyPress(key);
			if (did) return true;
			if (key == 'w') {
				if (this->_selected > 0) {
					this->_selected--;
					return true;
				}
			}
			if (key == 's') {
				if (this->_selected < this->children().size() - 1) {
					this->_selected++;
					return true;
				}
			}
			return false;
		}
	private:
		size_t _style = STYLE_VERTICAL;
		size_t _selected = 0;
	};

}