#include "State_Battle.h"


State_Battle::State_Battle(StateManager* l_stateManager) : BaseState(l_stateManager)
{ }


void State_Battle::OnCreate() {

  EventManager* mngr = m_stateMgr->GetContext()->m_eventManager;
  
  mngr->AddCallback(StateType::Battle, "Space", &State_Battle::Callback_Space, this);

  mngr->AddCallback(StateType::Battle, "Up", &State_Battle::Callback_Up, this);

  mngr->AddCallback(StateType::Battle, "Down", &State_Battle::Callback_Down, this);

  mngr->AddCallback(StateType::Battle, "Left", &State_Battle::Callback_Left, this);

  mngr->AddCallback(StateType::Battle, "Right", &State_Battle::Callback_Right, this);
  
  mngr->AddCallback(StateType::Battle, "Key_P", &State_Battle::Callback_Pause, this);

  mngr->AddCallback(StateType::Battle, "Key_Escape", &State_Battle::Callback_Esc, this);
}


void State_Battle::OnDestroy() {
  printEntityStatistics(__FUNCTION__);
  //getchar();
}


void State_Battle::Activate() {
  using namespace std;
  //printEntityStatistics(__FUNCTION__);
  m_battle.reset(new BattleCity);
  m_panel.reset(new InfoPanel({16,16}));
  // загрузка карты и инициализация
  m_battle->Start(m_stateMgr->GetContext()->CurrentStageFile());
  // выравнивание игровой области
  sf::Vector2f size = { m_battle->getInfo().map_width,
                      m_battle->getInfo().map_height };
  m_bkg.setSize(size);
  m_bkg.setFillColor(sf::Color::Black);
  Align();
  m_battle->setFieldOrigin(m_bkg.getPosition());
}


void State_Battle::Deactivate() {
  m_battle->Stop();
  m_battle.reset(nullptr);
  m_panel.reset(nullptr);

  m_stateMgr->GetContext()->NextStageFile();
  //printEntityStatistics(__FUNCTION__);
  //getchar();
}


void State_Battle::Update(const sf::Time& l_time) {
  m_battle->Update(l_time);
  m_panel->UpdateInfo(m_battle->getInfo().enemies, m_battle->getInfo().lives,
                      m_stateMgr->GetContext()->getStageNum() + 1);

  if (m_battle->getInfo().geme_over_flag) {
    m_stateMgr->SwitchTo(StateType::BattleReport);
  }
}


void State_Battle::Draw() {
  sf::RenderWindow* window = 
    m_stateMgr->GetContext()->m_wind->GetRenderWindow();

  window->clear(sf::Color(50,175,175));
  window->draw(m_bkg);
  m_battle->Draw(*window);
  m_panel->Draw(*window);
}



// control input Key callback
void State_Battle::Callback_Up(EventDetails* l_details) {
  m_battle->Player_up();
}


void State_Battle::Callback_Down(EventDetails* l_details) {
  m_battle->Player_down();
}


void State_Battle::Callback_Left(EventDetails* l_details) {
  m_battle->Player_left();
}


void State_Battle::Callback_Right(EventDetails* l_details) {
  m_battle->Player_right();
}


void State_Battle::Callback_Space(EventDetails* l_details) {
  m_battle->Player_fire();
}


void State_Battle::Callback_Pause(EventDetails* l_details) {

}


void State_Battle::Callback_Esc(EventDetails* l_details) {
  m_stateMgr->SwitchTo(StateType::TanksMenu);
}
//



void State_Battle::Align() {
  float w = m_bkg.getSize().x + m_panel->getBounds().width;
  float h = m_bkg.getSize().y;
  sf::RenderWindow* window =
    m_stateMgr->GetContext()->m_wind->GetRenderWindow();
  auto m_wind = window->getSize();
  m_bkg.setPosition( (m_wind.x - w) / 2, (m_wind.y - h) / 2);
  
  m_panel->setPosition(m_bkg.getGlobalBounds().left +
    m_bkg.getGlobalBounds().width + 16,
    (m_wind.y - h) / 2);
}




