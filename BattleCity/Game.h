/// \file
/// \ingroup SGML_Game_Book
#pragma once
#include "Window.h"
#include "EventManager.h"
#include "StateManager.h"
#include "SharedContext.h"
#include <iostream>

/// \brief запускает бесконечный цикл обновления и отображения игры.
/// 
/// Управление режимами программы (главное меню, битва, статистика и 
/// переключение уровней)  построено на базе паттерна \a "Состояние".
/// Класс Game является реализацией компонента \a "Клиент" этого паттерна.
/// В реализации экземпляр Game обращается к экземпляру StateManager как к абстрактному
/// режиму, не зависимо от правил последовательности переключения и текущих режимов.
/// Для обмена данными между режимами (конкретными состояниями) Game агрегирует 
/// и экземпляр SharedContext.
class Game {
 public: 
  Game();
  ~Game();
  void Update();
  void Render();
  void LateUpdate();
  sf::Time    GetElapsed();                             ///< длительность времени, с момента последнего обновления
  Window*     GetWindow();                              ///< доступ к окну рендеринга

private:
  SharedContext m_context;                              ///< общий констекст паттерна State
  Window        m_window;                               ///< графическое окно
  // менеджер состояний, cкрывающий текущее состояние и абстрагирующий хитросплетения 
  // последовательности переключения режимов 
  StateManager m_stateManager;                          
  sf::Clock   m_clock;
  sf::Time    m_elapsed;
  void        RestartClock();                           ///< сброс глобального таймера
};


