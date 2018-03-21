/// \file 
/// \ingroup Visualisation
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <unordered_map>
#include <memory>
#include <utility>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <cmath>

#include <SFML\Graphics.hpp>

#include "SpriteManager.h"
#include "SupportTools.h"

/// \brief  содержит параметры описывающие правила воспроизведения анимации.   
///     
///  Класс предоставляет метод для считывания набора \a SpriteSheet 
///  из файла конфигурации. В статическом члене контейнере хранится набор
///  загруженных \a SpriteSheet. Элемент этого контейнера может быть 
///  запрошен для использования.
///  
///  \warning перед тем как запрашивать элементы, не забыть загрузить файл в 
///  статический контейнер класса.
///  
///  \note как и в большинстве классов проекта используется открытый статический
///  член. Конечено, это позволяет изменить набор его элементов извне. 
///  Осбождение памяти этого динамического контейнера возлагается на ОС, по 
///  завершении программы.
///  
///  \include sprite.sheet
///  \see AnimBase
struct SpriteSheetInfo {
  /// тип статического хранилища спрайтов
  using Collection = std::unordered_map<std::string, const SpriteSheetInfo>;
  SpriteSheetInfo() = default;
  const sf::Texture*  texture;
  std::string    name;
  sf::Vector2f   size;
  sf::Vector2u   origin_pos;       ///< точка начала изображения на текстуре
  float          frame_time = 0;   ///< период обновления кадров
  int            frame_num = 0;    ///< количество кадров
  bool           loop_flag = 0;    ///< флаг цикличности анимации

  /// \brief  Загружает файл с набором описаний \a SpriteSheet в контейнер 
  /// 
  /// Прядок записи параметров в файле:
  /// \param  file  путь и имя файла
  /// \exception    в случае неверной записи : invalid_argument(..);
  static void LoadSpriteSheetInfo(std::string file);

  /// \brief  Предоставляет доступ к элементу коллекции \a SpriteSheet
  /// \param  name  ник анимации \a SpriteSheetInfo::name;
  static const SpriteSheetInfo* getSheet(std::string name);

  /// \brief  Набор зарегистрированных \a SpriteSheetInfo
  static Collection s_collection;
};

std::ostream& operator<<(std::ostream& os, const SpriteSheetInfo& trg);


/// \brief базовый класс для Анимаций
/// 
/// Обеспечивает функциональность анимации по умолчанию. Для этого не нужен 
/// \a SpriteSheetInfo, можно передать обычный спрайт из SpriteManager и он 
///    мерцать.
/// Для наследников предоставляются общие методы декорирования.
/// 
/// \pre  перед использованием экземпляров, нужно выполнить 
///     SpriteSheetInfo::LoadSpriteSheetInfo.
///     Позиция вводится относительно установленной локальной системы отсчета \a s_local.
/// \warning  -т.к. инициализация не предоставляет sf::Drawable объекта, его не 
///       забыть перед использованием. 
/// \see AnimList
class AnimBase {
 public:
  AnimBase(float start_time);                     ///< требует глобального времени старта
  virtual ~AnimBase() = default;
  virtual void Update(float time);                ///< обновление согласно глобальному времени
  virtual bool IsCompleted() { return false; }    ///< флаг завершения для нецикличных анимаций 

  void Draw(sf::RenderWindow& window);
  void setSprite(const std::string& name);
  void setSpriteSheet(const std::string& name);
  void setOrientation(float angle);               ///< угол в град, по часовой стрелке
  void setPosition(sf::Vector2f pos);             ///< \param pos точка в локальной системе

  /// \brief  устанавливает мерцание изображения
  /// \param  period  период 
  /// \param  cl      цветовая маска 
  void setBlink(float period, sf::Color cl = sf::Color::Transparent);

  /// \brief  возвращает имя хранимой анимации (или спрайта)
  const std::string& getName() const { return m_name; }

  /// \brief  локальная система отсчета позиций (только смещенная от системы окна)
  static sf::Vector2f s_local;

