#pragma once

#include <string>

#include "simple-ui/events.h"
#include "simple-ui/component.h"
#include "simple-ui/icons.h"

namespace sf {
    class Text;
}

namespace dfe_ui
{

    class PopupMenu;
    class MenuItem {
        public:
            MenuItem(PopupMenu *menu, const std::wstring & text, icon_type_t icon_type);
            virtual ~MenuItem();
            void onclick(component_event_t cb);
            component_event_t onclick();
            std::shared_ptr<PopupMenu> submenu();
            bool has_icon();
            bool has_submenu();
            void submenu(std::shared_ptr<PopupMenu> value);
            const std::wstring & text();
            icon_type_t icon_type();

        private:
            PopupMenu*                   m_menu;
            std::wstring                 m_text;
            std::shared_ptr<PopupMenu>   m_submenu;
            component_event_t       m_onclick;
            icon_type_t             m_icon_type;
    };

    class PopupMenu : public Component {
        public:
            PopupMenu(Window *window);
            virtual ~PopupMenu();
            MenuItem *add(const std::wstring & text, icon_type_t icon_type=img_none);
            void popup(int x, int y);

        protected:
            void handle_mouse_left_pressed(int x, int y) override;
            void handle_mouse_left_released(int x, int y) override;
            void handle_mouse_moved(int x, int y) override;
            void handle_parent_resized() override;
            void paint(sf::RenderTarget *render_target) override;
            bool clickable() override;
            component_cursor_t cursor() override;
            void handle_click() override;

        private:
            void popup(int x, int y, PopupMenu *parent);
            void select_item(int y_coord);
            void fix_coordinates(int &x, int &y);
            void update_text_min_y();

        private:    
            PopupMenu *m_parent = NULL;
            std::shared_ptr<PopupMenu> m_current_popupmenu;
            std::shared_ptr<sf::Text> m_text;
            std::vector<std::shared_ptr<MenuItem> > m_items;
            int m_text_min_y = 0;
            size_t m_selected_index = (size_t)-1;

            
    };
} // namespace dfe_ui