// 
InfoPanel::InfoPanel(sf::Vector2f cell) : m_cell_sz(cell), m_clr(sf::Color(128,128,128)),
                                          m_txt_clr(sf::Color::Black) {
  m_enemy_cnt = new SpriteTable({ 10, 2 }, cell);
  m_live_sp =  new SpriteTable({ 2, 2 }, cell);    // таблица жизней
  m_stage_sp = new SpriteTable({ 3, 2 }, cell);    // таблица уровней  
  
  m_live_cnt = new VisualPanel({ 2, 2}, cell);    // счетчик жизней
  m_stage =  new VisualPanel({ 3, 2}, cell);      // номер уровня
  m_main = new VisualPanel({ 26, 2 }, cell, cell.x);

  dsp_lives = new CharDisplay({ 2, 2 }, cell);
  dsp_stage =  new CharDisplay({ 1, 2}, cell);

  m_enemy_cnt->setColor(sf::Color::Transparent);
  m_live_sp->setColor(sf::Color::Transparent);
  m_stage_sp->setColor(sf::Color::Transparent);
  m_live_cnt->setColor(sf::Color::Transparent);
  m_stage->setColor(sf::Color::Transparent);
  m_main->setColor(m_clr);

  dsp_lives->setBckgColor(sf::Color::Transparent);
  dsp_stage->setBckgColor(sf::Color::Transparent);
  dsp_lives->setColor(m_txt_clr);
  dsp_stage->setColor(m_txt_clr);
  
  // init
  // таблица жизней
  m_live_cnt->Place(dsp_lives, { 0,0 });

  m_sprites.emplace_back(SpriteManager::Get("info_player_icon"));
  m_live_sp->Place(&m_sprites.back(), {0,0});
  m_live_cnt->Place(m_live_sp, {1,0});

  // номер уровня
  m_sprites.emplace_back(SpriteManager::Get("info_flag"));
  m_stage_sp->Place(&m_sprites.back());
  
  m_stage->Place(m_stage_sp);
  m_stage->Place(dsp_stage, {2,0});
  
  m_main->Place(m_enemy_cnt);
  m_main->Place(m_live_cnt, {14, 0});
  m_main->Place(m_stage, {20, 0});

  UpdateInfo(13,4,99);
}


InfoPanel::~InfoPanel() {
  delete m_enemy_cnt;    // счетчик вражеских танков
  delete m_live_sp;      // таблица жизней
  delete m_stage_sp;    // таблица уровней  

  delete m_live_cnt;    // счетчик жизней
  delete m_stage;      // номер уровня
  delete m_main;

  delete dsp_lives;
  delete dsp_stage;
}


void InfoPanel::UpdateInfo(int enemies, int lives, int stage) {
  using namespace std;
  
  if (enemies == i_enemies && lives == i_lives && stage == i_stage) {
    return;
  }
  i_enemies = enemies; i_lives = lives; i_stage = stage;

  m_enemy_cnt->Clear();
  dsp_lives->Clear();
  dsp_stage->Clear();
  // счетчик танков
  for (int i = 0; i < enemies; ++i) {
    m_sprites.emplace_back(SpriteManager::Get("info_enemy_icon"));
    m_enemy_cnt -> Place( &m_sprites.back() );
  }
  //cout << "\n\n Update info panel: \n\n";
  // таблица жизней
  *dsp_lives << "ip\n ";
  *dsp_lives << to_string(lives);
  // номер уровня
  ostringstream os;
  os << setw(2) << stage;
  *dsp_stage << os.str();
}


void InfoPanel::Draw(sf::RenderWindow& window) {
  m_main->Draw(window);
}

void InfoPanel::setPosition(float x, float y) {
  m_main->setPosition(x, y);
}


sf::FloatRect InfoPanel::getBounds() {
  return m_main->getBounds();
}
