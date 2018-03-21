#include "GameScenario.h"


GameScenario::GameScenario(GameInfo& init) : m_info(init)
{ }

void GameScenario::Update(const sf::Time& time) {
  m_time_last = time.asSeconds();

  if (s_free_ports.empty()) return;
  CreateTank(time.asSeconds());
}


void GameScenario::Start( ) {
  using namespace std;
  
  auto main_flag = Bonus::factory::Create('f', 0);
  main_flag->setPosition(m_info.flag_pos);
  auto flag_callback = [this]() {this->CommandGameOver(); };
  main_flag->setDestructionCallback(flag_callback);

  CreatePlayer();

  int cnt = 0;
  for (auto item : m_info.enemy_ports) {
    s_free_ports.push_back(cnt);
    ++cnt;
  }  
  
  // заполняем команды действий для бонусов
  m_bonus_effect_map.emplace(m_bonus_id[0], [this]() { this->ApplyBonusClock(); });
  m_bonus_effect_map.emplace(m_bonus_id[1], [this]() { this->ApplyBonusBomb(); });
  m_bonus_effect_map.emplace(m_bonus_id[2], [this]() { this->ApplyBonusStar(); });
  m_bonus_effect_map.emplace(m_bonus_id[3], [this]() { this->ApplyBonusTank(); });
  m_bonus_effect_map.emplace(m_bonus_id[4], [this]() { this->ApplyBonusHelmet(); });
  m_bonus_effect_map.emplace(m_bonus_id[5], [](   ) { std::cout << "Bonus No define." << std::endl; });

  printEntityStatistics(__FUNCTION__);
}


void GameScenario::Stop() {
  s_free_ports.clear();
  m_bonus_effect_map.clear();
  m_time_last = 0;
  printEntityStatistics(__FUNCTION__);
}


bool GameScenario::isGameOver() const {
  return false;
}


void GameScenario::CreatePlayer( ) {
  if (!m_info.lives--) {
    CommandGameOver();
    // к удаленному после уничтожения player объекту обращаются внешние методы.
    // их ссылка недействительна сейчас.
    m_player = Tank::factory::Create('e', m_time_last);
  }
  else {
    m_player = Tank::factory::Create('e', m_time_last);
    m_player->setPosition(m_info.player_pos);
    auto callback = [this]()
    {
      this->CreatePlayer();
    };
    m_player->setDestructionCallback(callback);
  }
}


void GameScenario::CreateTank(float time) {
  if (!--m_info.enemies) return;

  try {
    // связываем танк с номером порта
    int port_id = s_free_ports.back();
    s_free_ports.pop_back();
    // присваевается тип танка - по номеру порта
    char enemy_type = m_enemy_id.at(port_id % m_enemy_id.size());
    auto tmp = Tank::factory::Create(enemy_type, time);

    tmp->setPosition(m_info.enemy_ports[port_id]);

    auto callback = [this, port_id]() {
      this->EnemyDestructionEvents(port_id);
    };
    tmp->setDestructionCallback(callback);
    tmp->setAutoPilot();
  }
  catch (...) {
    std::cout << "GameScenario::CreateTank( t ) - No free ports " << std::endl;
    getchar();
    terminate();
  }
}


void GameScenario::CreateBonus(char type) {
  using namespace std;
  float x = fmod( rand(), m_info.map_width / 2.0 ) + m_info.map_width / 4.0;
  float y = fmod( rand(), m_info.map_height/ 2.0 ) + m_info.map_height/ 4.0;
  auto tmp = Bonus::factory::Create( type, 0);
  tmp->setPosition({ x, y });
  
  try {
    tmp->setDestructionCallback(m_bonus_effect_map.at(type));
  }
  catch (...) { }
}


void GameScenario::EnemyDestructionEvents(int id) {
  using namespace std;
  cout << "tank N_ : " << id << " is dead," << endl;
  s_free_ports.push_back(id);
  int bonus_type_id = id % m_bonus_id.size();
  CreateBonus(m_bonus_id.at(bonus_type_id));
}


void GameScenario::ApplyBonusClock() {
  std::cout << "ApplyBonusClock()" << std::endl;
  auto iter = Tank::factory::s_current_set.begin();
  ++iter;
  for ( ; iter != Tank::factory::s_current_set.end(); ++iter) {
    iter->setStateFreeze();
  }
}

void GameScenario::ApplyBonusBomb() {
  auto iter = Tank::factory::s_current_set.begin();
  ++iter;
  for (; iter != Tank::factory::s_current_set.end(); ++iter) {
    iter->setStateDestruction();
  }
}

void GameScenario::ApplyBonusStar() {
  std::cout << "ApplyBonusStar()" << std::endl;
  printEntityStatistics(__FUNCTION__);
  //getchar();
}

void GameScenario::ApplyBonusTank() {
  std::cout << "ApplyBonusTank()" << std::endl;
}

void GameScenario::ApplyBonusHelmet() {
  std::cout << "ApplyBonusHelmet()" << std::endl;
  m_player->setStateImmortal();
}

void GameScenario::CommandGameOver() {
  m_info.geme_over_flag = true;
  sf::sleep(sf::seconds(1.5));
}






