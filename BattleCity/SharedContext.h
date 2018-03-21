/// \file
/// \ingroup SGML_Game_Book
#pragma once
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

#include "Window.h"
#include "EventManager.h"

/// \brief доступ к общим данным режимов программы
/// 
/// 
/// \exception при инициализации экземпляра, производится чтение файла конфигураций,
/// что может вызывать исключения.
/// \see Game, BaseState, StateManager
class SharedContext {
 public:
  /// набор названий уровней и их карт
  using StageList = std::vector<std::pair<std::string, std::string>>;
    
  /// набор путей ко всем конфигурационным файлам
  using PathsMap = std::unordered_map<std::string, std::string>;

  /// \param paths_file путь к файлу
  SharedContext(const std::string & paths_file);

  /// графическое окно ОС
  Window* m_wind;
    
  /// обработчик событий пользовательского управления
  EventManager* m_eventManager;

  /// \brief загрузка списка путей к файлам данных
  /// 
  /// Пример содержимого такого файла:
  /// \include ..\resources\paths.cfg
  void LoadConfigFiles(const std::string &);
    
  /// \brief загрузка списка карт игры
  /// 
  /// Пример содержимого файла:
  ///     # level map list
  ///     # stage_name| failename.map
  ///     stage-first    ..\resources\stage1.map
  ///     ...
  void LoadStageList(const std::string & );
  
  
  /// номер текущего уровня
  int getStageNum(int num) const;
    
  /// список всех уровней и их схем-карт
  const StageList& getStageList() const { return map_list;  }
    
  /// номер текущего уровня
  const int getStageNum()const { return getStageNum(m_cnt);}
  
  /// назначет первый в списке уровень текущим
  void ResetStage() { m_cnt = 0; }
    
  /// имя текущего уровня
  const std::string& CurrentStageName()  { return map_list[getStageNum()].first; }
    
  /// файл схемы карты текущего уровня
  const std::string& CurrentStageFile()  { return map_list[getStageNum()].second; }

  /// назначить следующий в списке уровень текущим - и вернуть имя уровня
  const std::string& NextStageName()    { return map_list[getStageNum(++m_cnt)].first; }
  
  /// назначить следующий в списке уровень текущим - и вернуть путь к файлу схемы карты
  const std::string& NextStageFile()    { return map_list[getStageNum(++m_cnt)].second; }
  
  /// назначить предыдущий в списке уровень текущим - и вернуть имя уровня
  const std::string& PreviousStageName()  { return map_list[getStageNum(--m_cnt)].first; }
  
  /// назначить предыдущий в списке уровень текущим - и вернуть путь к файлу схемы карты
  const std::string& PreviousStageFile()  { return map_list[getStageNum(--m_cnt)].second; }

  /// путь к файлу, указанному по уникальному имени
  /// \param name идентификационное имя фала данных
  static const std::string& getFilePath(const std::string& name);

private:
  StageList map_list;
  /// курсор передвижений по списку уровней
  int m_cnt = 0;
  static PathsMap m_paths_map;
};



















