#include "resource.h"
#include "chh.hpp"
#include "cli.widgets.hpp"

CLI::Text i18n(std::string str) {
	return CLI::Text(CLI::LocalizingString(str));
}

int main() {
	auto app = new CLI::Application;
	app->loadLanguageFromResource("none", IDR_LANG_NONE, "lang");
	app->loadLanguageFromResource("en_us", IDR_LANG_EN_US, "lang");
	app->loadLanguageFromResource("zh_cn", IDR_LANG_ZH_CN, "lang");
	app->loadLanguageFromResource("zh_tw", IDR_LANG_ZH_TW, "lang");
	app->switchLanguage("zh_cn");

	auto menu = new CLI::List(app, i18n("menu"));
	auto menu_menu1 = new CLI::List(menu, i18n("menu") + " 1");
	auto menu_none = new CLI::Button(menu_menu1, "none");
	menu_none->bind([&](CLI::Widget*) {
		app->switchLanguage("none");
		});
	auto menu_en_us = new CLI::Button(menu_menu1, "en_us");
	menu_en_us->bind([&](CLI::Widget*) {
		app->switchLanguage("en_us");
		});
	auto menu_menu2 = new CLI::List(menu, i18n("menu") + " 2");
	auto menu_zh_cn = new CLI::Button(menu_menu2, "zh_cn");
	menu_zh_cn->bind([&](CLI::Widget*) {
		app->switchLanguage("zh_cn");
		});
	auto menu_zh_tw = new CLI::Button(menu_menu2, "zh_tw");
	menu_zh_tw->bind([&](CLI::Widget*) {
		app->switchLanguage("zh_tw");
		});

	app->mainloop();
	delete app;
	return 0;
}