#include "resource.h"
#include "hti.hpp"

hti::i18n::Text i18n(std::string str) {
	return hti::i18n::Text(hti::i18n::LocalizingString(str));
}

int main() {

	auto app = new hti::widgets::Application;
	app->loadLanguageFromResource("zh_cn", IDR_LANG_ZH_CN, "lang");
	app->loadLanguageFromResource("zh_tw", IDR_LANG_ZH_TW, "lang");
	app->loadLanguageFromResource("en_us", IDR_LANG_EN_US, "lang");
	app->loadLanguageFromResource("en_jp", IDR_LANG_EN_JP, "lang");
	app->loadLanguageFromResource("ja_jp", IDR_LANG_JA_JP, "lang");
	app->switchLanguage("zh_cn");

	auto menu = new hti::widgets::List(app, i18n("menu"));
	auto menu_launch = new hti::widgets::Button(menu, i18n("menu.launch"));
	auto menu_accounts = new hti::widgets::Button(menu, i18n("menu.accounts"));
	auto menu_settings = new hti::widgets::Button(menu, i18n("menu.settings"));

	auto menu_menu1 = new hti::widgets::List(menu, i18n("menu") + " 1");
	auto menu_default = new hti::widgets::Button(menu_menu1, "default");
	menu_default->bind([&](hti::widgets::Widget*) {
		app->switchLanguage("default");
		});
	auto menu_zh_cn = new hti::widgets::Button(menu_menu1, "zh_cn");
	menu_zh_cn->bind([&](hti::widgets::Widget*) {
		app->switchLanguage("zh_cn");
		});
	auto menu_zh_tw = new hti::widgets::Button(menu_menu1, "zh_tw");
	menu_zh_tw->bind([&](hti::widgets::Widget*) {
		app->switchLanguage("zh_tw");
		});
	auto menu_en_us = new hti::widgets::Button(menu_menu1, "en_us");
	menu_en_us->bind([&](hti::widgets::Widget*) {
		app->switchLanguage("en_us");
		});
	auto menu_en_jp = new hti::widgets::Button(menu_menu1, "en_jp");
	menu_en_jp->bind([&](hti::widgets::Widget*) {
		app->switchLanguage("en_jp");
		});
	auto menu_ja_jp = new hti::widgets::Button(menu_menu1, "ja_jp");
	menu_ja_jp->bind([&](hti::widgets::Widget*) {
		app->switchLanguage("ja_jp");
		});

	auto menu_menu2 = new hti::widgets::List(menu, i18n("menu") + " 2");
	auto menu_text1 = new hti::widgets::Button(menu_menu2, i18n("text.1"));
	auto menu_text2 = new hti::widgets::Button(menu_menu2, i18n("text.2"));
	auto menu_text3 = new hti::widgets::Button(menu_menu2, i18n("text.3"));
	auto menu_text4 = new hti::widgets::Button(menu_menu2, i18n("text.4"));

	auto menu_exit = new hti::widgets::Button(menu, i18n("exit"));
	menu_exit->bind([&](hti::widgets::Widget*) {
		app->exit();
		});

	app->mainloop();
	delete app;
	return 0;
}