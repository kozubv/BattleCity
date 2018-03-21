/// \file 
/// \ingroup game_modes
#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "BaseState.h"
#include "SpriteManager.h"
#include "EventManager.h"
#include "StateManager.h"
#include "GuiControl.h"


class LevelDisplay;

/// \brief ѕозвол¤ет выбрать карту дл¤ битвы.
/// 
///  клавиши Up и Down позвол¤ют перебирать список доступных карт.
/// список карт хранитс¤ в экземпл¤ре SharedContext, доступного
/// через защищенный член BaseState.
///
/// \note перед использованием режим должен быть зарегистрирован при помощи
///  StateManager::RegisterState
class State_LevelSelection : public BaseState {
 public:
  /// при создании, экземпл¤р св¤зываетс¤ с менеджером состо¤ний
  State_LevelSelection(StateManager* l_stateManager);
  ~State_LevelSelection();

  void OnCreate()       override;
  void OnDestroy()      override;
  void Activate()       override;
  void Deactivate()     override;
  void Update(const sf::Time& l_time) override;
  void Draw()           override;

  /// прокручивает список карт вперед
  void ScrollUp(EventDetails* l_details);

  /// прокручивает список карт назад
  void ScrollDown(EventDetails* l_details);

  /// запускает режим битвы на выбранной карте
  void CallbackReturn(EventDetails* l_details);

 private:
  /// визуальна¤ панель отображени¤ строки символов с названием
  LevelDisplay* m_display;
};


/// \brief визуальна¤ панель отображени¤ строки символов с названием выбранной карты
/// отображает : a-z, 1-9, +, -, *, /, >, <, |
class LevelDisplay {
 public:
  /// \param wind  размеры примоугольного окна, отображени¤
  /// \param len  длина отображаемой строки символов
  LevelDisplay(sf::Vector2u wind, unsigned len = 15);

  /// обновляет сожедржимое строки
  /// \param s  отображаема¤ строка
  void Update(const std::string& s);
  void Draw(sf::RenderWindow&);

  sf::FloatRect getBounds() const;            ///< границы отображени¤
  void setPosition(float x, float y);
  sf::Vector2f getFontSize();                ///< размер изображени¤ одного символа

 private:
  const sf::Vector2f s_cell_sz = {16, 16};        ///< размер ячеек в в дисплее
  CharDisplay m_display;
  
};


