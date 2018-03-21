/// \file 
/// \ingroup battle_city_game_classes
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <array>
#include <memory>
#include <stdexcept>
#include <utility>

#include <SFML\Graphics.hpp>

#include "Animating.h"


/// \brief объекты, способные к взаимному механическому контакту
/// 
/// Базовый класс всех игровых объектов, поведение которых завист от их позиции и размеров. 
/// Формирует интерфейс для алгоритма поиска геометрических пересечений.
/// Через статический контейнер класса можно получить доступ ко всем имеющимся 
///   экземплярам класса.
/// Единственный класс, работающий с конкретными экземплярами объектов (GameScenario) 
/// не требует владения ими. И обращается через указатель в контейнере-хранилище.
/// Экземпляры класса не могут коприроваться и присваиваться, т.к. описывают уникальные сущности.
/// Оператор перемещения и копирования при перемещении требуется для использования контейнеров STL.
/// Для помощи в управлении созданием, хранением и уничтожением многочисленными экземплярами 
///   GameEntity введен класс-шаблон Factory. 
/// \see EntityManager::Update, EntityManager::Draw, Factory
class GameEntity {
 public:
  /// хранилище указателей "живущих" экземпляров класса. 
  using Entity_ptr_list = std::list<std::shared_ptr<GameEntity>>;

  GameEntity() = default;
  GameEntity(GameEntity&)             = delete;
  GameEntity& operator=(GameEntity&)  = delete;
  GameEntity(GameEntity&&)            = default;
  GameEntity& operator=(GameEntity&&) = default;  
  virtual ~GameEntity() = default;
  
  // for alhoritm find intersections
  virtual void setPosition(sf::Vector2f ) = 0;
  virtual sf::Vector2f getPosition()  const = 0;
  virtual sf::Vector2f getSize()    const = 0;
  
  /// прямоугольные размеры занимаемой области
  sf::FloatRect getBounds() const {
    sf::Vector2f sz = getSize();
    sf::Vector2f pos = getPosition();
    float d = 0; // уменьшение размера
    return {pos.x - sz.x/2+d, pos.y - sz.y / 2+d, 
            sz.x - 2*d, sz.y - 2*d};
}

  /// не используется, (предполагался для использования RTTI при обработке взаимодействий)
  virtual void Interaction(GameEntity&) = 0;

  virtual void Update(const sf::Time& )  = 0 ;
  virtual void Draw(sf::RenderWindow& window) = 0;
  virtual bool isDestroyed() const = 0;          ///< индикатор уничтожения 
  static sf::Vector2f m_local;                   ///< начало координат системы отсчета позиции

  /// при уделении элемента из этого контейнера удаляется сам объект в содержащем его 
  /// контейнере. (Factory::s_current_set)
  static Entity_ptr_list g_entity_list;
};



/// \brief Создает экземпляры GameEntity и размещает их в едином хранилище, для централизованного доступа
/// \tparam  T  тип игровой сущности, 
/// \tparam  T_info  параметры конкретного типа сущности
/// 
/// Продеставляет глобальный доступ к методу создания игровых единиц. 
/// При создании предоставляет доступ по указателю к новому объекту, для его настроек.
/// При создании каждый экземпляр размещается в статическом контейнере STL,
///    инстанцированному для каждого типа наследников GameEntity.
/// Также, контейнер STL ипользуется из-за его аллокатора для большего порядка в куче.
/// Предоставляет централизованный доступ ко всем "живущим" экземплярам параметризующего класса T.
/// Является подобием паттерна "фабрика" со статическим методом создания.
/// Создает набор T_info, где лежат значения параметров конкретных подтипов (T::char type_) экземпляров класа T,
///   загружая хначения из файла.
/// Элеметны T_info хранилища выступают в роли разделяемых данных для 
/// экземпляров игровых единиц соответствующего подтипа (T::char type_).
/// (структура экземпляров T опирается на паттерн "Flyweight")
/// 
/// Пример использования: \snippet Units.cpp demo_code_factory_create
///
/// \see Tank, Bullet, Barrier, Bonus, GameScenario, EntityManager, example_unit_create.cpp
template <typename T, typename T_info>
class Factory {
 public:
  Factory(Factory&)        = delete;
  Factory& operator= (Factory&)  = delete;

  /// тип контейнера, где хранятся экземпляры. 
  // Список выбран, т.к. сохраняет итераторы действительными после произвольного удалениия
  using CurrentSet = std::list<T>;

  /// тип при помощи которого предоставляется косвенный доступ к созданному экземпляру
  using iterator = typename CurrentSet::iterator;

  /// хранит набор параметров типа (T::char type_), разделяемых экземплярами 
  using Collection = std::unordered_map<char, const T_info>;

  /// хранилище "живущих" на данный момент экземпл¤ров T
  static CurrentSet s_current_set;
  
  /// хранилище параметров типа (T::char type_), разделяемых экземплярами 
  static Collection s_collection;

  /// создает новый экземпляр типа T, размещяет его в хранилище экземпляров
  /// и возвращает итератор на созданный элекмент.
  /// \param type подтип игровой единицы T::char type_
  /// \param time момент времени создания (от момента старта игровой сценты)
  static iterator Create(char type, float time) {
    using namespace std;
    if ( s_collection.find(type) == s_collection.end()) {
      cout << "not find type :" << type << endl;
      if (s_collection.empty()) {
        cout << "s_collection is empty." << endl; 
      }
      getchar();
      throw invalid_argument("unknown type");
    }
    // размещаем объекn в контйнере
    s_current_set.push_back( T(type, time) );
    // регистрируем указаетль на объект в контейнере с единым типом
    auto iter = --s_current_set.end();  // итератор на размещенный объект
    auto deleter = [iter](GameEntity* ptr) {
      s_current_set.erase(iter);
    };
    GameEntity::g_entity_list.push_back(
      shared_ptr<GameEntity>(&s_current_set.back(), deleter));
    return --s_current_set.end();
  }
  // загрузка набора разделяемых данных конкретных подтипов класса T из файла в коллекцию класса
  static void LoadCollection(const std::string& file);
 
 private:
  // Изначально планировоалось определить класс как Singleton,
  // с тех пор осталось. пока так.
  Factory(std::string type_file);
};


template <typename T, typename T_info>
void Factory<T, T_info>::LoadCollection(const std::string& file) {
  using namespace std;

  ifstream fin(file);
  if (!fin.is_open()) {
    cout << "No find file: " << file << endl;
    getchar();
    throw invalid_argument("Can't open tiles file");
  }

  string line, name;
  int line_cnt = 0;
  while (getline(fin, line)) {
    ++line_cnt;
    if (line[0] == '#') continue;  // comments

    istringstream is(line);
    T_info tmp_info;
    if (is >> tmp_info) {
      s_collection.emplace(tmp_info.type, std::move(tmp_info));
    }
    else {
      cout << " can't load line N " << line_cnt << " : ";
      cout << line << endl;
      getchar();
      throw invalid_argument("Can't load tile sprites");
    }
  }
  return;
}

template <typename T1, typename T2>
typename Factory<T1, T2>::CurrentSet Factory<T1, T2>::s_current_set;

template <typename T1, typename T2>
typename Factory<T1, T2>::Collection Factory<T1, T2>::s_collection;
