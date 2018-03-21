#include "GameBattleCity.h"
#include "SharedContext.h"

BattleCity::BattleCity() : m_scenario(info_) {
  Tank::factory::LoadCollection(SharedContext::getFilePath("Tanks"));
  Bullet::factory::LoadCollection(SharedContext::getFilePath("Bullet"));
  Barrier::factory::LoadCollection(SharedContext::getFilePath("Barrier"));
  Bonus::factory::LoadCollection(SharedContext::getFilePath("Bonus"));
  SpriteSheetInfo::LoadSpriteSheetInfo(SharedContext::getFilePath("SpriteSheet"));
  
  for (auto item : SpriteSheetInfo::s_collection) {
    std::cout << item.second << std::endl;
  }
}


void BattleCity::Update(const sf::Time& time) {
  if (m_scenario.isGameOver()) {
    info_.geme_over_flag = true;
  }
  m_entity_manager.Update(m_clock.getElapsedTime());
  m_entity_manager.Interaction();

  m_scenario.Update(m_clock.getElapsedTime());
}


void BattleCity::Draw(sf::RenderWindow& wind) {
  m_entity_manager.Draw(wind);
}


void BattleCity::Start(const std::string& map_file_name) {
  printEntityStatistics(__FUNCTION__);
  info_ = GameInfo();
  LoadMapScheme(map_file_name);
  CreateMapBorders();
  m_scenario.Start( );
  m_clock.restart();
}


void BattleCity::Stop() { 
  m_scenario.Stop();

  GameEntity::g_entity_list.clear();
  Tank::factory::s_current_set.clear();
  Bullet::factory::s_current_set.clear();
  Barrier::factory::s_current_set.clear();
  Bonus::factory::s_current_set.clear();

  Tank::factory::s_collection.clear();
  Bullet::factory::s_collection.clear();
  Barrier::factory::s_collection.clear();
  Bonus::factory::s_collection.clear();

  printEntityStatistics(__FUNCTION__);
}

void BattleCity::Player_up()    { m_scenario.ControlledUnit().Forward(); }

void BattleCity::Player_down()  { m_scenario.ControlledUnit().Back(); }

void BattleCity::Player_left()  { m_scenario.ControlledUnit().Left(); }

void BattleCity::Player_right() { m_scenario.ControlledUnit().Ridht(); }

void BattleCity::Player_fire()  { m_scenario.ControlledUnit().Fire(); }

void BattleCity::LoadMapScheme(const std::string& file) {
  using namespace std;
  ifstream fin(file);
  if (!fin.is_open()) {
    cout << "No find file: " << file << endl;
    getchar();
    throw invalid_argument("can't open map file");
  }
  vector<string> m_map_scheme;
  string line;
  while (getline(fin, line)) {
    if (line[0] == '#') continue;  // comments
    m_map_scheme.push_back(std::move(line));
    line.clear();
  }
  ConvertMapScheme(m_map_scheme);
  return;
}


void BattleCity::ConvertMapScheme(const std::vector<std::string>& scheme) {
  using namespace std;
  sf::Vector2f block_sz =
    Barrier::factory::s_collection.begin()->second.size;

  // find max column num line
  auto max_str = *max_element(scheme.cbegin(), scheme.cend(), 
    [](const string& a, const string& b) {
    return   a.size() < b.size(); });

  info_.map_width = max_str.size()*block_sz.y;
  info_.map_height = scheme.size()*block_sz.x;

  // считываем содержимое схемы карты и интерпретируем в игровые элементы
  sf::Vector2f cursor = {0,0};

  unordered_map<char, function<void(sf::Vector2f)>> commands;
  
  GameInfo* tmp = &info_;
  commands.emplace('F', [tmp](sf::Vector2f pos) {  // create flag
    tmp->flag_pos = pos; 
  });
  
  commands.emplace('P', [tmp](sf::Vector2f pos) { // create player
    tmp->player_pos = pos; 
  });
  
  commands.emplace('E', [tmp](sf::Vector2f pos) { // create enemy position
    tmp->enemy_ports.push_back(pos); 
  });
  
  commands.emplace(' ', [ ](sf::Vector2f pos) { // space symbol ignor
    ;}); 
  
  for (auto& line : scheme) {
    cursor.x = 0;
    for (char item : line) {
      if (commands.find(item) == commands.end()) {
        // create barrier element
        auto tmp = Barrier::factory::Create(item, 0);
        sf::Vector2f pos = { (cursor.x + 0.5f)*block_sz.x,
                (cursor.y + 0.5f)*block_sz.y };
        tmp->setPosition(pos);
      } else {
        // create Units
        try {
          commands.at(item)({ cursor.x*block_sz.x,
                  cursor.y*block_sz.y });
        } catch (...) {
          cout << __FUNCTION__ << " :  "<< endl;
          cout << "unknown symbol in map scheme : " 
             << item << endl;
        }
      }
      ++cursor.x;
    }
    ++cursor.y;
  }

  cout << "\n\nmap load: " << endl;
  //getchar();
}

void BattleCity::setFieldOrigin(sf::Vector2f pos) {
  m_field_origin = pos;
  //Animation::s_local = pos;
  AnimBase::s_local = pos;
  GameEntity::m_local = pos;
}

void BattleCity::CreateMapBorders() {
  float w_brd = 50;
  const float& m_w = info_.map_width;
  const float& m_h = info_.map_height;

  BarrierTypeInfo hor_border;
  hor_border.name = "unknown";
  hor_border.type = 'H';
  hor_border.health = 1000;
  hor_border.obstruct_Z_eq_0 = 0;
  hor_border.obstruct_Z_greater_0 = 0;
  hor_border.size = { m_w, w_brd };
  Barrier::factory::s_collection.emplace(hor_border.type, hor_border);
  
  BarrierTypeInfo vert_border(hor_border);
  vert_border.type = 'V';
  vert_border.size = {w_brd, m_h};
  Barrier::factory::s_collection.emplace(vert_border.type, vert_border);

  auto left_brd  = Barrier::factory::Create('V', 0);
  auto right_brd  = Barrier::factory::Create('V', 0);
  auto top_brd  = Barrier::factory::Create('H', 0);
  auto down_brd  = Barrier::factory::Create('H', 0);

  left_brd->setPosition({ - w_brd / 2, m_h / 2 });
  right_brd->setPosition({ (m_w + w_brd / 2), m_h / 2 });
  top_brd->setPosition({ m_w / 2, - w_brd / 2 });
  down_brd->setPosition({ m_w / 2 , (m_h + w_brd / 2)});
}




//    tests
void test_game_battle_city_first();



// main
void test_game_battle_city()
{
  test_game_battle_city_first();

}



// definition 
void test_game_battle_city_first()
{
  std::cout << "hello Battle city" << std::endl;
  getchar();
}