 protected:
  void UpdateBlink(float time);

  const SpriteSheetInfo*  m_info;
  sf::Sprite     m_sprite;                 ///< sf::Drawable 
  float          m_time_last = 0;          ///< момент последнего обновления
  std::string    m_name;
 
private:
  sf::Color      m_color;
  float          m_time_blink_last = 0;    ///< момент последнего мерцания
  float          m_blink_period = 0;       
  float          m_time_blink = 0;         ///< время с последнего мерцания
  bool           m_blink_switch = 0;       ///< отслеживает применение мерциния
};


/// \brief  Однократно проигравающаяся анимация.
/// 
/// После завершения, продолжает отображаться последний кадр, поэтому 
/// анимацию нужно явно удалить из списка отображения.
class AnimSingle : public AnimBase {
 public:
  /// \param  time_start  глобальное время старта
  /// \param  sheet      ник анимации из \a SpriteSheetInfo
  AnimSingle(float time_start, const std::string& sheet);
  void Update(float time) override;
  virtual bool IsCompleted() { return f_stop; }

 private:
  using AnimBase::setSprite;
  int m_current_frame = -1;
  bool f_stop = false;
};


/// \brief   Циклически проигрывающаяся анимация.
/// 
/// Проигрывает кадры в последовательном цикле. 
/// - есть возможность ограничить время проигрывания \a setTimer()
class AnimLoop : public AnimBase {
 public:
  /// \param  time_start  глобальное время старта
  /// \param  sheet      ник анимации из \a SpriteSheetInfo
  AnimLoop(float time_start, const std::string& sheet);
  void Update(float time) override;
  virtual bool IsCompleted() { return f_stop; }

  /// \brief  задает конечную продолжительность воспроизведения
  /// \param timer продолжительность воспроизведения [сек]
  void setTimer(float timer);

 private:
  using AnimBase::setSprite;
  int m_current_frame = -1;
  bool f_stop = false;
  float m_timer = 1e20;
  float m_timer_start = 0;
};


/// \brief Комбинированная анимация.
///  
/// Предоставляет возможность управления набором анимаций, добавленных в набор.
/// Очередность отображения, обуслоаливает слои отображения. 
/// Дополнитлельную анимацию можно добавить в начало и конец набора.
/// После завершения, конкретная анимация удаляется из набора воспроизведения.
/// Переопределенные методы класса, действуют на набор как на единое целое. 
/// Экземпляр класса принимает полностью принимает владение елементами. 
///
/// Пример использования: \snippet Animating.cpp demo_code_animating
/// 
/// \see unique_anim, AnimLoop, AnimSingle
class AnimList {
 public:      
  /// \brief тип хранимого элемента элементарной анимации
  using element = std::unique_ptr<AnimBase>;
  void Update(float time);
  void Draw(sf::RenderWindow&);
  void Clear() { m_list.clear(); }            ///< очищает набор

  void setPosition(sf::Vector2f);
  void setOrientation(float);

  /// удаляет анимацию по атрибуту \a AnimBase::name, если таковой нет в наборе,
  /// набор остается неизменен
  void Remove(const std::string&);            
  element& InsertBack(element&&);              ///< вставка поверх всех
  element& InsertFront(element&&);            ///< вставка позади всех
  bool IsCompleted() const { return m_list.empty(); }
  
  /// если имя не присутствует в наборе, возвращается \a false
  bool IsCompleted(const std::string&);
 
 private:
  std::list<element> m_list;
};


/// \brief  аналог \a std::make_shared с параметрами, подходящими для AnimLoop и AnimSingle
///
/// \tparam  T  тип с конструктором : typename::typename(float time, const std::string& name)
///
/// \return  r-value ссылку для перемещения, созданного указателя.
template <typename T>
std::unique_ptr<T> unique_anim(float time, const std::string& name) {
  return std::unique_ptr<T>(new T(time, name));
}


void test_animating();
