#include "Units.h"

std::istream& operator>>(std::istream& is, TankTypeInfo& item) {
  return is >> item.name >> item.type >> item.size.x >> item.size.y
            >> item.health >> item.speed >> item.bullet_type;
}

std::ostream& operator<<(std::ostream& is, const TankTypeInfo& item) {
  std::string dlm = "\t";
  return is << item.name << dlm << item.type << dlm
            << item.size.x << dlm << item.size.y << dlm
            << item.health << dlm << item.speed << dlm << item.bullet_type;
}


unsigned Tank::id_cnt = 0;
Tank::Tank(char type, float time) : GameEntity( ), m_type(type), 
                                    m_id(id_cnt++), m_time_last(time) {
  m_state = BIRTH;
  m_anim_list.InsertBack(unique_anim<AnimSingle>(time, "Flare"));
  m_callback = []() {};
}


void Tank::Update(const sf::Time& time) {
  m_elepsed_time = time.asSeconds() - m_time_last;
  m_time_last = time.asSeconds();
  if (m_move_delay) --m_move_delay;
  // обновление автопилота
  if (m_Driver) m_Driver->ApplyControl(m_time_last);
  // проверка состояний
  switch (m_state)
  {
  case BIRTH:
    if (m_anim_list.IsCompleted()) setStateActiveNormal();
    m_anim_list.setPosition(m_pos);
    break;
  case ACTIVE:
    UpdateActiveState( );
    UpdatePosition( );
    break;
  case DESTRUC :
    if (m_anim_list.IsCompleted()) m_destroyed = true;
    break;
  default:
    ;
  }
  m_anim_list.Update(time.asSeconds());
}


void Tank::UpdateActiveState( ) {
  switch (m_tank_state)
  {
  case NORMAL:
    //std::cout << "normal : " << std::endl;
    break;
  case IMMORTAL:
    if (m_anim_list.IsCompleted("Shine")) setStateActiveNormal();
    //std::cout << "immortal : " << std::endl;
    break;
  case FREEZE:
    m_move_flag = false;
    if (m_anim_list.IsCompleted()) setStateActiveNormal();
    //std::cout << "freeze : " << std::endl;
    break;
  default:
    break;
  }
}


void Tank::Draw(sf::RenderWindow& window) {
  m_anim_list.Draw(window);
}

void Tank::Interaction(GameEntity&) {

}

void Tank::setPosition(sf::Vector2f pos) {
  m_pos_last = m_pos;
  m_pos = pos;
  m_anim_list.setPosition(pos);
}

void Tank::setOrientation(direction dir) {
  m_dir = dir;
  m_anim_list.setOrientation(dir);
}

sf::Vector2f Tank::getPosition() const {
  return m_pos;
}

sf::Vector2f Tank::getSize() const {
  return factory::s_collection.at(m_type).size;
}


//    TANK CALLBACK
void Tank::Left() {
  if (m_state != ACTIVE || m_tank_state == FREEZE) return;
  
  if (m_dir != LEFT) setOrientation(LEFT);
  m_move_flag = true;
}

void Tank::Ridht() {
  if (m_state != ACTIVE || m_tank_state == FREEZE) return;

  if (m_dir != RIGHT) setOrientation(RIGHT);
  m_move_flag = true;
}

void Tank::Forward() {
  if (m_state != ACTIVE || m_tank_state == FREEZE) return;

  if (m_dir != FORWD) setOrientation(FORWD);
  m_move_flag = true;
}

void Tank::Back() {
  if (m_state != ACTIVE || m_tank_state == FREEZE) return;

  if (m_dir != BACK) setOrientation(BACK);
  m_move_flag = true;
}

void Tank::Fire() {
  if (m_time_last - m_time_fire_last < m_delay_fire ||
    m_state != ACTIVE || m_tank_state == FREEZE) {
    return;
  }
  m_time_fire_last = m_time_last;
  char bullet_type = Tank::factory::s_collection[m_type].bullet_type;
  auto bullet = Bullet::factory::Create(bullet_type, m_time_last);
  bullet->setDirection(m_dir);
  bullet->setPosition(m_pos);
}

void Tank::setAutoPilot( ) {
  m_Driver.reset(new TankDriver(*this));
}

void Tank::setAutoPilot(TankDriverBase&& tank_capitan) {
  m_Driver.reset(&tank_capitan);
}

