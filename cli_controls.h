
#ifndef RIVERLAUNCHER3_CONTROLS_H
#define RIVERLAUNCHER3_CONTROLS_H
#include <string>
#include <utility>
#include <vector>
#include "language.h"
#ifdef _WIN32
#include <conio.h>
#endif

namespace CLI {
    typedef long long index;
    const int KEY_DEL = 9;
    const int KEY_TAB = 9;
    const int KEY_ESC = 27;
    const int KEY_ENTER = 13;
    const int KEY_DEL_ALL = 127;
    const int KEY_UP = 57416;
    const int KEY_DOWN = 57424;
    const int KEY_LEFT = 57419;
    const int KEY_RIGHT = 57421;
    const long long STYLE_MENU_VERTICAL = 0b1;
    const long long STYLE_MENU_TITLE_TO_LEFT = 0b100;
    const long long STYLE_MENU_TITLE_TO_RIGHT = 0b1000;
    const long long STYLE_MENU_CONTENT_TO_RIGHT = 0b10000;
    class Nothing;
    class Menu;
    class Input;
    class Test;
    class Pointer {
    protected:
        std::string type = "null";
        void* self = nullptr;
    public:
        [[nodiscard]] std::string getType() const {
            return this->type;
        }
        [[nodiscard]] Nothing* getNothingSelf() const {
            if (this->type == "nothing") return (Nothing*)this->self;
        }
        [[nodiscard]] Menu* getMenuSelf() const {
            if (this->type == "menu") return (Menu*)this->self;
        }
        [[nodiscard]] Input* getInputSelf() const {
            if (this->type == "input") return (Input*)this->self;
        }
        void reset(std::string newType, void* newSelf) {
            this->type = newType;
            this->self = newSelf;
        }
        Pointer(std::string t, void* s) {
            this->reset(t, s);
        }
        Pointer() {
            this->reset("null", nullptr);
        }
        [[nodiscard]] bool isNull() const {
            return (this->type == "null") || (this->self == nullptr);
        }
        std::string render(int X);
        std::pair<std::string, size_t> renderOrigin(int X);
        void receiveKey(int key);
        void navFrom(const Pointer& nFrom) const;
        [[nodiscard]] Pointer getNavFrom() const;
        [[nodiscard]] bool equals(const Pointer& theOther) const  {
            if (this->self == theOther.self && this->type == theOther.type) {
                return true;
            }
            return false;
        }
        void loseFocus() const;
        void gainFocus() const;
        bool isFocused() const;
    };
    typedef void KeyFunction(int, const Pointer&);
    typedef void NavigateFunction(const Pointer&, const Pointer&);
    typedef void Function(const Pointer&);
    std::string spacer(const std::string& str, int Y, int X) {
        Y-=2;
        X-=2;
        std::string str2 = warpStr(str, X);
        if (count(str2, "\n") == 0) return str2;
        std::vector<std::string> vs = split(str2, "\n");
        std::vector<std::string> vs2;
        while (vs.size() < Y) vs.push_back("");
        for (const std::string& i : vs) {
            std::string j = i;
            while (j.size() < X) {
                j.push_back('\1');
            }
            vs2.push_back(j);
        }
        return join(vs2, "\n");
    }
    std::string spacerMsg(const std::string& str, int Y, int X) {
        Y-=4;
        X-=4;
        std::string str2 = warpStr(str, X);
        if (count(str2, "\n") == 0) return str2;
        std::vector<std::string> vs = split(str2, "\n");
        std::vector<std::string> vs2;
        while (vs.size() < Y) vs.push_back("");
        for (const std::string& i : vs) {
            std::string j = i;
            while (j.size() < X) {
                j = "\1" + j;
            }
            vs2.push_back(j);
        }
        return join(vs2, "\n");
    }
    void putOntoScreen(std::vector<std::string>& vS, const std::string& s) {
        std::size_t y = 0;
        std::size_t x = 0;
        for (const char c : s) {
            if (c == '\n') {
                x = 0;
                y++;
                continue;
            }
            //if (x >= vS[0].size()) continue;
            if (y >= vS.size()) break;
            if (c == '\1') {
                vS[y][x] = ' ';
                x++;
                continue;
            }
            vS[y][x] = c;
            x++;
        }
    }
    int getKey(bool wait = false) {
        #ifdef _WIN32
            int k = 0;
            int c = 0;
            if (wait) k = _getch();
            while (_kbhit()) {
                k = 256 * k + _getch();
            }
            return k;
        #else
            int k = 0;
            int c = 0;
            if (wait) k = _getch();
            while (kbhit()) {
                c = getch();
                k = 256 * k + c;
            }
        return k;
        #endif
    }
    class Nothing {
    protected:
        Pointer ofThis{ "nothing", this };
        NavigateFunction* navFunc = [](const Pointer& self, const Pointer& newPage) {};
        Function* funcOnBeNav = [](const Pointer& self) {};
    public:
        Pointer getPtr() {
            return this->ofThis;
        }
        void setNavFrom(Pointer nFrom) {
            if (this->funcOnBeNav != nullptr) this->funcOnBeNav(this->getPtr());
            if (!nFrom.isNull()) {
                this->navFunc(this->getPtr(), nFrom);
                nFrom.gainFocus();
            }
        }
        void setNavFunc(NavigateFunction* nf) {
            this->navFunc = nf;
        }
        void setFuncOnBeNav(Function* fobn) {
            this->funcOnBeNav = fobn;
        }
    };
    class Menu {
    public:
        class Item {
        protected:
            StringVal s;
            Pointer n;
            bool e;
        public:
            Item(StringVal str, const Pointer& navigation, bool isEnabled) {
                this->s = str;
                this->n = navigation;
                this->e = isEnabled;
            }
            [[nodiscard]] StringVal getStr() const { return this->s; }
            [[nodiscard]] const Pointer& getNavigation() const { return this->n; }
            [[nodiscard]] bool isEnabled() const { return this->e; }
        };
    protected:
        Pointer navFrom;
        Pointer ofThis { "menu", this };
        StringVal name = "Menu";
        std::vector<Item> menuItems;
        KeyFunction* keyFunc = [](int key, const Pointer& ptr) {
            if (ptr.getType() != "menu") return;
            if (ptr.isNull()) return;
            Menu* menu = nullptr; menu = ptr.getMenuSelf();
            switch (key) {
            case KEY_UP:
            case KEY_LEFT:
            case 'w':
            case 'a': {
                menu->moveSelection(-1);
                break;
            }
            case KEY_DOWN:
            case KEY_RIGHT:
            case 's':
            case 'd': {
                menu->moveSelection(1);
                break;
            }
            case KEY_ESC:
            case 'x': {
                menu->navBack();
                break;
            }
            }
        };
        NavigateFunction* navFunc = [](const Pointer& self, const Pointer& newPage) {};
        Function* funcOnBeNav = [](const Pointer& self) {};
        std::vector<std::pair<StringVal, StringVal>> hint = {};
        unsigned int style = 0b0;
        index selected = -1;
        bool focus = 0;
    public:
        void loseFocus() {
            this->focus = 0;
        }
        void gainFocus() {
            this->focus = 1;
        }
        bool isFocused() {
            return this->focus;
        }
        static size_t wordLength(std::string str) {
            return str.size();
        }
        Pointer getPtr() {
            return this->ofThis;
        }
        Pointer getNavFrom() {
            return this->navFrom;
        }
        void setNavFrom(Pointer nFrom) {
            this->navFrom = nFrom;
            if (this->funcOnBeNav != nullptr) this->funcOnBeNav(this->getPtr());
        }
        void setKeyFunc(KeyFunction* kf) {
            this->keyFunc = kf;
        }
        void setNavFunc(NavigateFunction* nf) {
            this->navFunc = nf;
        }
        void setFuncOnBeNav(Function* fobn) {
            this->funcOnBeNav = fobn;
        }
        void navCurrent() {
            if (this->getSelection() >= this->getItems().size()) return;
            if (this->getSelection() < 0) return;
            const CLI::Pointer& nav = this->menuItems[this->selected].getNavigation();
            if (nav.isNull()) return;
            if (nav.equals(this->getNavFrom())) this->navBack();
            else if (nav.equals(this->getPtr()));
            else  {
                this->navFunc(this->getPtr(), nav);
                nav.gainFocus();
                this->loseFocus();
                nav.navFrom(this->getPtr());
            }
        }
        void navBack() {
            if (this->getNavFrom().isNull());
            else {
                this->getNavFrom().gainFocus();
                this->loseFocus();
                this->navFunc(this->getPtr(), this->getNavFrom());
            }
        }
        StringVal getName() {
            return this->name;
        }
        void setName(StringVal s) {
            this->name = s;
        }
        std::vector<StringVal> getItems() {
            std::vector<StringVal> vs;
            for (const auto& i : this->menuItems) vs.push_back(i.getStr());
            return vs;
        }
        void addItem(const StringVal& newItem, const Pointer& navTo = {}, const bool isEnabled = true) {
            this->menuItems.push_back(Item(newItem, navTo, isEnabled));
        }
        void clearItems() {
            this->menuItems.clear();
            this->selected = 0;
        }
        void setHint(const std::vector<std::pair<StringVal, StringVal>>& newHint = {}) {
            this->hint = newHint;
        }
        void addHint(StringVal key, StringVal val) {
            this->hint.push_back({key, val});
        }
        void setSelection(index sel = 0) {
            if (sel < 0) sel = 0;
            if (sel > this->menuItems.size()) sel = 0;
            this->selected = sel;
        }
        void moveSelection(index offset) {
            if (this->selected == -1 && offset > 0) {
                this->selected = offset-1;
                return;
            }
            if (offset < 0 && this->selected < -offset) {
                this->selected = 0;
                return;
            }
            this->selected += offset;
            if (this->selected >= this->menuItems.size()) this->selected = this->menuItems.size() - 1;
        }
        index getSelection() {
            return this->selected;
        }
        void receiveKey(int key) {
            this->keyFunc(key, this->ofThis);
        }
        std::string genContent(int X) {
            std::string o;
            std::string ln;
            for (long long i = 0; i < this->menuItems.size(); i++) {
                ln = "";
                if (i == this->selected && this->isFocused()) ln += "[";
                else if (i == this->selected && !this->isFocused()) ln += ".";
                else ln += " ";
                if ((this->style & STYLE_MENU_VERTICAL)) ln += "\1";

                std::vector<std::string> v;
                if (count(menuItems[i].getStr(), "\n") > 0) {
                    v = split(menuItems[i].getStr(), "\n");
                    ln += v[0];
                }
                else ln += menuItems[i].getStr();

                if ((this->style & STYLE_MENU_VERTICAL)) ln += "\1";
                if (i == this->selected && this->isFocused()) ln += "]";
                else if (i == this->selected && !this->isFocused()) ln += ".";
                else ln += " ";
                if (this->style & STYLE_MENU_VERTICAL) ln += "\n";
                o += ln;

                for (size_t j = 1; j < v.size(); j ++) {
                    ln = "\1\1\1\1";
                    ln += v[j];
                    if ((this->style & STYLE_MENU_VERTICAL)) {
                        while ((ln.size() % X) != 0) ln += "\1";
                        ln += "\n";
                    }
                    o += ln;
                }

            }
            return o;
        }
        std::string getHint(int X) {
            std::string h;
            std::string ln;
            for (const auto& i : this->hint) {
                ln = "";
                ln += i.first.asString() + ": " + i.second.asString() + ". ";
                ln += "\n";
                h += ln;
            }
            return h;
        }
        std::pair<std::string, size_t> renderOrigin(int X) {
            X -= 2;
            std::string ro = this->getName().asString() + "\n" + this->genContent(X) + "\n" + this->getHint(X) + "\n";
            
            int cnt = 0;
            for (char i : ro) { if (i == '\n') cnt++; }
            return { warpStr(ro, X), cnt };
        }
        static std::string stylize(std::string str, int X, int st) {
            str = warpStr(str, X);
            if (count(str, "\n") == 0) return str;
            std::vector<std::string> vs = split(str, "\n");
            size_t titleOffset = 0;
            if (st & STYLE_MENU_TITLE_TO_LEFT) titleOffset = 0;
            else if (st & STYLE_MENU_TITLE_TO_RIGHT) titleOffset = X - wordLength(vs[0]);
            else titleOffset = (X - wordLength(vs[0])) / 2;
            size_t contentOffset = 0;
            {   std::string ln;
                for (size_t i = 0; i < titleOffset; i++) {
                    ln += "\1";
                }
                ln += vs[0];
                vs[0] = ln; }
            for (std::string& i : vs) {
                std::string ln;
                if (st & STYLE_MENU_CONTENT_TO_RIGHT) contentOffset = X - wordLength(i);
                for (size_t i = 0; i < contentOffset; i++) {
                    ln += "\1";
                }
                ln += i;
                i = ln;
            }
            return join(vs, "\n");
        }
        void chStyle(long long newStyle) {
            this->style = newStyle;
        }
        long long getStyle() {
            return this->style;
        }
        Menu() {
            this->setSelection();
            this->setHint({
                {"W", "Select the prev one"},
                {"S", "Select the next one"},
                {"X", "Back to previous page"}
            });
        }
    };
    class Input {
    protected:
        Pointer navFrom;
        Pointer ofThis{ "input", this };
        StringVal prompt = "Input: ";
        std::vector<std::pair<StringVal, const Pointer>> menuItems;
        KeyFunction* keyFunc = [](int key, const Pointer& ptr) {
            if (ptr.getType() != "input") return;
            if (ptr.isNull()) return;
            Input* input= ptr.getInputSelf();
            if (!input->startInput) {
                input->startInput = true;
                return;
            }
            if (key == KEY_DEL && input->inputContent != "") input->inputContent.pop_back();
            if (key != KEY_DEL) input->inputContent += key;
            if (key == KEY_DEL_ALL) input->inputContent = "";
            if (key == KEY_ESC || key == KEY_ENTER) input->navBack();
         };
        NavigateFunction* navFunc = [](const Pointer& self, const Pointer& newPage) {};
        Function* funcOnBeNav = [](const Pointer& self) {};
        std::string inputContent = "";
        bool startInput = false;
        unsigned long long length = -1;
        unsigned int style = 0b0;
        bool focus = 0;
    public:
        void loseFocus() {
            this->focus = 0;
        }
        void gainFocus() {
            this->focus = 1;
        }
        bool isFocused() {
            return this->focus;
        }
        Pointer getPtr() {
            return this->ofThis;
        }
        Pointer getNavFrom() {
            return this->navFrom;
        }
        void setNavFrom(Pointer nFrom) {
            this->navFrom = nFrom;
            this->inputContent = "";
            this->startInput = false;
            if (this->funcOnBeNav != nullptr) this->funcOnBeNav(this->getPtr());
        }
        void setKeyFunc(KeyFunction* kf) {
            this->keyFunc = kf;
        }
        void setNavFunc(NavigateFunction* nf) {
            this->navFunc = nf;
        }
        void setFuncOnBeNav(Function* fobn) {
            this->funcOnBeNav = fobn;
        }
        void navBack() {
            if (this->getNavFrom().isNull());
            else this->navFunc(this->getPtr(), this->getNavFrom());
        }
        StringVal getPrompt() {
            return this->prompt;
        }
        void setPrompt(StringVal s) {
            this->prompt = s;
        }
        void receiveKey(int key) {
            this->keyFunc(key, this->ofThis);
        }
        std::string getInput() {
            return this->inputContent;
        }
        std::string genContent(int X) {
            std::string o = this->getPrompt().asString() + "[" + this->getInput();
            if (this->isFocused()) o += "_";

            if (length != -1) {
                while ((o.size() % X) != 0) o += " ";
            }
            else {
                while ((o.size() % X) != X-1) o += " ";
                o += "]";
            }
            return o;
        }
        std::pair<std::string, size_t> renderOrigin(int X) {
            X -= 2;
            std::string ro;

            std::string t = this->genContent(X);
            if (!t.ends_with("\n") && t != "") {
                while ((t.size() % X) != 0) t += " ";
                t += "\n";
            }
            ro += t;

            int cnt = 0;
            for (char i : ro) { if (i == '\n') cnt++; }
            return { warpStr(ro, X), cnt };
        }
        void chStyle(long long newStyle) {
            this->style = newStyle;
        }
    };
    std::string Pointer::render(int X) {
        if (this->isNull()) return "";
        std::string s;
        Pointer ptr;
        ptr = this->getNavFrom();
        while (!ptr.isNull()) {
            auto t = ptr.renderOrigin(X);
            s = t.first + s;
            ptr = ptr.getNavFrom();
        }
        s += this->renderOrigin(X).first;
        return s;
    }
    [[nodiscard]] Pointer Pointer::getNavFrom() const {
        if (this->isNull()) return {};
        if (this->getType() == "nothing") return {};
        if (this->getType() == "menu") return this->getMenuSelf()->getNavFrom();
        if (this->getType() == "input") return this->getInputSelf()->getNavFrom();
        return {};
    }
    std::pair<std::string, size_t> Pointer::renderOrigin(int X) {
        std::pair<std::string, int> s;
        if (this->isNull()) return { "", 0 };
        if (this->getType() == "menu") {
            s = this->getMenuSelf()->renderOrigin(X);
            std::string a = s.first;
            a = CLI::Menu::stylize(a, X - 2, this->getMenuSelf()->getStyle());
            a = warpStr(a, X);
            s.first = a;
        }
        if (this->getType() == "input") s = this->getInputSelf()->renderOrigin(X);
        return s;
    }
    void Pointer::receiveKey(int key) {
        if (this->isNull()) return;;
        if (this->getType() == "menu") this->getMenuSelf()->receiveKey(key);
        if (this->getType() == "input") this->getInputSelf()->receiveKey(key);
    }
    void Pointer::navFrom(const Pointer& nFrom) const {
        if (this->isNull()) return;;
        if (this->getType() == "nothing") return this->getNothingSelf()->setNavFrom(nFrom);
        if (this->getType() == "menu") this->getMenuSelf()->setNavFrom(nFrom);
        if (this->getType() == "input") this->getInputSelf()->setNavFrom(nFrom);
    }
    void Pointer::loseFocus() const {
        if (this->isNull()) return;;
        if (this->getType() == "menu") this->getMenuSelf()->loseFocus();
        if (this->getType() == "input") this->getInputSelf()->loseFocus();
    }
    void Pointer::gainFocus() const {
        if (this->isNull()) return;;
        if (this->getType() == "menu") this->getMenuSelf()->gainFocus();
        if (this->getType() == "input") this->getInputSelf()->gainFocus();
    }
    bool Pointer::isFocused() const {
        if (this->isNull()) return false;;
        if (this->getType() == "menu") this->getMenuSelf()->isFocused();
        if (this->getType() == "input") this->getInputSelf()->isFocused();
    }
}
#endif //RIVERLAUNCHER3_CONTROLS_H
