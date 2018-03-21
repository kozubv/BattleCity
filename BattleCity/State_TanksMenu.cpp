#include "State_TanksMenu.h"
#include "SupportTools.h"

State_TanksMenu::State_TanksMenu(StateManager* l_stateManager) 
    : BaseState(l_stateManager) {
}


//State_TanksMenu::~State_TanksMenu()
//{  }


void State_TanksMenu::OnCreate() {
  using namespace std;
  using namespace sf;
  Vector2u  w_size = m_stateMgr->GetContext()->m_wind->GetWindowSize();
  // ставим посередине главную надпись
  m_label = SpriteManager::Get("main_label");
  toCenter(m_label);
  m_label.move(w_size.x / 2, m_label.getLocalBounds().height);

  // пункты меню
  menu_item_1_ = SpriteManager::Get("1_player");
  menu_item_1_.setPosition( w_size.x / 2, w_size.y / 2);

  menu_item_2_ = SpriteManager::Get("exit");
  menu_item_2_.setPosition(w_size.x / 2, w_size.y / 2 + 50);

  // курсор пунктов меню
  m_cursor = SpriteManager::Get("cursor");
  m_cursor.setPosition(w_size.x / 2.5, menu_item_1_.getGlobalBounds().top);

  //EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
  EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
  evMgr->AddCallback(StateType::TanksMenu, "Up", &State_TanksMenu::CallbackUp, this);
  evMgr->AddCallback(StateType::TanksMenu, "Down", &State_TanksMenu::CallbackDown, this);
  evMgr->AddCallback(StateType::TanksMenu, "Return", &State_TanksMenu::CallbackReturn, this);
}


void State_TanksMenu::OnDestroy() {
}

void State_TanksMenu::Activate() {
}

void State_TanksMenu::Deactivate() {
}

void State_TanksMenu::Update(const sf::Time& l_time) {
  //std::cout << " down " << std::endl;
}

void State_TanksMenu::Draw() {
  sf::RenderWindow* window = m_stateMgr->GetContext()->m_wind->GetRenderWindow();
  window->draw(m_label);
  window->draw(menu_item_1_);
  window->draw(menu_item_2_);
  window->draw(m_cursor);
}

void State_TanksMenu::CallbackUp(EventDetails* l_details) {
  if (menu_choice == EXIT) {
    menu_choice = PLAY;
    m_cursor.setPosition(m_cursor.getPosition().x,
              menu_item_1_.getGlobalBounds().top);
  }
  return;
}


void State_TanksMenu::CallbackDown(EventDetails* l_details) {
  if (menu_choice == PLAY) {
    menu_choice = EXIT;
    m_cursor.setPosition(m_cursor.getPosition().x,
      menu_item_2_.getGlobalBounds().top);
  }
  return;
}


void State_TanksMenu::CallbackReturn(EventDetails* l_details) {
  std::cout << " insert " << std::endl; 
  switch (menu_choice) {
  case PLAY:
    m_stateMgr->SwitchTo(StateType::LevelSelection);
    std::cout << "LevelSelection" << std::endl;

    break;
  case EXIT:
    //m_stateMgr->SwitchTo(StateType::MainMenu);
    m_stateMgr->GetContext()->m_wind->Close();
    std::cout << "LevelSelection" << std::endl;
    break;
  }
  return;
}


void test_State_TanksMenu() {
}