//    TANK STATES
void Tank::setStateActiveNormal() {
  m_state = ACTIVE;
  m_tank_state = NORMAL;
  // anim settings
  m_anim_list.Clear();
  const std::string& name = factory::s_collection.at(m_type).name;
  m_anim_list.InsertBack(unique_anim<AnimLoop>(m_time_last, name));
  m_anim_list.setPosition(m_pos);
  m_anim_list.setOrientation(m_dir);
}

void Tank::setStateFreeze() {
  m_tank_state = FREEZE;
  // anim settings
  m_anim_list.Clear();
  const std::string& name = factory::s_collection.at(m_type).name;
  auto tmp = unique_anim<AnimLoop>(m_time_last, name);
  tmp->setTimer(4);
  tmp->setBlink(0.1, sf::Color(100, 150, 250, 200));
  m_anim_list.InsertBack(std::move(tmp));
  m_anim_list.setPosition(m_pos);
  m_anim_list.setOrientation(m_dir);
}

void Tank::setStateImmortal() {
  m_tank_state = IMMORTAL;
  // anim settings
  auto tmp = unique_anim<AnimLoop>(m_time_last, "Shine");
  tmp->setTimer(10);
  m_anim_list.InsertBack(std::move(tmp));
  m_anim_list.setPosition(m_pos);
  m_anim_list.setOrientation(m_dir);
}


void Tank::setStateDestruction() {
  if (m_state == DESTRUC) return;
  m_state = DESTRUC;
  // anim settings
  m_anim_list.Clear();
  auto& anim = m_anim_list.InsertBack(unique_anim<AnimSingle>(m_time_last, "Bang"));
  anim->setPosition(m_pos);
  m_callback();
}


void Tank::setDestructionCallback(std::function<void(void)> callback) {
  m_callback = callback;
}


void Tank::UpdatePosition( ) {
  using namespace std;
  if (m_move_flag == false) return;
  m_move_flag = false;
  ++m_move_delay;
  int speed = factory::s_collection.at(m_type).speed;
  sf::Vector2f shift = MovingShift2D(m_elepsed_time, speed, m_dir);
  setPosition({m_pos.x + shift.x, m_pos.y + shift.y});
}



TankDriver::TankDriver(Tank& tank) : TankDriverBase(tank) {
  m_dir = getRandomDir();
}


void TankDriver::ApplyControl(float time ) {
  auto pos = m_tank.getPosition();
  float dx = pos.x - m_pos.x;
  float dy = pos.y - m_pos.y;
  m_pos = pos;
  if (std::abs(dx) < delta && std::abs(dy) < delta) {
    m_dir = getRandomDir();
  }
  switch (m_dir)
  {
  case FORWD :
    m_tank.Forward();
    break;
  case RIGHT:
    m_tank.Ridht();
    break;
  case BACK:
    m_tank.Back();
    break;
  case LEFT:
    m_tank.Left();
    break;
  }
  using namespace std;

  if (time - m_time_fire_last > m_fire_delay) {
    //std::cout << "fire " << std::endl;
    m_tank.Fire();
    m_time_fire_last = time;
  }
}


direction TankDriver::getRandomDir() {
  int dir = rand() % 4 * 90 - 90;
  direction d = static_cast<direction>(dir);
  //std::cout << "random direction\t: " << (int)d << std::endl;
  return d;
}







std::istream& operator>>(std::istream& is, BulletTypeInfo& item) {
  return is >> item.name >> item.type >> item.size.x >> item.size.y
            >> item.speed >> item.damage;
}

std::ostream& operator<<(std::ostream& is, const BulletTypeInfo& item) {
  std::string dlm = "\t";
  return is << item.name << dlm << item.type << dlm
    << item.size.x << dlm << item.size.y << dlm
    << item.speed << dlm << item.damage;
}


unsigned Bullet::id_cnt = 0;
Bullet::Bullet(char type, float time) : GameEntity(), m_type(type), 
                                        m_id(id_cnt++), m_time_last(time), m_state(ACTIVE) {
  using namespace std;
  unique_ptr<AnimBase> tmp_anim(new AnimBase(time));
  tmp_anim->setSprite("slow_soft");
  m_anim_list.InsertBack(std::move(tmp_anim));
}

void Bullet::Update(const sf::Time& time) {
  switch (m_state) 
  {
  case ACTIVE :
    UpdatePosition(time);
    break;
  case DESTRUC :
    if (m_anim_list.IsCompleted()) m_destroyed = true;
    break;
  default :
    ;
  }
  //m_anim.Update(time);
  m_anim_list.Update(time.asSeconds());
}

