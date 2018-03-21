/// \defgroup game_modes Отдельные режимы игры
/// \brief Слабосвязанные между собой состояния, которые принимает программа в ходе работы
/// Меню, отчет о результатах битвы, сама битва и т.д.

/// \file 
/// \ingroup game_modes
#pragma once
#include "BaseState.h"
#include "SpriteManager.h"
#include "EventManager.h"
#include "StateManager.h"

/// \brief Режим главного меню. Выбор битвы или выход из программы.
/// 
/// Режим программы по умолчанию. 
/// Дает возможность перейти к выбору уровня или выйти из программы.
/// Отображает логотип, курсор и пункты меню. 
///
/// \note перед использованием режим должен быть зарегистрирован при помощи
/// StateManager::RegisterState
/// 
class State_TanksMenu : public BaseState {
  enum MenuChoice { 
    PLAY = 0,   ///< битва
    EXIT = 1    ///< выход
  };
  
 public:
  /// при создании, экземпляр связывается с менеджером состояний
  State_TanksMenu(StateManager* l_stateManager);

  void OnCreate()     override;
  void OnDestroy()    override;
  void Activate()     override;
  void Deactivate()   override;
  void Update(const sf::Time& l_time) override;
  void Draw()         override;

  /// поднимает курсор вверх по элементам меню
  void CallbackUp(EventDetails* l_details);

  /// опускает курсор вних по элементам меню
  void CallbackDown(EventDetails* l_details);

  /// переключает режим на выбранный при помощи курсора
  void CallbackReturn(EventDetails* l_details);

private:
  sf::Sprite m_label;                     ///< фоновая надпись
  sf::Sprite menu_item_1_;
  sf::Sprite menu_item_2_;
  sf::Sprite m_cursor;                    ///< изображение курсора
  MenuChoice menu_choice = PLAY;          ///< текущий выбор курсором
};


void test_State_TanksMenu();









