/// \file 
/// \ingroup game_modes
#pragma once

#include <array>
#include <list>
#include <string>
#include <sstream>
#include <iomanip>

#include "BaseState.h"
#include "SpriteManager.h"
#include "EventManager.h"
#include "StateManager.h"
#include "GuiControl.h"

/// количество уничтоженных танков каждого типа. тип по порядку
using Points = std::array<unsigned, 4>;

/// бонусные очки за уничтоженую единицу. Тип по позиции в массиве
const int EnemiIdReward[4] = {100, 200, 300, 400};

/// данные отображаемые на дисплее отчета
struct BattleReportData {
  int hi_score = 0;             ///< лучший результат
  int stage = 0;                ///< номер уровня
  int player1 = 0;              ///< счет игрока
  Points pts = {0, 0, 0, 0};    ///< счетчик уничтожений по типу 
};


/// \brief визуальная панель отображения таблицы результатов битвы
/// 
/// Выводит на экран отчет в виде таблицы с заполенными полями параметров
/// \note 
class ReportTable {
 public:
  /// инициализируется размерами символьного дисплея
  /// \param size size.x - кол-во строк, size.y - кол-во символов в строке) 
  ReportTable(sf::Vector2u size);

  void UpdateInfo(const BattleReportData& data);
  void Draw(sf::RenderWindow& window);
  void Update(const sf::Time& l_time);

 private:
  ///std::string ReportLine(int cnt, int reward);
  CharDisplay m_display;
  sf::Vector2f m_window_sz;

  /// форматированная строка отчета
  std::string  report_string_;
  // title
  static const sf::Vector2u s_table_size;
  static const sf::Vector2f s_sz;
};


/// \brief отображает на экране статистику битвы
/// \note перед использованием режим должен быть зарегистрирован при помощи
///  StateManager::RegisterState
class State_BattleReport : public BaseState {
 public:
  State_BattleReport(StateManager* l_stateManager);

  void OnCreate()     override;
  void OnDestroy()    override;
  void Activate()     override;
  void Deactivate()   override;
  void Update(const sf::Time& l_time) override;
  void Draw()         override;

  /// запускает битву на следующей карте, или переходит в главное меню, если
  /// последняя карта была последней в списке
  void NextMode(EventDetails* l_details);

 private:
  ReportTable m_report_table;
};