void Bullet::Draw(sf::RenderWindow& window) {
  /*if (!m_anim.IsCompleted()) {
    m_anim.Draw(window);
  }*/
  m_anim_list.Draw(window);
}

void Bullet::setStateDestruction() {
  if (m_state == DESTRUC) return;
  m_state = DESTRUC;
  
  m_anim_list.Clear();
  auto& tmp = m_anim_list.InsertBack(unique_anim<AnimSingle>(m_time_last, "Bang"));
  tmp->setPosition(m_pos);
}

void Bullet::Interaction(GameEntity&) {

}

void Bullet::setPosition(sf::Vector2f pos) {
  m_pos = pos;
  //m_anim.setPosition(pos);
  m_anim_list.setPosition(pos);
}

sf::Vector2f Bullet::getPosition() const {
  return m_pos;
}

sf::Vector2f Bullet::getSize() const {
  return factory::s_collection.at(m_type).size;
}

void Bullet::setDirection(direction dir) { 
  m_dir = dir; 
  //m_anim.setOrientation(dir);
  m_anim_list.setOrientation(dir);
}


/*void 
Bullet::setLastTime(float time) { 
  m_time_last = time; 
  m_anim.setLastTime(time);
}*/


void Bullet::UpdatePosition(const sf::Time& time) {
  float m_elapsed_time = time.asSeconds() - m_time_last;
  m_time_last = time.asSeconds();
  
  int speed = factory::s_collection.at(m_type).speed;

  float shift = m_elapsed_time * speed;
  float dx = shift*cos((float)m_dir / 180.0 * M_PI);
  float dy = shift*sin((float)m_dir / 180.0 * M_PI);
  sf::Vector2f pos(m_pos);
  pos.x += dx;
  pos.y += dy;
  setPosition(pos);
}




std::istream& operator>>(std::istream& is, BarrierTypeInfo& item) {
  is >> item.name >> item.type >> item.size.x >> item.size.y
     >> item.health >> item.obstruct_Z_eq_0 >> item.obstruct_Z_greater_0;
  return is;
}


std::ostream& operator<<(std::ostream& is, const BarrierTypeInfo& item) {
  std::string dlm = "\t";
  return is << item.name << dlm << item.type << dlm
    << item.size.x << dlm << item.size.y << dlm << item.health << dlm
    << item.obstruct_Z_eq_0 << dlm << item.obstruct_Z_greater_0;
}


unsigned Barrier::id_cnt = 0;
Barrier::Barrier(char type, float time) : GameEntity(), m_type(type), m_id(id_cnt++) {
  const std::string& name = factory::s_collection.at(type).name;
  m_sprite = SpriteManager::Get(name);
  m_sprite.setOrigin({ getSize().x / 2, getSize().y / 2 });
}

void Barrier::Update(const sf::Time&) { 
  m_sprite.setPosition(m_pos.x + m_local.x, m_pos.y + m_local.y);
}

void Barrier::Draw(sf::RenderWindow& window) {
  window.draw(m_sprite);
}

void Barrier::Interaction(GameEntity&) {
}

void Barrier::setStateDestruction() {
  m_destroyed = true;
}

bool Barrier::isTopDrawLayer() const {
  return m_type == '*';
}

void Barrier::setPosition(sf::Vector2f pos) {
  m_pos = pos;
  //std::cout << "Local : " << m_local.x << ";  y " << m_local.y;
  //getchar();
  m_sprite.setPosition(pos.x + m_local.x, pos.y + m_local.y);
}

sf::Vector2f Barrier::getPosition() const {
  return m_pos;
}

sf::Vector2f Barrier::getSize() const {
  if (m_type == 'B') {
    
  }
  return factory::s_collection.at(m_type).size;
}






std::istream& operator>>(std::istream& is, BonusTypeInfo& item) {
  return is >> item.name >> item.type >> item.size.x >> item.size.y
            >> item.period;
}

std::ostream& operator<<(std::ostream& is, const BonusTypeInfo& item) {
  std::string dlm = "\t";
  return is << item.name << dlm << item.type << dlm
            << item.size.x << dlm << item.size.y << dlm
            << item.period;
}


