#include "resource.h"
#include "cli.basic.hpp"
#include "cli.widgets.hpp"

int main() {
	CLI::Application app;
	app.languages.loadLanguageFromResource("none", IDR_LANG_NONE, "lang");
	app.languages.loadLanguageFromResource("en_us", IDR_LANG_EN_US, "lang");
	app.languages.loadLanguageFromResource("zh_cn", IDR_LANG_ZH_CN, "lang");
	app.languages.current = "zh_cn";
	app.add_child(new CLI::List(app, { "menu" }));
	app.children[0]->add_child(new CLI::Button(app, {"menu.launch"}));
	app.children[0]->add_child(new CLI::Button(app, {"menu.accounts"}));
	app.children[0]->add_child(new CLI::Button(app, {"menu.settings"}));
	app.mainloop();
	return 0;
}