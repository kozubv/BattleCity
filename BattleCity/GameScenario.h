/// \file 
/// \ingroup battle_city_game_classes
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <utility>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include "SFML\Graphics.hpp"

#include "Units.h"

/// \brief информация о состоянии игровой сцены
struct GameInfo {
  sf::Vector2f player_pos;                ///< стартовая позиция игрока
  sf::Vector2f flag_pos;                  ///< позиция главного знамени
  std::vector<sf::Vector2f> enemy_ports;  ///< набор позиций появления соперников
  float   map_height = 0;                 ///< высота игрового поля
  float   map_width = 0;                  ///< ширина игрового поля
  int     lives = 1;                      ///< оставшиеся жизни игрока
  int     enemies = 19;                   ///< оставшиеся соперники
  bool    geme_over_flag = false;         ///< метка завершения игры
};


/// \brief Управляет ходом событий в течение битвы. Создает и настраивает соперников, бонусы и т.д.
/// 
/// Класс управления сценарием игры. Напрямую создает игровые единицы, и задает их исходные настройки.
/// - Настраивает действия для конкретных бонусов, подписываясь на их события (паттерн "Слушатель")
/// - Настраивает события при уничтожении соперников, подписываяь на события их унтожения (паттерн "Слушатель")
/// - Отслеживает состояние игрока, управляет счетчиком жизни.
/// - Отслеживает события завершения игры.
class GameScenario {
 public:
  /// инициализируется стартовыми настройками, загруженными из файла схемы карты
  GameScenario(GameInfo& init);

  /// \param time глобальное время битвы
  void Update(const sf::Time& time);
  
  /// применяет стартовые настройки для начального состояния битвы
  void Start( );

  /// сбрасывает текущее состояния для рестарта сценария
  void Stop();
  bool isGameOver() const;                ///< влаг завершения игры

  /// объект, управляемый угроком. (Сценарий имеет мозможность менять фукус управления в реализации)
  Tank& ControlledUnit() { return *m_player; }
  
  void CreatePlayer( );                 ///< создает юнит игрока меняя счетчик жизней. 
  void ApplyBonusClock();               ///< действие бонуса "Часы"
  void ApplyBonusBomb();                ///< действие бонуса "Бомба"
  void ApplyBonusStar();                ///< действие бонуса "Звезда" (нет реализации)
  void ApplyBonusTank();                ///< действие бонуса "Танк" (нет реализации)
  void ApplyBonusHelmet();              ///< действие бонуса "Шлем"

  void CommandGameOver();               ///< процедура завершения игры

  void EnemyDestructionEvents(int id);  ///< обработчик события уничтожения соперников

 private:
  /// создает юнит соперника и производит настройки его поведения
  /// \param time момент по часам игрового времени
  void CreateTank(float time);

  /// создает бонус заданного типа, в соответствие с файлами настроек
  void CreateBonus(char type);
  
  /// указатель на соотвтетсвующий элемент в хранилище Tank::iterator::s_current_set
  Tank::iterator m_player;

  /// текущее состояние игры
  GameInfo& m_info;
  
  std::vector<int> s_free_ports;        ///< набор совободных портов, где создаются соперники

  /// набор типов вражеских юнитов
  const std::vector<char> m_enemy_id      = { 'a', 'b', 'c', 'd' };

  /// набор типов танков игрока, с ссответствующими параметрами и отображением
  const std::vector<char> m_player_rang_id  = { 'e', 'f', 'g', 'h' };

  /// набор типов бонусов
  const std::vector<char> m_bonus_id      = { 'c', 'b', 's', 't', 'h', 'w'};

  /// набор действий для конкретных бонусов
  std::unordered_map<char, std::function<void(void)>> m_bonus_effect_map;

  float m_time_last = 0 ;            ///< момент последнего обновления
};


/// \example GameScenario.cpp
/// \brief Реализиция сценария игры. 
///
/// Этот класс непосредтвенно использует все возможности игровых сущностей. 
/// Правила игры и задаются здесь.