
#include <SFML/Graphics.hpp>

#include "drawings.h"

#include "simple-ui/theme.h"
#include "simple-ui/win.h"
#include "simple-ui/default_font.h"
#include "simple-ui/menu.h"


namespace dfe_ui
{
    

MenuItem::MenuItem(PopupMenu *menu, const std::wstring & text, icon_type_t icon_type) {
    m_menu = menu;
    m_icon_type = icon_type;
    m_text = text;
}

MenuItem::~MenuItem() {
}

void MenuItem::onclick(component_event_t cb) {
    m_onclick = cb;
}

component_event_t MenuItem::onclick() {
    return m_onclick;
}

std::shared_ptr<PopupMenu> MenuItem::submenu() {
    return m_submenu;
}

const std::wstring & MenuItem::text() {
    return m_text;
}

icon_type_t MenuItem::icon_type() {
    return m_icon_type;
}

void MenuItem::submenu(std::shared_ptr<PopupMenu> value) {
    m_submenu = value;
}

bool MenuItem::has_icon() {
    return m_icon_type != img_none;
}

bool MenuItem::has_submenu() {
    return m_submenu.get() != NULL;
}

PopupMenu::PopupMenu(Window *window) : Component(window) {
    this->coordinates(0, 0, 1, 1);
    auto font = load_default_font();
    if (font) {
        m_text.reset(new sf::Text(*font));
        update_text_min_y();
    }
}

PopupMenu::~PopupMenu() {
}

MenuItem *PopupMenu::add(const std::wstring & text, icon_type_t icon_type) {
    auto item = std::make_shared<MenuItem>(this, text, icon_type);
    m_items.push_back(item);
    return item.get();
}

void PopupMenu::handle_parent_resized() {
    if (!m_text) return;

    int h = m_items.size() * 30;

    if (h < 30) {
        h = 30;
    }

    int w = 4;
    bool has_submenu = false;
    bool has_icon = false;
    m_text->setPosition({0, 0});
    static bool first_time = true;

    for (size_t i = 0; i < m_items.size(); i++) {
        if (!has_icon && m_items[i]->has_icon()) {
            has_icon = true;
            w += 30;
        }
        if (!has_submenu && m_items[i]->has_submenu()) {
            has_submenu = true;
            w += 30;
        }
        m_text->setString(m_items[i]->text());
        auto sz = m_text->getLocalBounds();
        if (sz.size.x > w) {
            w = sz.size.x * 2;
        }
    }

    first_time = false;

    this->coordinates(this->x(), this->y(), w + 4, h);
}

void PopupMenu::handle_mouse_left_pressed(int x, int y) {
    if (!m_text) return;
    select_item(y);

}

void PopupMenu::handle_mouse_left_released(int x, int y) {
    if (!m_text) return;
    select_item(y);

}

void PopupMenu::handle_mouse_moved(int x, int y) {
    if (!m_text) return;
    select_item(y);
}

void PopupMenu::select_item(int y_coord) {
    if (m_items.empty()) return;
    m_selected_index = (y_coord / 30);
    if (m_selected_index >= m_items.size()) {
        m_selected_index = m_items.size() - 1;
    }
    if (m_current_popupmenu != m_items[m_selected_index]->submenu()) {
        if (m_current_popupmenu) {
            m_current_popupmenu->float_off();
        }
        m_current_popupmenu = m_items[m_selected_index]->submenu();
        if (m_current_popupmenu) {
            m_current_popupmenu->popup(abs_x() + abs_w(), abs_y() + m_selected_index * 30, this);
        }
    }
}

void PopupMenu::handle_click() {
    if (m_items.empty()) return;
    if (m_selected_index >= m_items.size()) {
        return;
    }

    if (m_items[m_selected_index]->has_submenu()) {
        m_current_popupmenu->popup(abs_x() + abs_w(), abs_y() + m_selected_index * 30, this);
        return;
    } 
    
    if (m_items[m_selected_index]->onclick()) {
        m_items[m_selected_index]->onclick()(this);
    }
    
    float_off();

    if (m_parent) {
        m_parent->float_off();
        m_parent = NULL;
    }
}

void PopupMenu::popup(int x, int y, PopupMenu *parent) {
    m_parent = parent;
    popup(x, y);
}

void PopupMenu::popup(int x, int y) {
    if (!m_text) return;
    
    int new_charsize = theme::menu_text_size() * abs_scale();
    if (new_charsize < 1) {
        new_charsize = 1;
    }

    if (m_text->getCharacterSize() != new_charsize) {
        m_text->setCharacterSize(new_charsize);
        update_text_min_y();
    }

    m_selected_index = (size_t) -1;
    m_current_popupmenu.reset();
    fix_coordinates(x, y);
    coordinates(x, y, w(), h());
    handle_parent_resized();
    Component::float_on();
}

void PopupMenu::fix_coordinates(int &x, int &y) {
    if (x + w() > window()->w()) {
        x = window()->w() - w();
    }
    if (y + h() > window()->h()) {
        y = window()->h() - h();
    }
    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
}

void PopupMenu::update_text_min_y() {
    if (m_text) {
        m_text_min_y = compute_text_min_y(m_text.get());
    }
}

void PopupMenu::paint(sf::RenderTarget *render_target) {
    if (!m_text) return;
   
    auto scale = abs_scale();
    auto x = this->abs_x();
    auto y = this->abs_y();
    auto w = this->abs_w();
    auto h = this->abs_h();

    Drawing dw(Drawing::drawing_flat_box);
    dw.size(w, h);
    dw.position(x, y);
    dw.color(theme::menu_fill_color());
    dw.outline_color(theme::menu_fill_color());
    dw.draw(render_target);
    dw.size(w, 30);

    Drawing ar(Drawing::drawing_arrow, Drawing::direction_right);
    ar.color(theme::menu_fill_color());
    ar.outline_color(theme::menu_fill_color());
    ar.size(14, 14);

    IconTextureBase *icons = load_icons_texture();;
    std::pair<int, int> icon_coord(0, 0);
    std::pair<int, int> icon_size(0, 0);

    bool has_icon = false;
    for (size_t i = 0; i < m_items.size(); i++) {
        if (m_items[i]->icon_type() != img_none) {
            has_icon = true;
            break;
        }
    }
  
    for (size_t i = 0; i < m_items.size(); i++) {
        if (m_selected_index == i) {
            dw.color(theme::menu_selected_fill_color());
            dw.outline_color(theme::menu_selected_fill_color());
            m_text->setFillColor(sf::Color(theme::menu_selected_text_color()));
            m_text->setOutlineColor(sf::Color(theme::menu_selected_text_color()));
            ar.color(theme::menu_selected_text_color());
            ar.outline_color(theme::menu_selected_text_color());
        } else {
            dw.color(theme::menu_fill_color());
            dw.outline_color(theme::menu_fill_color());
            m_text->setFillColor(sf::Color(theme::menu_text_color()));
            m_text->setOutlineColor(sf::Color(theme::menu_text_color()));
            ar.color(theme::menu_text_color());
            ar.outline_color(theme::menu_text_color());
        }
        dw.position(x, y);
        dw.draw(render_target);
        m_text->setString(m_items[i]->text());
        m_text->setPosition({(float)x + ( has_icon ? 30 : 2) , (float)y});

        render_target->draw(*m_text.get());
        if (m_items[i]->icon_type() != img_none) {
            icon_coord = icons->get_coords(m_items[i]->icon_type());
            icon_size = icons->get_size(m_items[i]->icon_type());
            sf::Sprite icon(*static_cast<sf::Texture *>(icons->sfml_texture()), {{icon_coord.first, icon_coord.second}, { icon_size.first, icon_size.second}});
            icon.setPosition({x + 3, y + 3});
            render_target->draw(icon);
        }
        if (m_items[i]->has_submenu()) {
            ar.position(x + w - 16, y + 8);
            ar.draw(render_target);
        }
        y += 30;
    }

    auto color = sf::Color(theme::menu_outline_color());

    x = this->abs_x() + 1;
    y = this->abs_y() + 1;
    w = this->abs_w() - 2;
    h = this->abs_h() - 2;

    sf::Vertex line[] = {
            {sf::Vector2f(x, y), color},
            {sf::Vector2f(x + w, y), color},
            {sf::Vector2f(x + w, y + h), color},
            {sf::Vector2f(x, y + h), color},
            {sf::Vector2f(x, y), color}
    };
    render_target->draw(line, 5, sf::PrimitiveType::LineStrip);
}

bool PopupMenu::clickable() {
    return true;
}

component_cursor_t PopupMenu::cursor() {
    return cursor_hand;
}

} // namespace dfe_ui
