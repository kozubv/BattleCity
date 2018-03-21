/// \file 
/// \ingroup battle_city_game_classes
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <functional>
#include <utility>
#include <stdexcept>
#include <cmath>
#include <cstdlib>

#include "SFML\Graphics.hpp"

#include "EntityBase.h"
#include "SpriteManager.h"
#include "Animating.h"

/// \example example_unit_create.cpp
/// \brief Демонстрирует примеры создания игровых сущностей и их удаление

class Bullet;
class Barrier;
class Bonus;
class TankDriverBase;

/// направления, доступные для юнитов
// явно продублирован знаковый тип значений,
// угол [0 .. 180, 0 ..-180], знак + от оси X к Y
enum direction : int { 
  FORWD = -90,
  RIGHT = 0,
  BACK = 90, 
  LEFT = 180 
};

/// состояние отображения
enum state { 
  BIRTH = 1,  ///< появление 
  ACTIVE = 2, ///< действие
  DESTRUC = 3 ///< уничтожение
};


/// \brief разделяемая информация для подтипов экземпляров класса Tank
/// \see Factory
struct TankTypeInfo {
  std::string name;
  char type = 0;                      ///< подтип, 
  sf::Vector2f size;                  ///< размер прямоугольной области

  int   health = 0;                   ///< здоровье
  int   speed = 0;                    ///< скорость перемещения
  char bullet_type = 0;               ///< подтип используемого снаряда (класса Bullet)
};
std::istream& operator>>(std::istream& is, TankTypeInfo& item);
std::ostream& operator<<(std::ostream& is, const TankTypeInfo& item);


/// \brief Танк
///
/// Предоставляет интерфейс управления движением по игровому полю
/// Имеет подтипы, указанные в члене-данных char type_
///   подтип определяет параметры TankTypeInfo включенные в состав объекта, как разделяемые данные.
/// Определяет поведение в зависимости от текущего состояния.
/// Обеспечивает графическое отображение \a танка на игровом поле в зависимости от состояния
/// Предоставляет возможность назначить каждому экземпляру стратегию автономного
///   поведения Tank::setAutoPilot (паттерн "стратегия" \ref TankDriverBase) 
/// Позволяет предоставить команду, исполняемую при уничтожении 
///   Tank::setDestructionCallback (паттерн "команда") 
/// 
/// Коллекция подтипов: 
/// \include ..\resources\tanks.cfg
class Tank : public GameEntity {
  /// состояния от воздействия бонусов
  enum tank_state {
    NORMAL = 0,    ///< cтандартное 
    FREEZE = 1,    ///< неподвижное
    IMMORTAL = 2   ///< бессмертен
  };

 public:
  using Info = TankTypeInfo;                        ///< структура разделяемых данных
  using factory = Factory<Tank, TankTypeInfo>;      ///< класс, управляющи созданием и размещением 
  using iterator = factory::iterator;               ///< тип косвенного доступа
  friend factory; // локализует создание экземпляров
  
  void Interaction(GameEntity&)  override;
  void Update(const sf::Time& time)  override;
  void Draw(sf::RenderWindow& window)  override;
  bool isDestroyed() const override { return m_destroyed; }
  bool isImmobile() const { return !bool(m_move_delay); } ///< индикатор неподвижности
  
  void setPosition(sf::Vector2f )    override;
  void setOrientation(direction);                    ///< задать направление
  sf::Vector2f getPosition()  const override;
  direction   getOrientation() const { return m_dir; }
  sf::Vector2f getSize()    const override;

  void Left();                      ///< перемешение
  void Ridht();                     ///< перемешение
  void Forward();                   ///< перемешение
  void Back();                      ///< перемешение
  void Fire();                      ///< стрельба

  /// переводит \a стретегию поведения в автономный режим по умолчанию
  void setAutoPilot( );

  /// переводит поведение танка в автономный режим, предоставляя стратегию поведения
  /// \param tank_capitan стретегия поведения танка (r-value ссылка)
  /// \note экземпляр стратегии удалится вместе с танком
  void setAutoPilot(TankDriverBase&& tank_capitan);

  void setStateFreeze();                  ///< неподвижное состояние
  void setStateImmortal();                ///< бессмертие
  void setStateDestruction();             ///< разрушение
  
