/// \file 
/// \ingroup game_modes
#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <list>
#include <unordered_map>
#include <utility>
#include <stdexcept>
#include <memory>

#include "BaseState.h"
#include "EventManager.h"
#include "StateManager.h"

#include "SpriteManager.h"
#include "GuiControl.h"
#include "GameBattleCity.h"

class InfoPanel;

/// \brief Режим битвы.
///
/// Основной режим игры. Управляет запуском игровой модели BattleCity и настраивает 
/// отображение игрового поля и информационной панели в графическом окне.
/// Настраивает связь событий пользовательского управления с командами управления игрой
class State_Battle : public BaseState {
 public:
  State_Battle(StateManager* l_stateManager);

  void OnCreate()     override;
  void OnDestroy()    override;
  void Activate()     override;
  void Deactivate()   override;
  void Update(const sf::Time& l_time) override;
  void Draw()         override;

  void Callback_Up(EventDetails* l_details);      ///< нажатие соответствующей кнопки
  void Callback_Down(EventDetails* l_details);    ///< нажатие соответствующей кнопки
  void Callback_Left(EventDetails* l_details);    ///< нажатие соответствующей кнопки
  void Callback_Right(EventDetails* l_details);   ///< нажатие соответствующей кнопки
  void Callback_Space(EventDetails* l_details);   ///< нажатие соответствующей кнопки
  void Callback_Pause(EventDetails* l_details);   ///< нажатие соответствующей кнопки
  void Callback_Esc(EventDetails* l_details);     ///< нажатие соответствующей кнопки

 private:
  ///выравнивает совместное отображение игрового поля и информационной панели по центру
  void Align();

  /// экземпляр игровой модели
  std::unique_ptr<BattleCity> m_battle;
  std::unique_ptr<InfoPanel>  m_panel;
  sf::RectangleShape m_bkg;
};


/// \brief правая информационная панель в режиме битвы
/// 
/// Отображает количество оставшихся врагов, количество жизней игрока 
/// и текущий уровень (порядковый номер карты)
class InfoPanel {
 public:
  InfoPanel(sf::Vector2f cell);
  ~InfoPanel();

  /// обновляет значения отображаемых параметров
  /// \param cnt    количество оставшихся врагов
  /// \param lives  жизни игрока
  /// \param stage  номер уровня
  void UpdateInfo(int cnt, int lives, int stage);
  void Draw(sf::RenderWindow& window);

  void setPosition(float x, float y);
  
  /// размер прямоугольной области отображения
  sf::FloatRect getBounds();

 private:
  SpriteTable* m_enemy_cnt;       ///< счетчик вражеских танков
  SpriteTable* m_live_sp;         ///< таблица жизней
  SpriteTable* m_stage_sp;        ///< таблица уровней  

  VisualPanel* m_live_cnt;        ///< счетчик жизней
  VisualPanel* m_stage;           ///< номер уровня
  VisualPanel* m_main;            ///< главная понель

  CharDisplay* dsp_lives;         ///< символьный дисплей для жизней
  CharDisplay* dsp_stage;         ///< символьный дисплей для номера уровня

  std::list<sf::Sprite> m_sprites;///< хранилище отображаемых спрайтов
  const sf::Vector2f    m_cell_sz;///< размер яцейки таблицы выравнивания элементов
  sf::Color m_clr;                ///< цвет фона панели
  sf::Color m_txt_clr;            ///< цвет шрифта в дисплеях

  int i_enemies = 0;
  int i_lives = 0;
  int i_stage = 0;
};
