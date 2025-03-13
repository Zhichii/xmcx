#include "resource.h"
#include "cli.basic.hpp"
#include "cli.widgets.hpp"

CLI::Text i18n(std::string str) {
	return CLI::Text(CLI::LocalizingString(str));
}

int main() {
	auto app = new CLI::Application;
	app->loadLanguageFromResource("none", IDR_LANG_NONE, "lang");
	app->loadLanguageFromResource("en_us", IDR_LANG_EN_US, "lang");
	app->loadLanguageFromResource("zh_cn", IDR_LANG_ZH_CN, "lang");
	app->switchLanguage("zh_cn");

	auto menu = new CLI::List(app, i18n("menu"));
	auto menu_menu1 = new CLI::List(menu, i18n("menu") + " 1");
	auto menu_launch = new CLI::Button(menu_menu1, i18n("menu.launch"));
	menu_launch->bind([&](CLI::Widget*) {
		app->switchLanguage("en_us");
		});
	auto menu_xxx = new CLI::Button(menu_menu1, i18n("menu.launch"));
	auto menu_menu2 = new CLI::List(menu, i18n("menu") + " 2");
	auto menu_accounts = new CLI::Button(menu_menu2, i18n("menu.accounts"));
	auto menu_settings = new CLI::Button(menu_menu2, i18n("menu.settings"));

	app->mainloop();
	delete app;
	return 0;
}