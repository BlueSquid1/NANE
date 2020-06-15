#include "SubMenu.h"

SubMenu::SubMenu(SDL_Renderer* gRenderer, const std::string& name, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour, FontType font)
: ToggleButton(gRenderer, name, NULL, false, forgroundColour, inactiveBackgroundColour, activeBackgroundColour, font)
{
}

void SubMenu::AddButton(const ToggleButton& button)
{
    this->menuItems.push_back(button);
}

void SubMenu::SetDimensions(int posX, int posY, int width, int height)
{
    ToggleButton::SetDimensions(posX, posY, width, height);

    int itemHeightPadding = 10;
    int itemWidthPadding = 10;

    //calculate dropdown size
    int subMenuHeight = 0;
    int subMenuWidth = 0;
    for(const ToggleButton& item : this->menuItems)
    {
        const SDL_Rect& itemMinSize = item.CalculateMinSize();
        subMenuHeight += itemMinSize.h + itemHeightPadding;
        subMenuWidth = std::max(itemMinSize.w + itemWidthPadding, subMenuWidth);
    }
    
    //update dropdown menu
    this->dropdownMenuDimensions.x = posX;
    this->dropdownMenuDimensions.y = posY + height;
    this->dropdownMenuDimensions.w = subMenuWidth;
    this->dropdownMenuDimensions.h = subMenuHeight;

    //reposition submenu items
    int itemPosX = this->dropdownMenuDimensions.x;
    int itemPosY = this->dropdownMenuDimensions.y;
    for(ToggleButton& item : this->menuItems)
    {
        const SDL_Rect& itemMinSize = item.CalculateMinSize();
        int itemHeight = itemMinSize.h + itemHeightPadding;
        item.SetDimensions(itemPosX, itemPosY, subMenuWidth, itemHeight);
        itemPosY += itemHeight;
    }
}

void SubMenu::HandleEvent(const SDL_Event& e)
{
    //handle buttons if they are visible
    if(this->GetIsPressed())
    {
        for(ToggleButton& menuItem : this->menuItems)
        {
            menuItem.HandleEvent(e);
        }
    }

    //enable or disable submenu
    ToggleButton::HandleEvent(e);
}

void SubMenu::Display()
{
    //display text in bar
    ToggleButton::Display();

    //display background panel
    if(this->GetIsPressed())
    {
        //dropdown panel
        SDL_Color panelColour;
        panelColour.r = 0x50;
        panelColour.g = 0x50;
        panelColour.b = 0x50;
        panelColour.a = 0xFF;
        SDL_SetRenderDrawColor( gRenderer, panelColour.r, panelColour.g, panelColour.b, panelColour.a );
        SDL_RenderFillRect(this->gRenderer, &this->dropdownMenuDimensions);

        //display itens on drop down
        for(ToggleButton& item : this->menuItems)
        {
            item.Display();
        }
    }
}