  /// принимает команду, для вызова при уничтожении (например оповещает слушателя)
  void setDestructionCallback( std::function<void(void)> callback );

  // данные открываются для обработки взаимодействий
  friend void EntityInteraction(Tank&, Tank&);
  friend void EntityInteraction(Tank&, Bullet&);
  friend void EntityInteraction(Tank&, Barrier&);
  friend void EntityInteraction(Tank&, Bonus&);

 private:
  /// \param type подтип 
  /// \param time время создания
  Tank(char type, float time);

  void setStateActiveNormal();              ///< переводит в активное нормальное состояние
  void UpdateActiveState( );
  void UpdatePosition( );                   ///< переместить юнит и анимацию

  std::unique_ptr<TankDriverBase> m_Driver;        ///< стратегия поведения
  
  sf::Vector2f m_pos;                       ///< координаты на игровом поле
  sf::Vector2f m_pos_last;                  ///< позиция до обновления

  direction m_dir = FORWD;                  ///< направление движения
  const int m_id;                           ///< уникальный номер экземпляра
  char      m_type;                         ///< подтип (\ref Factory::LoadCollection)
  AnimList  m_anim_list;                    ///< слои анимации
  float     m_elepsed_time = 0;             ///< время жизни с момента создания
  float     m_time_last = 0;                ///< время последнего обновления
  float     m_time_fire_last = 0;           ///< время последнего выстрела
  float     m_delay_fire = 0.25;            ///< интервал стрельбы
  int       m_move_delay = 0;               ///< индикатор движения
  bool      m_move_flag = false;            ///< нажатие на газ
  bool      m_destroyed = false;            ///< готовность к удалению объекта
  state     m_state;                      
  tank_state m_tank_state = NORMAL;          
  std::function<void(void)> m_callback;     ///< вызывается при уничтожении
  
  static unsigned id_cnt;                   ///< отсчитывает уникальные номера
};


/// \brief интерфейс стратегии автономного поведения танков
/// 
/// Определяет контракт, по которому Tank обращается к пилоту, за указаниями
/// \see Tank::setAutoPilot
class TankDriverBase {
 public:
  TankDriverBase(Tank& tank) : m_tank(tank) {}
  virtual ~TankDriverBase() = default;
  virtual void ApplyControl(float ) = 0;
 
 protected:
  Tank& m_tank;
};


/// \brief Стратегия автономного поведения Tank по умолчанию
class TankDriver : public TankDriverBase {
 public:
  TankDriver(Tank& tank);
  void ApplyControl(float ) override;
  direction getRandomDir();               ///< генерирует случайное направление
 
 private:
  sf::Vector2f  m_pos;                    ///< прошлая позиция 
  direction     m_dir;
  const float   delta = 0.0001;           ///< критерий перемещения
  const float   m_fire_delay = 1;         ///< периодичность стрельбы
  float         m_time_fire_last = 0;     ///< последний выстрел
};


/// \brief разделяемая информация для подтипов экземпляров класса Bullet
/// \see Factory
struct BulletTypeInfo {
  std::string name;
  char type = 0;                          ///< подтип, 
  sf::Vector2f size;                      ///< размер прямоугольной области
  int speed = 0;                          ///< скорость полета
  int damage = 0;                         ///< урон
};

std::istream& operator>>(std::istream& is, BulletTypeInfo& item);
std::ostream& operator<<(std::ostream& is, const BulletTypeInfo& item);


/// \brief Снаряд
///
/// Описывает поведение снаряда в игре. 
/// Имеет подтипы, указанные в члене-данных char type_
///   подтип определяет параметры BulletTypeInfo включенные в состав объекта, как разделяемые данные.
///
/// Пример конфигурационного файла подтипов снаряда:
/// \include ..\resources\bullet.cfg
/// \see Tank::Fire
class Bullet : public GameEntity {
 public:
  using Info = BulletTypeInfo;                    ///< \copydoc Tank::Info
  using factory = Factory<Bullet, Info>;          ///< \copydoc Tank::factory
  using iterator = factory::iterator;             ///< \copydoc Tank::iterator
  friend factory;

  void Interaction(GameEntity&)  override;
  void Update(const sf::Time& )  override;
  void Draw(sf::RenderWindow& window) override;
  bool isDestroyed() const override { return m_destroyed; }

  void setPosition(sf::Vector2f pos) override;
  sf::Vector2f getPosition()  const override;
  sf::Vector2f getSize()    const override;

