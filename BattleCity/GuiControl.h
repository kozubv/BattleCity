/// \file 
/// \ingroup Visualisation
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <unordered_map>
#include <utility>

#include <SFML\Graphics.hpp>

#include "SpriteManager.h"
#include "SupportTools.h"

/// \brief "Component" паттерна "Компоновщик" для визуальных элементов
/// 
/// При наличии необходимости раполагать графические элементы программы, согласно 
/// иерархии принадлежности обычно рекомендуется использовать паттерн "компоновщик".
/// класс \a VisualBase предоставляет интерфейс, общения с абстрактными графическими элементами.
///
/// \see VisualPanel, SpriteTable, CharDisplay
class VisualBase {
 public:
  VisualBase() {};
  VisualBase(VisualBase& )            = delete;
  VisualBase& operator=(VisualBase& ) = delete;
  virtual ~VisualBase() {};

  virtual void Draw(sf::RenderWindow&) const  = 0;
  virtual void setPosition(float x, float y)  = 0;
  virtual void setOrigin(float x, float y)    = 0;
  virtual sf::FloatRect getBounds() const     = 0;
  
  sf::Vector2f getBoundsSize() const {          ///< Размер w, h, прямоугольника
    return {getBounds().width, getBounds().height }; 
  }
};


/// \brief элемент "Composite" паттерна "Компоновщик" для визуальных элементов
/// 
/// Добавляемые элементы выравниваются по ячейкам прямоугольной таблицы. 
/// Привязка происходит к ячейкам таблицы, с заданными при инициализации размерами ячеек.
/// Управляет набором спрайтов, при этом не является их владельцем. т.е. за создание
/// и удаление не отвечает.
/// -  
/// - (main window coordinate sistem)
/// -   |
/// -   V    (origin x)
/// -   *===========.=================================================
/// -   =      .    таблица позиций
/// -   =      .    0        1          ... size.y
/// - (origin y)......*------------------------------------------------
/// -   =    0  | {VisualBase*}  |  {VisualBase*}  | {ячейка}  |
/// -   =      -------------------------------------------------
/// -   =    1  | {VisualBase*}  |  {VisualBase*}  |      |
/// -    =           -------------------------------------------------
/// -   =          ...              ...
/// -   =      -------------------------------------------------
/// -   =  size.x  |        |          |      |
/// -   =      -------------------------------------------------
///
/// -при инициализации указывается:
///    - кол-во линий и колонок таблицы в size
///    - размер сторон ячейки (cell_sz) определяет дистанцию между элементами.
///  
/// see SpriteTable
class VisualPanel : public VisualBase {
 public:
  /// \param size    размер таблицы
  /// \param cell_sz  размер ячейки таблицы (w, h дистанции)
  /// \param border  ширина рамки по периметру 
  VisualPanel(sf::Vector2u size, sf::Vector2f cell_sz, float border = 0);

  void Draw(sf::RenderWindow&) const  override;
  void setPosition(float x, float y)  override;
  void setOrigin(float x, float y)  override;
  sf::FloatRect  getBounds() const  override;
  
  /// \brief добавить в набор таблицы и сменить позицию, согласно 
  /// \warning при добавлении, если уже имеется элемент в указанной позиции, 
  /// размещенные через void Place(VisualBase* lief, sf::Vector2u line_column), тот не удаляется
  void Place(VisualBase* lief);

  /// \brief добавить в набор таблицы и сменить позицию, согласно позиции
  /// \param line_column  {строка, столбец} позиции в таблице
  /// \note при добавлении, если уже имеется элемент в указанной позиции, тот удаляется.
  void Place(VisualBase* lief, sf::Vector2u line_column);
  void setColor(sf::Color clr);
  sf::Vector2f getOrigin() const;              ///< точка системы координат панели

 private:
  /// тип хранилища включаемых элементов
  using Contents = std::unordered_map<int, VisualBase* >;
  sf::RectangleShape    m_backg;            ///< прямоугольник отображения фона панели
  sf::Vector2u          m_sz;              
  sf::Vector2f          m_cel_sz;
  Contents              m_items;            ///< хранилище включаемых элементов
  float                 m_brd;  
  unsigned              m_cnt = 0;            ///< курсор на следующую ячейку 
};


