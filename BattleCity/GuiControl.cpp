#include <cctype>

#include "GuiControl.h"


VisualPanel::VisualPanel(sf::Vector2u size, sf::Vector2f cell_sz, float brd)
    : m_sz(size), m_cel_sz(cell_sz), m_backg({ size.y*cell_sz.x, size.x*cell_sz.y }), m_brd(brd) {
  m_backg.setFillColor(sf::Color(150, 150, 50));
  m_backg.setOutlineThickness(m_brd);
  m_backg.setOutlineColor(sf::Color(50, 50, 50));
}


void VisualPanel::Draw(sf::RenderWindow& window) const {
  window.draw(m_backg);
  for (auto& item : m_items) {
    item.second->Draw(window);
  }
}


void VisualPanel::setPosition(float x, float y) {
  m_backg.setPosition(x, y);
  auto bnd = getOrigin();
  for (auto item : m_items) {
    item.second->setOrigin(-bnd.x, -bnd.y);
  }
}


void VisualPanel::setOrigin(float x, float y) {
  m_backg.setOrigin(x, y);
  auto bnd = getOrigin();
  for (auto item : m_items) {
    item.second->setOrigin(-bnd.x, -bnd.y);
  }
}


sf::Vector2f VisualPanel::getOrigin() const {
  auto b = m_backg.getGlobalBounds();
  return {b.left+m_brd, b.top+m_brd};
}


sf::FloatRect VisualPanel::getBounds() const {
  return m_backg.getGlobalBounds();
}


void VisualPanel::Place(VisualBase* item) {
  int line = m_cnt / m_sz.y;
  int caln = m_cnt % m_sz.y;
  //std::cout << "__ : " << line << "__ : " << caln << std::endl;
  item->setPosition(caln * m_cel_sz.x, line * m_cel_sz.y);

  auto xy = getOrigin();
  item->setOrigin(-xy.x, -xy.y);
  m_items.insert({ m_cnt, item });
  ++m_cnt;
  std::cout << m_items.size() << std::endl;
}


void VisualPanel::Place(VisualBase* item, sf::Vector2u pos) {
  int cnt = pos.y * m_sz.y + pos.x;
  Contents::iterator iter;
  if ((iter = m_items.find(cnt)) != m_items.end()) {
    m_items.erase(iter);
  }
  m_items.insert({ cnt, item });
  auto xy = getOrigin();
  item->setOrigin(-xy.x, -xy.y);
  item->setPosition(pos.y*m_cel_sz.x, pos.x*m_cel_sz.y);
}


void VisualPanel::setColor(sf::Color clr) {
  m_backg.setFillColor(clr);
  m_backg.setOutlineColor(clr);
}





SpriteTable::SpriteTable(const sf::Vector2u& size, const sf::Vector2f& cell_sz)
    : m_sz(size), m_cel_sz(cell_sz), m_backg({ size.y*cell_sz.x, size.x*cell_sz.y }) {
  m_backg.setFillColor(sf::Color(50,50,50));
}

void SpriteTable::Draw(sf::RenderWindow& window) const {
  window.draw(m_backg);
  for (auto& item : m_items) {
    window.draw(*item.second);
  }
}

void SpriteTable::setPosition(float x, float y) {
  m_backg.setPosition(x, y);
  sf::FloatRect bnd = m_backg.getGlobalBounds();
  for (auto item : m_items) {
    item.second->setOrigin(-bnd.left, -bnd.top);
  }
}


void SpriteTable::setOrigin(float x, float y) {
  m_backg.setOrigin(x, y);
  sf::FloatRect bnd = m_backg.getGlobalBounds();
  for (auto item : m_items) {
    item.second->setOrigin(-bnd.left, -bnd.top);
  }
}


sf::FloatRect SpriteTable::getBounds() const {
  return m_backg.getGlobalBounds();
}


// own metods
void SpriteTable::Place(sf::Sprite* sp) {
  if (sp) {
    int line = m_cnt / m_sz.y;
    int caln = m_cnt % m_sz.y;
    //std::cout << "__ : " << line << "__ : " << caln << std::endl;
    sp->setPosition(caln * m_cel_sz.x, line * m_cel_sz.y);

    auto xy = m_backg.getGlobalBounds();
    sp->setOrigin(-xy.left, -xy.top);
    m_items.insert({ m_cnt, sp });
  }
  ++m_cnt;
}


void SpriteTable::Place(sf::Sprite* sp, sf::Vector2u pos) {
  int cnt = pos.y * m_sz.y + pos.x;
  //std::cout << "pos : " << cnt << std::endl;
  Contents::iterator iter;
  if ((iter = m_items.find(cnt)) != m_items.end()) {
    m_items.erase(iter);
  }
  m_items.insert({ cnt, sp });
  auto xy = m_backg.getGlobalBounds();
  sp->setOrigin(-xy.left, -xy.top);
  sp->setPosition(pos.y*m_cel_sz.x, pos.x*m_cel_sz.y);
}


void SpriteTable::Insert(sf::Sprite* src, unsigned cnt) {
  auto xy = m_backg.getGlobalBounds();
  src->setOrigin(-xy.left, -xy.top);
  m_items.insert({cnt, src});
}


