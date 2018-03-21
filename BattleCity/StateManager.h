/// \file
/// \ingroup SGML_Game_Book
#pragma once
#include <vector>
#include <unordered_map>

#include "SharedContext.h"

#include "State_TanksMenu.h"
#include "State_LevelSelection.h"
#include "State_BattleReport.h"
#include "State_Battle.h"

/// \warning первому члену должно быть задано значение больше 0. 
/// т.к. StateType(0) используется для глобального состояния. 
enum class StateType { 
  Battle = 1,     ///< битва
  BattleReport,   ///< статистика битвы
  TanksMenu,      ///< главноое меню игры
  LevelSelection  ///< выбор карты
};

/// \brief хранилище созданных состояний, 
using StateContainer = std::vector<std::pair<StateType, BaseState*>>;

/// \brief контейнер типов состояний (активных на текущий момент)
using TypeContainer = std::vector<StateType>;

/// \brief фабрика состояний
using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;

/// \brief Предоставляем методы контроля, регистрации и смены режимов программы
///
/// Выступает в роли контроллера правил последовательной смены состояний программы
/// и спользуется при их: создании, удалении, активации и деактивации. 
/// В конкретной м случае класс \a StateManager является реализацией компонента
///  \a "Контекст" паттерна "Состояние". 
/// Имеет интерфейс, через который "Клиент" (Game) обращается к режимам.
///
/// \note 
/// \par Данная реализация допускает наличие параллельных состояний, набор которых изменяем.
/// Для обозначения этого набора дальше в описании используется термин 
/// "список исполняемых состояний". 
/// \par общий интерфейс состояний BaseState предоставляет возможность управления свойствами 
///  прозрачности и параллельности ( \a StateBase::SetTranscendent
///  и \a StateBase::SetTransparent). StateManagers осуществляет обновления с учетом этих свойств.
///
/// \see BaseState, Game
class StateManager {
 public:
  /// требует доступ к хранилищу открытых данных
  /// \param l_shared общие данные состояний
  StateManager(SharedContext* l_shared);
  ~StateManager();
    
  void Update(const sf::Time& l_time);                ///< часть интерфейса обращения к BaseState
  void Draw();                                        ///< часть интерфейса обращения к BaseState
    
  /// удаление всех состояний 
  void ProcessRequests();
    
  /// доступ к общим данным
  SharedContext* GetContext();
    
  /// запрос наличия указанного состояния в списке исполняемых
  bool HasState(const StateType& l_type);
    
  /// помещает состояние в конце списка исполняемых состояний.
  /// Перед размещением, вызывается :
  ///     метод StateBase::Deactivate() предъдущего состояния
  ///     метод StateBase::Activate() нового
  void SwitchTo(const StateType& l_type);
    
  /// удаляет состояние из списка исполняемых состояний
  /// Перед удалением, вызывается : метод StateBase::onDestroy() 
  void Remove(const StateType& l_type);

 private:
  /// \brief создает и размещает в списке исполняемых состояний новое \n
  /// Перед размещением, вызывается : метод StateBase::onCreate() 
  void CreateState(const StateType& l_type);  
    
  /// \brief фактичесики удаляет состояния назначенные к удалению из списка исполняемых состояний \n
  /// Перед удалением, вызывается : метод StateBase::onDestroy() 
  void RemoveState(const StateType& l_type);

  /// добавляет метод создания конкретного состояния в фабрику
  template<class T>
  void RegisterState(const StateType& l_type) {
    m_stateFactory[l_type] = [this]() -> BaseState* {
      return new T(this);
    };
  }

  /// открытые данные между состояниниями
  SharedContext* m_shared;
    
  /// списка исполняемых состояний на текущем шаге состояний (т.е. параллельных)
  StateContainer m_states;
    
  /// список назначенных к удалению
  TypeContainer m_toRemove;
    
  /// \brief Фабрика конкретных состояний
  ///
  /// \note является реализацией паттерна "Фабрика"
  StateFactory m_stateFactory;
};