  void setStateDestruction( );                    ///< \copydoc Tank::setStateDestruction
  void setDirection(direction);

  // данные открываются для обработки взаимодействий
  friend void EntityInteraction(Tank&, Bullet&);
  friend void EntityInteraction(Bullet&, Bullet&);
  friend void EntityInteraction(Bullet&, Barrier&);
  friend void EntityInteraction(Bullet&, Bonus&);

 private:
  /// \copydoc Tank::Tank
  Bullet(char type, float time);
  /// \copydoc Tank::UpdatePosition
  void UpdatePosition(const sf::Time&);
  sf::Vector2f  m_pos;                          ///< координаты на игровом поле
  const int     m_id;                           ///< уникальный номер экземпляра
  const char    m_type;                         ///< подтип (\ref Factory::LoadCollection)
  AnimList      m_anim_list;                    ///< слои анимации
  direction     m_dir = FORWD;                  ///< направление движения
  float         m_time_last = 0;                ///< время последнего обновления
  bool          m_destroyed = false;            ///< готовность к удалению объекта
  state         m_state;

  static unsigned id_cnt;                       ///< отсчитывает уникальные номера
};


/// \brief разделяемая информация для подтипов экземпляров класса Barrier
/// \see Factory
struct BarrierTypeInfo {
  std::string name;
  char type = 0;                                ///< подтип, 
  sf::Vector2f size;                            ///< размер прямоугольной области
  int health = 0;                               ///< здоровье
  bool obstruct_Z_eq_0 = 0;                     ///< преграждает путь объектам с нулевой высотой
  bool obstruct_Z_greater_0 = 0;                ///< преграждает путь летящим объектам
};

std::istream& operator>>(std::istream& is, BarrierTypeInfo& item);
std::ostream& operator<<(std::ostream& is, const BarrierTypeInfo& item);


/// \brief Препятствия. 
///
/// Описывает цикл жизни и свойства препятствий на игровой карте 
/// (разрушаемые стены, неразрушаемые .., лес, ров и т.д.) 
/// Имеет подтипы, указанные в члене-данных char type_
///   подтип определяет параметры BarrierTypeInfo включенные в состав объекта, как разделяемые данные.
///
/// Пример конфигурационного файла видов препятствий:
/// \include ..\resources\barrier.cfg
class Barrier : public GameEntity {
 public:
  using Info = BarrierTypeInfo;                   ///< \copydoc Tank::Info
  using factory = Factory<Barrier, Info>;         ///< \copydoc Tank::factory
  using iterator = factory::iterator;             ///< \copydoc Tank::iterator
  friend factory;
  
  void Update(const sf::Time&)        override;
  void Draw(sf::RenderWindow& window) override;
  void Interaction(GameEntity&)       override;
  bool isDestroyed() const            override { return m_destroyed; }
  void setStateDestruction();                     ///< \copydoc Tank::setDestruction
  bool isTopDrawLayer() const;                    ///< отображается поверх основной сцены

  void  setPosition(sf::Vector2f pos) override;
  sf::Vector2f getPosition()    const override;
  sf::Vector2f getSize()        const override;
  
  friend void EntityInteraction(Tank&, Barrier&);
  friend void EntityInteraction(Bullet&, Barrier&);
private:
  ///< \copydoc Tank::Tank
  Barrier(char type, float time = 0);
  sf::Vector2f  m_pos;                           ///< координаты на игровом поле
  const int     m_id;                            ///< уникальный номер экземпляра
  const char    m_type;                          ///< подтип (\ref Factory::LoadCollection)
  sf::Sprite    m_sprite;                        ///< графическое представление
  bool          m_destroyed = false;             ///< готовность к удалению объекта
  static unsigned id_cnt;
};


/// \brief разделяемая информация для подтипов экземпляров класса Bonus
/// \see Factory
struct BonusTypeInfo {
  std::string name;
  char type = 0;                                  ///< подтип, 
  sf::Vector2f size;                              ///< размер прямоугольной области
  unsigned period;                                ///< период действия (или другая интерпретация)
};

std::istream& operator>>(std::istream& is, BonusTypeInfo& item);
std::ostream& operator<<(std::ostream& is, const BonusTypeInfo& item);


