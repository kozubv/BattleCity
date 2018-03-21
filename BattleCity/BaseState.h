/// \file
/// \ingroup SGML_Game_Book
#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

/// \brief общий интерфейс режимов программы. компонент "базовый интерфейс состояний" паттерна "Состояние"
/// 
/// Для обеспечения наличия независимых режимов в программе, возможности их
/// отдельной разработки и комбинирования в порядке взаимодействия применен паттерн State.
/// 
/// \see StateManager
class BaseState {
  friend class StateManager;
 public:
  /// \param l_stateManager указатель на экземпляр "Контекста" управления режимами
  BaseState(StateManager* l_stateManager) 
        : m_stateMgr(l_stateManager), m_transparent(false), m_transcendent(false) {}
  
  virtual ~BaseState() {}
    
  /// вызывается единожды при создании экземпляра.
  /// \see StateManager
  virtual void OnCreate() = 0;
    
  
  /// вызывается единожды при удалении экземпляра 
  /// \see StateManager
  virtual void OnDestroy() = 0;

  /// вызывается единожды при добавлении в список исполняемых состояний
  /// \see StateManager
  virtual void Activate() = 0;
    
  /// вызывается единожды при удалении из в список исполняемых состояний
  /// \see StateManager
  virtual void Deactivate() = 0;

  /// param l_time время с момента последнего обновления
  virtual void Update(const sf::Time& l_time) = 0;
  virtual void Draw() = 0;

  /// если true, при отображении так же отображается и состояние, идущее до данного в 
  /// списке исполняемых состояний
  /// \see StateManager::Draw
  void SetTransparent(const bool& l_transparent);
  bool IsTransparent() const ;
    
  /// если true, при обновлении так же отображается и состояние, идущее до данного в 
  /// списке исполняемых состояний
  /// \see StateManager::Update
  void SetTranscendent(const bool& l_transcendence);
  bool IsTranscendent()const;

  StateManager* GetStateManager();

 protected:
  StateManager* m_stateMgr;
    
  bool m_transparent;
    
  bool m_transcendent;
};


inline void BaseState::SetTransparent(const bool& l_transparent) {
  m_transparent = l_transparent;
}

inline bool BaseState::IsTransparent()const {
  return m_transparent;
}

inline void BaseState::SetTranscendent(const bool& l_transcendence) {
  m_transcendent = l_transcendence;
}

inline bool BaseState::IsTranscendent()const {
  return m_transcendent;
}

inline StateManager* BaseState::GetStateManager() {
  return m_stateMgr;
}

