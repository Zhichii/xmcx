#include "resource.h"
#include "chh.hpp"
#include "cli.widgets.hpp"

cli::Text i18n(std::string str) {
	return cli::Text(cli::LocalizingString(str));
}

int main() {

	auto app = new cli::Application;
	app->loadLanguage("none", "Unifont", "{}");
	app->loadLanguageFromResource("zh_cn", "SimHei", IDR_LANG_ZH_CN, "lang");
	app->loadLanguageFromResource("zh_tw", "SimHei", IDR_LANG_ZH_TW, "lang");
	app->loadLanguageFromResource("en_us", "Unifont", IDR_LANG_EN_US, "lang");
	app->loadLanguageFromResource("en_jp", "SimHei", IDR_LANG_EN_JP, "lang");
	app->loadLanguageFromResource("ja_jp", "SimHei", IDR_LANG_JA_JP, "lang");
	app->switchLanguage("en_jp");

	auto menu = new cli::List(app, i18n("menu"));
	auto menu_launch = new cli::Button(menu, i18n("menu.launch"));
	auto menu_accounts = new cli::Button(menu, i18n("menu.accounts"));
	auto menu_settings = new cli::Button(menu, i18n("menu.settings"));

	app->mainloop();
	delete app;
	return 0;
}