unsigned Bonus::id_cnt = 0;
Bonus::Bonus(char type, float time) : GameEntity(), m_type(type), m_id(id_cnt++) {
  const std::string& name = factory::s_collection.at(type).name;
  auto& anim = m_anim_list.InsertBack(AnimList::element(new AnimBase(0)));
  anim->setSprite(name);
  if (m_type != 'f') {
    anim->setBlink(0.2, sf::Color(255, 100, 255, 100));
  }
  m_callback = []() {};
}

void Bonus::Update(const sf::Time& time) {
  //m_sprite.setPosition(m_pos.x + m_local.x, m_pos.y + m_local.y);
  m_anim_list.Update(time.asSeconds());
  m_anim_list.setPosition(m_pos);
}

void Bonus::Draw(sf::RenderWindow& window) {
  //window.draw(m_sprite);
  m_anim_list.Draw(window);
}

void Bonus::DestroyThis() {
  m_destroyed = true;
}

void Bonus::Interaction(GameEntity&) {
}

void Bonus::setPosition(sf::Vector2f pos) {
  m_pos = pos;
  //m_sprite.setPosition(pos.x + m_local.x, pos.y + m_local.y);
}

sf::Vector2f Bonus::getPosition() const {
  return m_pos;
}

sf::Vector2f Bonus::getSize() const {
  return factory::s_collection.at(m_type).size;
}

void Bonus::setStateDestruction() {
  m_callback();
  m_destroyed = true;
}

void Bonus::setDestructionCallback(std::function<void(void)> callback) { 
  m_callback = callback;
}







void EntityInteraction(Tank& first, Tank& second) {
  sf::FloatRect rec_sect;
  if (first.m_state == DESTRUC || 
      second.m_state == DESTRUC ||
      !first.getBounds().intersects(second.getBounds(), rec_sect)) {
    return;
  }
  using namespace std;
  // выбрать нарушителя
  sf::Vector2f p1(first.getPosition());
  sf::Vector2f p2(second.getPosition());
  int dir1 = atan2(p2.y - p1.y, p2.x - p1.x) * 180.0 / M_PI; // направление от 1 k 2
  int dir2 = atan2(p1.y - p2.y, p1.x - p2.x) * 180.0 / M_PI; // направление от 2 к 1  
  if (dir1 < -(180 - 45)) dir1 = -dir1;
  if (dir2 < -(180 - 45)) dir2 = -dir2;
  if (abs(dir1 - first.getOrientation()) <= 45 ) {
    if(!first.isImmobile()) first.setPosition(first.m_pos_last);
  }
  if (abs(dir2 - second.getOrientation()) <= 45 ) {
    if (!second.isImmobile()) second.setPosition(second.m_pos_last);
  }
}


void EntityInteraction(Tank& t, Bullet& b) {
  if (t.m_state == DESTRUC || 
      t.m_state == BIRTH ||
      b.m_state == DESTRUC ||
      !t.getBounds().intersects(b.getBounds())) {
    return;
  }
  //std::cout << "Interaction: tank bullet" << std::endl;

  if (t.m_type != 'e' && b.m_type == 'S') {
    t.setStateDestruction();
    b.setStateDestruction();
  } 
  else if(t.m_type == 'e' && b.m_type != 'S') {
    t.setStateDestruction();
    b.setStateDestruction();
  }
}


void EntityInteraction(Tank& t, Barrier& b) {
  sf::FloatRect rec_sect;
  if (!t.getBounds().intersects(b.getBounds(), rec_sect)) return;
  // смотрим у разделяемых данных препятствий, можно ли проехать через это препятствие?
  const auto& item = Barrier::factory::s_collection.at(b.m_type);
  if (item.obstruct_Z_eq_0 == 1) return;
  //std::cout << "Interaction: tank barrier  " << std::endl;
  sf::Vector2f new_pos(t.getPosition());
  new_pos.x -= rec_sect.width  * cos((float)t.m_dir / 180.0f * M_PI);
  new_pos.y -= rec_sect.height * sin((float)t.m_dir / 180.0f * M_PI);
  t.setPosition(new_pos);
}


void EntityInteraction(Tank& t, Bonus& b) {
  sf::FloatRect rec_sect;
  if (!t.getBounds().intersects(b.getBounds(), rec_sect)) return;
  //std::cout << "Interaction: tank bonus" << std::endl;
  if (t.m_type == 'e') {
    b.setStateDestruction();
  }
}


