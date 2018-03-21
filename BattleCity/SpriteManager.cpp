#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>

#include "SpriteManager.h"
#include "SharedContext.h"

TextureManager::TextureManager() {
  LoadPaths(SharedContext::getFilePath("Textures"));
  LoadTextures();
}


TextureManager::~TextureManager() {
  for (auto item : m_textures) {
    delete item.second;
#ifdef NDEBUG
    std::cout << item.first << " : deleted" << std::endl;
#endif // NDEBUG
  }
}


void TextureManager::LoadPaths(const std::string& fileName) {
  using namespace std;
  ifstream paths;
  paths.open(fileName);
  if (paths.is_open()) {
    string line;
    while (std::getline(paths, line)) {
      stringstream keystream(line);
      string pathName;
      string path;
      keystream >> pathName;
      keystream >> path;
      if (!path.empty() && !pathName.empty()) { // защита от пустых строк 
        m_paths.emplace(pathName, path);
      }
    }
    paths.close();
#ifdef  NDEBUG
    cout << "Readble name & paths: " << endl;
    for (auto item : m_paths) {
      cout << item.first << " in path " << item.second << endl;
    }
#endif //  NDEBAG
    return;
  }
  std::cerr << "! Failed loading the path file: " << fileName << std::endl;
}


void TextureManager::LoadTextures() {
  for (auto item : m_paths) {
    sf::Texture* texture = new sf::Texture();
    if (!texture->loadFromFile(item.second))
    {
      delete texture;
      std::cerr << "no load texture : " << item.second << std::endl;
      getchar();
      terminate();
    }
    else {
      m_textures.emplace(item.first, texture);
    }
  }
}


const sf::Texture* TextureManager::Get(const std::string& id_name) {
  static TextureManager mngr;
  Textures::iterator iter;
  if ((iter = mngr.m_textures.find(id_name)) ==
    mngr.m_textures.end()) {
    return nullptr;
  } 
  else {
    return iter->second;
  }
}






SpriteInfo::SpriteInfo() : m_rect(sf::IntRect(0, 0, 0, 0)) {
};


SpriteInfo::SpriteInfo(const sf::Texture* texture , const sf::IntRect& rect)
    : m_texture(texture), m_rect(rect) {
}


std::istream& operator>>(std::istream& is, SpriteInfo& spriteInfo) {
  using namespace std;
  string  texture; 
  int    x, y, w, h;
  if (is >> texture) {
    cout << texture << endl;
    if (is.eof()) {        // не указываются границы
      cout << "EOF : " << endl;
      spriteInfo.m_texture = TextureManager::Get(texture);
      auto size = spriteInfo.m_texture->getSize();
      spriteInfo.m_rect = sf::IntRect(0, 0, size.x, size.y);
    }
    if(is >> x >> y >> w >> h ){  
      //cout << "x y z : " << endl;
      spriteInfo.m_texture = TextureManager::Get(texture);
      spriteInfo.m_rect = sf::IntRect(x, y, w, h);
    }
    else { // чисел для границ не хватает ИЛИ много
      cout << "LOL : " << endl;
      is.setstate(ios_base::badbit);
    }
  }
#ifdef NDEBUG
  if (!is.good()) {
  //  cerr << "no reading" << endl;
  }
#endif // NDEBUG

  return is;
}


std::ostream& operator<<(std::ostream& is, const SpriteInfo& spriteInfo) {
  using namespace std;
  const sf::IntRect & rec = spriteInfo.m_rect;
  cout << " Sprite info : Rect " << rec.left << 
    " " << rec.top << " " << rec.width << " " << rec.height;
  return is;
}




SpriteManager::SpriteManager() {
  LoadSprites();
}


sf::Sprite SpriteManager::Get(const std::string& id_name) {
  using namespace std;
  static SpriteManager mngr;
  SpriteInfoMap::iterator iter;
  if ((iter = mngr.m_spriteInfoMap.find(id_name)) ==
    mngr.m_spriteInfoMap.end()) {
    cout << endl << endl << "not faund sprite name : " << id_name << endl;
    getchar();
    throw invalid_argument("Несуществующее имя спрайта.");
  }
  else {
    return sf::Sprite(*iter->second.m_texture, 
              iter->second.m_rect);
  }
}



void SpriteManager::LoadSprites() {
  using namespace std;
  const string& file = SharedContext::getFilePath("Sprites");
  ifstream ifs(file);
  string line;
  if (!ifs.is_open()) {
    cerr << "No load Sprites file : " 
      << file << endl;
    getchar();
    terminate();
  }
  while (getline(ifs, line)) {
    if (line[0] != '#') {      // игнорим комменты
      istringstream spriteline(line);
      string name;
      SpriteInfo spriteInfo;
      if (spriteline >> name >> spriteInfo) {
        m_spriteInfoMap.insert({name, spriteInfo});
      }
    }
  }
}


//! [demo_code_texture_manager]
void test_TextureManager() {
  using namespace  sf;
  using namespace  std;

  sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);

    SharedContext(R"(..\resources\paths.cfg)");

  auto tex1 = TextureManager::Get("Tanks");
  auto tex2 = TextureManager::Get("Exit");
  Sprite sprite1(*tex1);
  sprite1.move(150, 50);

  Sprite sprite2(*tex2);
  sprite2.move(50, 200);

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear();
    window.draw(shape);
    window.draw(sprite1);
    window.draw(sprite2);
    window.display();
  }
}
//! [demo_code_texture_manager]


void test_SpriteManager() {
  using namespace std;

  string line;
  ifstream ifs;
  getchar();
  ifs.open("sprites.cfg");
  if (!ifs.is_open()) {
    cerr << "No OPen (" << endl;
  }
  if (getline(ifs, line)) {
    cout << "This read line" << line << endl;
    stringstream str(line);
    SpriteInfo spriteInfo;
    if (str >> spriteInfo){
      cout << "This read line :  " << spriteInfo << endl;
    }
  }
  getchar();
  return;
}



void test_SpriteManager_createSprite() {
  using namespace  sf;
  using namespace  std;

  RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
  CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);

  //Texture tx = *TextureManager::Get("Tanks");
    SharedContext(R"(..\resources\paths.cfg)");

    Sprite sp1 = SpriteManager::Get("main_label");
  Sprite sp2 = SpriteManager::Get("1_player");
  sp2.move(200, 200);

  while (window.isOpen())
  {
    Event event;
    while (window.pollEvent(event))
    {
      if (event.type == Event::Closed)
        window.close();
    }

    window.clear();
    window.draw(shape);
    window.draw(sp1);
    window.draw(sp2);
    window.display();
  }
}