/// \brief элемент "Leaf" паттерна "Компоновщик" для визуальных элементов
/// 
/// Обладает правилами позиционирования аналогичными \a VisualPanel. \ref VisualPanel
/// Управляет набором спрайтов, при этом не является их владельцем. т.е. за создание
/// и удаление не отвечает.
///
/// \see VisualPanel
class SpriteTable : public VisualBase {
 public:
  /// \param size    размер таблицы
  /// \param cell_sz  размер ячейки таблицы (w, h дистанции)
  SpriteTable(const sf::Vector2u& size, const sf::Vector2f& cell_sz);

  void Draw(sf::RenderWindow&) const  override;
  void setPosition(float x, float y)  override;
  void setOrigin(float x, float y)  override;
  sf::FloatRect  getBounds() const  override;

  /// \brief добавить в набор таблицы и сменить позицию, согласно 
  /// \warning при добавлении, если уже имеется элемент в указанной позиции, 
  /// размещенные через void Place(VisualBase* lief, sf::Vector2u line_column), тот не удаляется
  void Place(sf::Sprite*);

  /// \brief добавить в набор таблицы и сменить позицию, согласно позиции
  /// \param line_column  {строка, столбец} позиции в таблице
  /// \note при добавлении, если уже имеется элемент в указанной позиции, тот удаляется.
  void Place(sf::Sprite*, sf::Vector2u);

  void Clear() { m_items.clear(); m_cnt = 0; };      ///< очищает хранилице элементов
  void setColor(sf::Color cl) { m_backg.setFillColor(cl); };
  // getters
  sf::Vector2u getSize()      const { return m_sz; }      ///< размер таблицы
  sf::Vector2f getCellSize()  const { return m_cel_sz; }  ///< размер клетки
  unsigned getCount()         const { return m_cnt; }
  sf::Vector2f getPosition()  const { return m_backg.getPosition(); }
  
  /// \brief вствка без изменения локального положения. меняется только начало отсчета
  /// на Origin содержащей панели.
  /// \param cnt позиция в таблице, с которой будет ассоциирован этот элемент
  void Insert(sf::Sprite*, unsigned cnt);

  /// \brief удалить элемент из конца
  void PopBack();

 private:
  /// тип хранилища включаемых элементов
  using Contents = std::unordered_map<int, sf::Sprite* >;
  sf::RectangleShape  m_backg;
  sf::Vector2u        m_sz;
  sf::Vector2f        m_cel_sz;
  Contents            m_items;
  unsigned            m_cnt = 0;
};


/// \brief подобие символьного дисплея в окне
/// 
/// Отображающего на экране строку через ассоциированные символам изображения.
/// \note отображает : a-z, 1-9, +, -, *, /, >, <, |
///    Самостоятельно создает и удаляет элементы.
///
/// Пример использование : \snippet GuiControl.cpp demo_code_char_display
/// \see SpriteManager, VisualPanel
class CharDisplay : public SpriteTable {
 public:
  /// \param size    {line, column} символьного поля
  /// \param cell_sz  {горизонтальный отступ символов, вертикальный}
  CharDisplay(sf::Vector2u size, sf::Vector2f cell_sz);

  /// \brief пробразует строку в изображения
  CharDisplay& operator<<(const std::string& s);
  void Clear();                      ///< очистка дисплея
  void setBckgColor(sf::Color cl) { SpriteTable::setColor(cl); } ///< цвет фона
  CharDisplay& setColor(sf::Color clr = sf::Color::White);///< цветовая маска

 private:
  using Resources = std::list<sf::Sprite>;
  using SpriteTable::Place;
  Resources m_buffer;
  std::string m_str = "";
  sf::Vector2u m_sz;
  sf::Vector2u csr = {0,0};
  sf::Color m_clr;
};


void test_GuiControl();
