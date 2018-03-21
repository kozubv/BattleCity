#include "Animating.h"

///---------------------------------------------------------
/// AnimBase

sf::Vector2f AnimBase::s_local;

///---------------------------------------------------------
AnimBase::AnimBase(float start_time) : m_time_last(start_time),
                                       m_color(sf::Color::Transparent) {
}

///---------------------------------------------------------
void AnimBase::Update(float time) {
  UpdateBlink(time);
}

///---------------------------------------------------------
void AnimBase::Draw(sf::RenderWindow & window) {
  window.draw(m_sprite);
}

///---------------------------------------------------------
void AnimBase::setSprite(const std::string & name) {
  m_sprite = SpriteManager::Get(name);
  auto rec = m_sprite.getLocalBounds();
  m_sprite.setOrigin(rec.width / 2, rec.height / 2);
  m_name = name;
}

///---------------------------------------------------------
void AnimBase::setSpriteSheet(const std::string& name) {
  using namespace std;
  m_name = name;
  m_info = SpriteSheetInfo::getSheet(name);
  m_sprite.setTexture(*m_info->texture);

  sf::IntRect rec;
  rec.left = m_info->origin_pos.x;
  rec.top = m_info->origin_pos.y;
  rec.width = m_info->size.x;
  rec.height = m_info->size.y;

  m_sprite.setTextureRect(rec);
  m_sprite.setOrigin(m_info->size.x / 2, m_info->size.y / 2);
}

///---------------------------------------------------------
void AnimBase::setOrientation(float angle) {
  m_sprite.setRotation(angle);
}

///---------------------------------------------------------
void AnimBase::setPosition(sf::Vector2f pos) {
  m_sprite.setPosition({ pos.x + s_local.x, pos.y + s_local.y });
}

///---------------------------------------------------------
void AnimBase::setBlink(float period, sf::Color cl) {
  m_blink_period = period;
  m_color = cl;
}

///---------------------------------------------------------
void AnimBase::UpdateBlink(float time) {
  using namespace std;
  if (!m_blink_period) return;

  m_time_blink = time - m_time_blink_last;
  if (m_time_blink < m_blink_period) return;

  m_time_blink_last = time;
  m_time_blink = 0;

  if (m_blink_switch) {
    m_sprite.setColor(m_color);
  }
  else {
    m_sprite.setColor(sf::Color::White);
  }
  m_blink_switch = !m_blink_switch;
  return;
}


///---------------------------------------------------------
/// AnimSingle

AnimSingle::AnimSingle(float time_start, const std::string & sheet_name) 
    try : AnimBase(time_start) {
      setSpriteSheet(sheet_name);
    }
catch (...) {
  std::cout << "Not found sprite sheet : in SingleAnimation constructor"
            << std::endl;
  getchar();
  terminate();
}

///---------------------------------------------------------
void AnimSingle::Update(float time) {
  AnimBase::UpdateBlink(time);
  if (f_stop || (time - m_time_last) < m_info->frame_time) {
    return;
  }
  m_time_last = time;
  ++m_current_frame;
  if (m_current_frame == m_info->frame_num) {
    f_stop = true;
    //std::cout << "stop frame" << std::endl;
    return;
  }
  int x = m_info->origin_pos.x + m_info->size.x * m_current_frame;
  sf::IntRect rec = m_sprite.getTextureRect();
  rec.left = x;
  m_sprite.setTextureRect(rec);
  //std::cout << "change frame" << std::endl;
}


///---------------------------------------------------------
/// AnimLoop

AnimLoop::AnimLoop(float time_start, const std::string & sheet_name) 
    try : AnimBase(time_start) {
      setSpriteSheet(sheet_name);
}
catch (...) {
  std::cout << "Not found sprite sheet : in LoopAnimation constructor"
            << std::endl;
  getchar();
  terminate();
}

///---------------------------------------------------------
void AnimLoop::Update(float time) {
  AnimBase::UpdateBlink(time);

  using namespace std;
  if (time - m_time_last < m_info->frame_time) {
    return;
  }
  if (time - m_timer_start > m_timer) {
    f_stop = true;
    return;
  }
  m_time_last = time;

  int next_frame_position = ++m_current_frame % m_info->frame_num;
  //cout << "next_frame_position \t" << next_frame_position << endl;
  int x = m_info->origin_pos.x + m_info->size.x * next_frame_position;
  sf::IntRect rec = m_sprite.getTextureRect();
  rec.left = x;
  m_sprite.setTextureRect(rec);
}

///---------------------------------------------------------
void AnimLoop::setTimer(float timer) {
  m_timer_start = m_time_last;
  m_timer = timer;
}


///---------------------------------------------------------
/// SpriteSheetInfo

SpriteSheetInfo::Collection SpriteSheetInfo::s_collection;

///---------------------------------------------------------
void SpriteSheetInfo::LoadSpriteSheetInfo(std::string file) {
  using namespace std;

  ifstream fin(file);
  if (!fin.is_open()) {
    cout << "No find file: " << file << endl;
    getchar();
    throw invalid_argument("Can't open spriteSheet file");
  }

  string line, texture_name;

  // unique parametres
  while (getline(fin, line)) {
    if (line[0] == '#') continue;  // comments
    istringstream is(line);
    SpriteSheetInfo tmp_info;
    if (is >> tmp_info.name
      >> texture_name
      >> tmp_info.size.x
      >> tmp_info.size.y
      >> tmp_info.frame_time
      >> tmp_info.origin_pos.x
      >> tmp_info.origin_pos.y
      >> tmp_info.frame_num
      >> tmp_info.loop_flag) {
      tmp_info.texture = TextureManager::Get(texture_name);
      s_collection.emplace(tmp_info.name, std::move(tmp_info));
    }
    else {
      cout << " Animating: can't load, sprite sheet  : "
        << line << endl;
      getchar();
      throw invalid_argument("Can't load tile sprites");
    }
  }
  return;
}

