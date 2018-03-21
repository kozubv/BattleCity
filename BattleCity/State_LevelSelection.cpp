#include "State_LevelSelection.h"


State_LevelSelection::State_LevelSelection(StateManager* l_stateManager) : BaseState(l_stateManager) {
  m_stateMgr->GetContext()->LoadStageList(SharedContext::getFilePath("StageList"));
  auto win_xy = l_stateManager->GetContext()->m_wind->GetWindowSize();
  m_display = new LevelDisplay(win_xy);
}


State_LevelSelection::~State_LevelSelection() {
  delete m_display;
}


void State_LevelSelection::OnCreate() {
  EventManager* mngr = m_stateMgr->GetContext()->m_eventManager;
  mngr->AddCallback(StateType::LevelSelection, "press_Up", &State_LevelSelection::ScrollUp, this);
  mngr->AddCallback(StateType::LevelSelection, "press_Down", &State_LevelSelection::ScrollDown, this);
  mngr->AddCallback(StateType::LevelSelection, "Return", &State_LevelSelection::CallbackReturn, this);
}

void State_LevelSelection::OnDestroy() {
}

void State_LevelSelection::Activate() {
  const std::string& name = m_stateMgr->GetContext()->CurrentStageName();
  m_display->Update(name);
}

void State_LevelSelection::Deactivate() {
}

void  State_LevelSelection::Update(const sf::Time& l_time) {
}

void State_LevelSelection::Draw() {
  sf::RenderWindow* window = m_stateMgr->GetContext()->m_wind->GetRenderWindow();
  window->clear(sf::Color(100,128,50));
  m_display->Draw(*window);
}

void State_LevelSelection::ScrollUp(EventDetails* l_details) {
  const std::string& name = 
    m_stateMgr->GetContext()->NextStageName();
  m_display->Update(name);
}

void State_LevelSelection::ScrollDown(EventDetails* l_details) {
  const std::string& name =
    m_stateMgr->GetContext()->PreviousStageName();
  m_display->Update(name);
}

void State_LevelSelection::CallbackReturn(EventDetails* l_details) {
  m_stateMgr->SwitchTo(StateType::Battle);
}


//
//const sf::Vector2f LevelDisplay::s_cell_sz = {16,16};

LevelDisplay::LevelDisplay(sf::Vector2u wind, unsigned len) : m_display({1, len}, s_cell_sz) {
  m_display.setPosition((wind.x - m_display.getBoundsSize().x) / 2,
                        (wind.y - m_display.getBoundsSize().y) / 2);
  m_display.setBckgColor(sf::Color::Transparent);
  m_display.setColor(sf::Color(50, 20, 20));
  Update("stage ");
}


void LevelDisplay::Update(const std::string& line) {
  m_display.Clear();
  m_display << line;
}


void LevelDisplay::Draw(sf::RenderWindow & window) {
  m_display.Draw(window);
}


sf::FloatRect LevelDisplay::getBounds() const {
  return m_display.getBounds();
}


void LevelDisplay::setPosition(float x, float y) {
  
}


sf::Vector2f LevelDisplay::getFontSize() {
  return s_cell_sz;
}