void SpriteTable::PopBack() {
  m_items.erase(m_cnt);
  --m_cnt;
}





CharDisplay::CharDisplay(sf::Vector2u sz, sf::Vector2f c_sz) 
    : SpriteTable(sz, c_sz), m_sz(sz), m_clr(sf::Color(255, 255, 128))
{}

CharDisplay& CharDisplay::operator<<(const std::string& str) {
  using namespace std;
  istringstream is(str);
  string line;
  getline(is, line);
  do {
    for (auto ch : line) {
      ch = tolower(ch);
      if (csr.y < m_sz.y) {
        if (ch == ' ') {
          m_buffer.push_back(SpriteManager::Get("unknown"));
          Place(&m_buffer.back(), { csr.x, csr.y++ });
          continue;
        }
        try {
          m_buffer.push_back(SpriteManager::Get(std::string(1, ch)));
          Place(&m_buffer.back(), { csr.x, csr.y++ });
        }
        catch (...) {
          cout << __FUNCTION__<< endl;
          cout << "fatal symbol value in Char Display:" << endl;
          getchar();
          terminate();
        }
        m_buffer.back().setColor(m_clr);
      }
      else break;
    }
  } while ( ( getline(is, line) && !(csr.y = 0) )  && 
        (csr.x++ < m_sz.x) ) ;
  return *this;
}


void CharDisplay::Clear() {
  SpriteTable::Clear();
  csr = { 0,0 };
  m_buffer.clear();
}


CharDisplay& CharDisplay::setColor(sf::Color clr) {
  m_clr = clr;
  return *this;
}



//----------- TESTS





// declaration
void test_VisualPanel();
void test_SpriteTable();
void test_Scoreboard();
void test_CharDisplay();

// main test
void test_GuiControl()
{
  //test_VisualPanel();
  //test_SpriteTable();
  //test_Scoreboard();
  test_CharDisplay();
}


// definitions test fanctions

#include "SharedContext.h"

//! [demo_code_char_display]
void test_CharDisplay() {
  using namespace  sf;
  using namespace  std;

    SharedContext(R"(..\resources\paths.cfg)");

  CharDisplay dsp({ 5, 10 }, {16,32});
  dsp << "he\n\n-";

  dsp.setColor(Color::Yellow) << "world";
  dsp.setColor() << "color";
  // imaging
  sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
  while (window.isOpen()){
    sf::Event event;
    while (window.pollEvent(event)){
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();
    dsp.Draw(window);
    window.display();
  }
}
//! [demo_code_char_display]  

/*
void test_Scoreboard()
{
  using namespace  sf;
  using namespace  std;

  Scoreboard sb(10, { 30, 30 });
  sb.setString("abcd 123");
  sb.setPosition(100, 100);

  Scoreboard sb2(sb);
  
  VisualPanel pn({ 4, 1 }, sb.getBoundsSize(), 10);
  pn.Place(&sb);
  pn.Place(&sb2, {3,0});

  pn.setPosition(50, 50);

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

    window.clear();
    //window.draw(sp);
    pn.Draw(window);

    window.display();
  }
}
*/


void test_SpriteTable()
{
  using namespace  sf;
  using namespace  std;

  sf::Sprite s1(SpriteManager::Get("1"));
  sf::Sprite s2(SpriteManager::Get("2"));
  sf::Sprite s3(SpriteManager::Get("3"));
  sf::Sprite s4(SpriteManager::Get("a"));
  sf::Sprite s5(SpriteManager::Get("b"));
  
  SpriteTable tb({ 1, 10 }, { 32, 32 });
  tb.Place(&s1);
  tb.Place(&s2);
  tb.Place(&s3);
  tb.Place(&s4);
  tb.Place(&s5);

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

    window.clear();
    //window.draw(sp);
    tb.Draw(window);

    window.display();
  }
}



void test_VisualPanel()
{
  using namespace  sf;
  using namespace  std;

  sf::Sprite s1(SpriteManager::Get("1"));
  sf::Sprite s2(SpriteManager::Get("2"));
  sf::Sprite s3(SpriteManager::Get("3"));
  sf::Sprite s4(SpriteManager::Get("a"));
  sf::Sprite s5(SpriteManager::Get("b"));


  SpriteTable tb({ 1, 5 }, { 20, 20 });
  tb.Place(&s1);
  //tb.Place(&s3);
  tb.Place(&s4, {0, 4});

  //tb.Place(&s5, {0,3});

  VisualPanel pan({ 1, 3 }, {tb.getBounds().width, tb.getBounds().height});
  
  SpriteTable tb2({1, 5}, {20, 20});
  tb2.Place(&s2, {0, 2});
  tb2.Place(&s3, {0, 4});


  pan.Place(&tb);
  pan.Place(&tb2);


  //pan.setPosition(50, 100);

  // imaging
  sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
  while (window.isOpen())  {
    sf::Event event;
    while (window.pollEvent(event))    {
      if (event.type == sf::Event::Closed) window.close();
    }
    
    window.clear();
    //window.draw(sp);
    pan.Draw(window);

    window.display();
  }
}