///---------------------------------------------------------
const SpriteSheetInfo * SpriteSheetInfo::getSheet(std::string name) {
  //Collection::iterator iter;
  auto iter = s_collection.find(name);
  if (iter == s_collection.end()) {
    std::cout << "faled in getting sprite sheet: " << std::endl;
    std::cout << "invalid name : " << name << std::endl;
    if (s_collection.empty()) {
      std::cout << "ANIM COLLECTION IS EMPTY =<" << std::endl;
    }
    getchar();
    throw std::invalid_argument(" no ");
  }
  return &iter->second;
}

///---------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const SpriteSheetInfo& trg) {
  std::string del = "\t";
  os << trg.name << del
    << trg.size.x << del << trg.size.y << del
    << trg.origin_pos.x << del << trg.origin_pos.y << del
    << trg.frame_num << del
    << trg.frame_time
    << trg.loop_flag;
  return os;
}


///---------------------------------------------------------
/// AnimList

void AnimList::Update(float time) {
  using namespace std;

  m_list.remove_if([](element& ptr)
  {
    return ptr->IsCompleted();
  });
  for (auto& item : m_list) {
    item->Update(time);
  }
}

///---------------------------------------------------------
void AnimList::Draw(sf::RenderWindow & window) {
  for (auto& item : m_list) {
    item->Draw(window);
  }
}

///---------------------------------------------------------
void AnimList::setPosition(sf::Vector2f pos) {
  for (auto& item : m_list) {
    item->setPosition(pos);
  }
}

///---------------------------------------------------------
void AnimList::setOrientation(float ang) {
  for (auto& item : m_list) {
    item->setOrientation(ang);
  }
}

///---------------------------------------------------------
void AnimList::Remove(const std::string& name) {
  using namespace std;
  m_list.remove_if([&name](element& ptr)
  {
    return ptr->getName() == name;
  });
}


///---------------------------------------------------------
AnimList::element& AnimList::InsertBack(element&& ptr) {
  m_list.emplace_back(std::move(ptr));
  return m_list.back();
}

///---------------------------------------------------------
AnimList::element& AnimList::InsertFront(element&& ptr) {
  m_list.emplace_front(std::move(ptr));
  return m_list.back();
}

///---------------------------------------------------------
bool AnimList::IsCompleted(const std::string& name) {
  using namespace std;
  auto tmp = find_if(m_list.begin(), m_list.end(), [&name](element& ptr)
  {
    return ptr->getName() == name && !ptr->IsCompleted();
  });
  return tmp == m_list.cend();
}





// tests


// declaration
void test_load_sprite_sheet();
void test_animate_scene();
void test_animate_queue();
void test_anim_base();

// main
void test_animating()
{
  //test_load_sprite_sheet();
  //test_animate_scene();
  test_animate_queue();
  //test_anim_base();
}



// definitions
void test_load_sprite_sheet()
{
  using namespace std;
  SpriteSheetInfo::LoadSpriteSheetInfo(R"(entity_info\sprite.sheet)");
  for (auto item : SpriteSheetInfo::s_collection) {
    cout << item.second << endl;
  }
  getchar();
}




void test_anim_base()
{
  using namespace  sf;
  using namespace  std;

  test_load_sprite_sheet();
  Clock cl;
  ////////////////////////////
  //aq.push_back("PlayerC");
  //aq.push_back("Shine");
  //aq.push_back("Flare");

  AnimBase ab(0);
  ab.setSprite("cursor");
  ab.setPosition({ 100, 100 });
  ab.setBlink(0.4, sf::Color::Magenta);

  AnimLoop al(0, "PlayerC");
  al.setPosition({ 200,100 });
  al.setTimer(5);

  AnimSingle as(0, "PlayerB");
  as.setPosition({ 200,200 });

  // imaging
  sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
      // left key is pressed: move our character
      cout << sf::Event::Count << endl;
    }

    ab.Update(cl.getElapsedTime().asSeconds());
    al.Update(cl.getElapsedTime().asSeconds());
    as.Update(cl.getElapsedTime().asSeconds());

    window.clear();
    //
    ab.Draw(window);
    al.Draw(window);
    as.Draw(window);
    //
    window.display();

    //sf::sleep(sf::seconds(0.05));
    //cl.restart();
  }
}




#include "SharedContext.h"
//! [demo_code_animating]
void test_animate_queue() {
  using namespace  sf;
  using namespace  std;

    SharedContext(R"(..\resources\paths.cfg)");

    SpriteSheetInfo::LoadSpriteSheetInfo(SharedContext::getFilePath("SpriteSheet"));

  AnimList aq;

  unique_ptr<AnimSingle>  la1(new AnimSingle(0, "Bang"));

  auto tmp = unique_anim<AnimLoop>(0, "PlayerB");
  tmp->setTimer(10);
  tmp->setBlink(0.5, sf::Color::Blue);

  aq.InsertBack(std::move(tmp));

  auto tmp2 = unique_anim<AnimLoop>(0, "Shine");
  tmp2->setTimer(3);
  aq.InsertBack(std::move(tmp2));

  aq.setPosition({ 100, 100 });
  aq.setOrientation(270);
  
    Clock cl;
  // imaging
  sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    aq.Update(cl.getElapsedTime().asSeconds());

    window.clear();
    //
    aq.Draw(window);
    window.display();
    sf::sleep(sf::seconds(0.08));
  }
}
//! [demo_code_animating]