/// \brief Бонус
///
/// Имеет подтипы, указанные в члене-данных char type_
///   подтип определяет параметры BarrierTypeInfo включенные в состав объекта, как разделяемые данные.
/// 
/// Пример конфигурационного файла видов препятствий:
/// \include ..\resources\bonus.cfg
class Bonus : public GameEntity {
public:
  using Info = BonusTypeInfo;                     ///< \copydoc Tank::Info
  using factory = Factory<Bonus, Info>;           ///< \copydoc Tank::factory
  using iterator = factory::iterator;             ///< \copydoc Tank::iterator
  friend factory;

  void Interaction(GameEntity&)       override;
  void Update(const sf::Time&)        override;
  void Draw(sf::RenderWindow& window) override;
  bool isDestroyed() const override { return m_destroyed; }
  void DestroyThis();

  void  setPosition(sf::Vector2f pos) override;
  sf::Vector2f getPosition()    const override;
  sf::Vector2f getSize()        const override;
  
  void setStateDestruction();                ///< \copydoc Tank::setStateDestruction

  /// \copydoc Tank::setDestructionCallback
  void setDestructionCallback(std::function<void(void)> callback);
  
  friend void EntityInteraction(Bullet&, Bonus&);
private:
  /// \copydoc Tank::Tank
  Bonus(char type, float time = 0);
  sf::Vector2f    m_pos;                     ///< координаты на игровом поле
  const int       m_id;                      ///< уникальный номер экземпляра
  const char      m_type;                    ///< подтип (\ref Factory::LoadCollection)
  sf::Sprite      m_sprite;                  ///< слои анимации
  AnimList        m_anim_list;               ///< слои анимации
  bool            m_destroyed = false;       ///< готовность к удалению объекта
  std::function<void(void)> m_callback;      ///< вызывается при уничтожении

  static unsigned id_cnt;
};


void EntityInteraction(Tank&, Tank&);         ///< взаимодействия танка с танком

void EntityInteraction(Tank&, Bullet&);       ///< взаимодействие танка со снарядом

void EntityInteraction(Tank&, Barrier&);      ///< взаимодействие танка с препятствием

void EntityInteraction(Tank&, Bonus&);        ///< взаимодействия танка с игровым бонусом

void EntityInteraction(Bullet&, Bullet&);     ///< взаимодействия снаряда со снярядом

void EntityInteraction(Bullet&, Barrier&);    ///< взаимодействия снаряда с препятствием

void EntityInteraction(Bullet&, Bonus&);      ///< взаимодействия снаряда с игровым бонусом



//-----------------------------------------------------------------------------
// вспомогательные функции
inline sf::Vector2f MovingShift2D(float time, float speed, float angle) {
  float shift = time * speed;
  float dx = shift * cos(angle / 180.0f * M_PI);
  float dy = shift * sin(angle / 180.0f * M_PI);
  return sf::Vector2f(dx, dy);
}

inline void PrintVector(const sf::Vector2f& v) {
  using namespace std;
  cout << "x : " << v.x << ";\ty : " << v.y << endl;
}

inline void PrintRec(const sf::FloatRect& rec) {
  using namespace std;
  cout << "x : " << rec.left << ";\ty : " << rec.top
    << ";\tw : " << rec.width << ";\th :"<<rec.height << endl;
}


inline void printEntityStatistics(const std::string& label) {
  /* выводи состав хранилищ юнитов
  using namespace std;
  cout << endl << label << endl;
  cout << "Statistics : " << endl;
  cout << "GameEntity::g_entity_list.size() \t: " << GameEntity::g_entity_list.size() << endl;
  cout << "tanks \t\t: "  << Tank::factory::s_current_set.size() << endl;
  cout << "bullets \t: "  << Bullet::factory::s_current_set.size() << endl;
  cout << "barriers \t: " << Barrier::factory::s_current_set.size() << endl;
  cout << "bonus \t\t: "  << Bonus::factory::s_current_set.size() << endl;

  cout << "\n collection tanks \t: "  << Tank::factory::s_collection.size() << endl;
  cout << "collection bullets \t: "  << Bullet::factory::s_collection.size() << endl;
  cout << "collection barriers \t: "  << Barrier::factory::s_collection.size() << endl;
  cout << "collection bonus \t: "    << Bonus::factory::s_collection.size() << endl;
  */
}


// tests
void test_factory_battle_city();
