#include "resource.h"
#include "chh.hpp"
#include "cli.widgets.hpp"

cli::Text i18n(std::string str) {
	return cli::Text(cli::LocalizingString(str));
}

int main() {

	auto app = new cli::Application;
	app->loadLanguage("none", "{}");
	app->loadLanguageFromResource("zh_cn", IDR_LANG_ZH_CN, "lang");
	app->loadLanguageFromResource("zh_tw", IDR_LANG_ZH_TW, "lang");
	app->loadLanguageFromResource("en_us", IDR_LANG_EN_US, "lang");
	app->loadLanguageFromResource("en_jp", IDR_LANG_EN_JP, "lang");
	app->loadLanguageFromResource("ja_jp", IDR_LANG_JA_JP, "lang");
	app->switchLanguage("zh_cn");

	auto menu = new cli::List(app, i18n("menu"));
	auto menu_launch = new cli::Button(menu, i18n("menu.launch"));
	auto menu_accounts = new cli::Button(menu, i18n("menu.accounts"));
	auto menu_settings = new cli::Button(menu, i18n("menu.settings"));

	auto menu_menu1 = new cli::List(menu, i18n("menu") + " 1");
	auto menu_zh_cn = new cli::Button(menu_menu1, "zh_cn");
	menu_zh_cn->bind([&](cli::Widget*) {
		app->switchLanguage("zh_cn");
		});
	auto menu_zh_tw = new cli::Button(menu_menu1, "zh_tw");
	menu_zh_tw->bind([&](cli::Widget*) {
		app->switchLanguage("zh_tw");
		});
	auto menu_en_us = new cli::Button(menu_menu1, "en_us");
	menu_en_us->bind([&](cli::Widget*) {
		app->switchLanguage("en_us");
		});
	auto menu_en_jp = new cli::Button(menu_menu1, "en_jp");
	menu_en_jp->bind([&](cli::Widget*) {
		app->switchLanguage("en_jp");
		});
	auto menu_ja_jp = new cli::Button(menu_menu1, "ja_jp");
	menu_ja_jp->bind([&](cli::Widget*) {
		app->switchLanguage("ja_jp");
		});

	auto menu_menu2 = new cli::List(menu, i18n("menu") + " 2");
	auto menu_text1 = new cli::Button(menu_menu2, i18n("text.1"));
	auto menu_text2 = new cli::Button(menu_menu2, i18n("text.2"));
	auto menu_text3 = new cli::Button(menu_menu2, i18n("text.3"));
	auto menu_text4 = new cli::Button(menu_menu2, i18n("text.4"));

	auto menu_exit = new cli::Button(menu, i18n("exit"));
	menu_exit->bind([&](cli::Widget*) {
		app->exit();
		});

	app->mainloop();
	delete app;
	return 0;
}