void EntityInteraction(Bullet& b1, Bullet& bul2) {
  if (b1.m_state == DESTRUC || bul2.m_state == DESTRUC) return;
  if (!b1.getBounds().intersects(bul2.getBounds())) return;
  //std::cout << "Interaction: bullet bullet" << std::endl;
  using namespace std;
  bul2.setStateDestruction();
  b1.setStateDestruction();
}


void EntityInteraction(Bullet& bullet, Barrier& barrier) {
  if (bullet.m_state == DESTRUC ||
    !bullet.getBounds().intersects(barrier.getBounds())) {
    return;
  }
  if (barrier.m_type == 'V' || barrier.m_type == 'H') {
    bullet.setStateDestruction();
  }
  else {
    //    bullet.setStateDestruction();
    //    barrier.setStateDestruction();
    //  }
    switch (barrier.m_type)
    {
    case '-':
      bullet.setStateDestruction();
      barrier.setStateDestruction();
      break;
    case '=':
      bullet.setStateDestruction();
      break;
    default:
      break;
    }
  }
}

void EntityInteraction(Bullet& bullet, Bonus& bonus) {
  if (bullet.m_state == DESTRUC ||
      !bullet.getBounds().intersects(bonus.getBounds())) {
    return;
  }
  if (bonus.m_type == 'f') {
      bullet.setStateDestruction();
      bonus.setStateDestruction();
  }
}




//    tests          
void test_factory_loading();
void test_factory_create();



// main 
void test_factory_battle_city()
{
  //test_factory_loading();
  test_factory_create();
}



// definitions
#include "SharedContext.h"

//! [demo_code_factory_create]
void test_factory_create()
{
  // проверка документирвоания
  using namespace std;
  cout << "start game entity creating :" << endl;

  SharedContext(R"(..\resources\paths.cfg)");

  SpriteSheetInfo::LoadSpriteSheetInfo(SharedContext::getFilePath("SpriteSheet"));

  Tank::factory::LoadCollection(SharedContext::getFilePath("Tanks"));
  Tank::factory::Create('a', 0);
  Tank::factory::Create('b', 0);
  Tank::factory::Create('c', 0);
  Tank::factory::Create('d', 0);
  cout << "Tanks create" << endl;

  Bullet::factory::LoadCollection(SharedContext::getFilePath("Bullet"));
  Bullet::factory::Create('A', 0);
  Bullet::factory::Create('B', 0);
  Bullet::factory::Create('C', 0);
  Bullet::factory::Create('D', 0);
  cout << "Bullets create" << endl;

  Barrier::factory::LoadCollection(SharedContext::getFilePath("Barrier"));
  Barrier::factory::Create('-', 0);
  Barrier::factory::Create('+', 0);
  Barrier::factory::Create('*', 0);
  Barrier::factory::Create('/', 0);
  Barrier::factory::Create('=', 0);
  cout << "Barriers create" << endl;

  Bonus::factory::LoadCollection(SharedContext::getFilePath("Bonus"));
  Bonus::factory::Create('c', 0);
  Bonus::factory::Create('b', 0);
  Bonus::factory::Create('s', 0);
  Bonus::factory::Create('t', 0);
  Bonus::factory::Create('h', 0);
  Bonus::factory::Create('w', 0);
  Bonus::factory::Create('f', 0);
  cout << "Bonus create" << endl;

  GameEntity::g_entity_list.clear();

  cout << "after clearing" << endl;
  getchar();
}
//! [demo_code_factory_create]




void test_factory_loading() {
  using namespace std;
  cout << "start:" << endl;
  getchar();

  // tanks
  string file1 = R"(entity_info\tanks.cfg)";
  cout << file1 << endl;
  Tank::factory::LoadCollection(file1); 
  for (auto item : Tank::factory::s_collection) {
    cout << item.second << endl;
  }

  // 
  string file2 = R"(entity_info\bullet.cfg)";
  cout << file1 << endl;
  Bullet::factory::LoadCollection(file2);
  for (auto item : Bullet::factory::s_collection) {
    cout << item.second << endl;
  }

  // 
  string file3 = R"(entity_info\barrier.cfg)";
  cout << file3 << endl;
  Barrier::factory::LoadCollection(file3);
  for (auto item : Barrier::factory::s_collection) {
    cout << item.second << endl;
  }

  // 
  string file4 = R"(entity_info\bonus.cfg)";
  cout << file4 << endl;
  Bonus::factory::LoadCollection(file4);
  for (auto item : Bonus::factory::s_collection) {
    cout << item.second << endl;
  }

  cout << "end" << endl;
  getchar();
  //Factory<>
}