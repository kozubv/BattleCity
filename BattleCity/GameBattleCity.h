/// \defgroup battle_city_game_classes Описание юнитов, карты и сценатрия игры
/// \brief Непосредственное описание игровых сущностей и все что с ними связано

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
#include <algorithm>

#include "SFML\Graphics.hpp"


#include "Units.h"
#include "EntityManager.h"
#include "Animating.h"
#include "GameScenario.h"
#include "EntityBase.h"

/// \brief Занимается загрузкой карты и созданием игрового поля,
///   загрузкой файлов параметров игровых объектов
///
/// - Задает параметры игровых объектов для текущей битвы, загружая их их файлов перед началом
/// - Задает набор анимаций для игровых объектов, загружая его из файла
/// - Синхронизирует работу обработчика взаимодействий объектов (EntityManager) и 
///    игрового сценария (GameScenario).
/// - Настраивает область отображения анимации и игровое пространство,
///    согласно размеру игрового поля и его позиции в графическом окне.
/// - Сбрасывает состояние игры до исходного при рестарте
/// - Передает события управления в игровой сценарий
///
/// \note для обновления игры используется собственный экземпляр часов sf::Clock, 
///    это дает возможность учитывать задержки в операциях на каждом шаге.
class BattleCity {
 public:
  /// при создании экземпляра производится обращение к данным файлов, что может вызывать исключения
  /// имена фалов параметров игровых объектов и их анимаций заданы в коде реализации ( это минус )
  BattleCity();
    
  /// \param time глобальное время, непрерывное в ходе работы, с нулем на старте битвы
  void Update(const sf::Time& time);
  void Draw(sf::RenderWindow& window);

  /// параметры игровой сцены на текущем шаге
  const GameInfo& getInfo() { return info_; }

  /// Загружает файл карты, генерирует игровое поле, передает считанные настройки в сценарий
  /// \param map_file_name файл карты игрового поля
  void Start(const std::string& map_file_name);
  
  void Stop();                            ///< очистка состояния для рестарта
  void Player_up();
  void Player_down();
  void Player_left();
  void Player_right();
  void Player_fire();

  /// загружает файл-карту построчно в набор строк
  void LoadMapScheme(const std::string& map_file_name);

  /// генерирует игровые объекты по значению символов в строках
  void ConvertMapScheme(const std::vector<std::string>& map_scheme);

  /// устанавливает начало системы координат области
  /// визуализации в начало поля отображения сцены
  void setFieldOrigin(sf::Vector2f pos);

 private:
  /// создает границы игрового поля, за которые не могу выйти подвижные юниты
  void CreateMapBorders();

  EntityManager m_entity_manager;
  GameInfo      info_;
  GameScenario  m_scenario;

  sf::Vector2f m_field_origin;  ///< начало координат поля отображения относительно окна
  sf::Clock m_clock;
};

void test_game_battle_city();
