/// \defgroup SGML_Game_Book Заимстованый исходный код "SFML Game Development"
///  \brief Набор классов, взятых из code example к книге "SFML game development book".(главы 1-5) \n
/// https://www.packtpub.com/game-development/sfml-game-development

/// \file
/// \ingroup SGML_Game_Book
#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

/// типы событий пользовательского управления
///
/// KeyDown-TextEntered - стандартное перечисление типов библиотеки SFML
/// для единого образа обработки событий sf::Event и "Событий реального времени" классов
/// sf::Keyboard, sf::Mouse и sf::Joystick. 
/// Преобразование к единой форме идентификации событий позволяет однообразно настраивать 
/// кнопки пользовательского ввода в текстовом файле (EventManager::LoadBindings)
///
/// \see sf::Event, sf::Keyboard, sf::Mouse
enum class EventType {
  KeyDown       = sf::Event::KeyPressed,
  KeyUp         = sf::Event::KeyReleased,
  MButtonDown   = sf::Event::MouseButtonPressed,
  MButtonUp     = sf::Event::MouseButtonReleased,
  MouseWheel    = sf::Event::MouseWheelMoved,
  WindowResized = sf::Event::Resized,
  GainedFocus   = sf::Event::GainedFocus,
  LostFocus     = sf::Event::LostFocus,
  MouseEntered  = sf::Event::MouseEntered,
  MouseLeft     = sf::Event::MouseLeft,
  Closed        = sf::Event::Closed,
  TextEntered   = sf::Event::TextEntered,
  Keyboard      = sf::Event::Count + 1,             ///< событие реального времени
  Mouse,                                            ///< событие реального времени
  Joystick                                          ///< событие реального времени
};


/// код клавиши, в частности.
struct EventInfo {
  EventInfo()  { m_code = 0; }
  EventInfo(int l_event){ m_code = l_event; }
  union { 
        int m_code;  
    };
};

/// контейнер информации о событии : тип события - код клавиши
using Events = std::vector<std::pair<EventType, EventInfo>>;


/// \brief ассоциирует событие со строковым идентификатором
struct EventDetails {
  EventDetails(const std::string& l_bindName) : m_name(l_bindName) {
    Clear();
  }

  void Clear() {
    m_size = sf::Vector2i(0, 0);
    m_textEntered = 0;
    m_mouse = sf::Vector2i(0, 0);
    m_mouseWheelDelta = 0;
    m_keyCode = -1;
  }

  std::string   m_name;
  sf::Vector2i  m_size;
  sf::Uint32    m_textEntered;
  sf::Vector2i  m_mouse;
  int m_mouseWheelDelta;
  int m_keyCode; // Single key code.
};


/// ассоциирует имя действия программы (паттерн "Команда") со списком событий ввода
struct Binding {
  Binding(const std::string& l_name) : m_name(l_name), m_details(l_name) 
  {}
  
  ~Binding()  
  {}

  void BindEvent(EventType l_type, EventInfo l_info = EventInfo()) {
    m_events.emplace_back(l_type, l_info);
  }

  EventDetails  m_details;
  Events        m_events;
  std::string   m_name;
  int           c = 0;      // Count of events that are "happening".
};

/// набор зарегистрированных именованных событий пользовательского ввода
using Bindings = std::unordered_map<std::string, Binding*>;

/// набор соответствующих "команд программы" - "именам событий пользовательского ввода"
using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;

enum class StateType;

/// каждый режим имеет собственный CallbackContainer. 
/// Поэтому их нужно хранить для каждого режима отдельно.  
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

/// \brief отслеживает возникновения событий пользователького ввода
///
/// Регистрирует, отслеживает и приводит в действие события пользовательского ввода.
/// 1. Регистритует именованные события управления
/// конфигураций (EventManager::LoadBindings)
/// 2. Ассоциирует программные "комманды" каждого режима с "событиями 
/// управления" (EventManager::AddCallback)
/// 3. Отслеживает возникновение зарегистрированных событий управления и 
/// запускает команды ассоциированные с ними (EventManager::Update, EventManager::HandleEvent)
/// 4. Отслеживает позицию мыши EventManager::GetMousePos.
/// 
/// \see 
class EventManager {
 public:
    /// при создании загружает конфигурационный файл событий управления
  EventManager();
  ~EventManager();
    
    /// добавляет именованное событие управления в набор отслеживаемых
  bool AddBinding(Binding *l_binding);
    
    /// удаляет из набора
  bool RemoveBinding(std::string l_name);

    /// устанавливает текущий режим программы, для соответтвующего ассоциирования 
    /// событий управления с вызываемыми "коммандами"
    /// \see StateManager::SwitchTo
  void SetCurrentState(StateType l_state);
    
    /// фокус активности окна
  void SetFocus(const bool& l_focus);

  /// \brief ассоциирует событие управления с конкретной коммандой программы.
  /// 
  /// AddCallback(StateType::TanksMenu, "name", &State_TanksMenu::CallbackReturn, this);
  /// 
  /// \see 
  template<class T> 
  bool AddCallback(StateType l_state, const std::string& l_name,
                   void(T::*l_func)(EventDetails*), T* l_instance ) {
    // пытаемся разместить новую std::pair, в хранилище команд для режимов
    // возврат - itr указывает на элемент с ключем state в любом случае
    // (если уже имелся элемент - itr указывает на давний)
    auto itr = m_callbacks.emplace(l_state, CallbackContainer()).first;
    // преобразуем вызов метода экземляра класс в вызываемый объект
    auto temp = std::bind(l_func, l_instance, std::placeholders::_1);
    // добавляем созданную "команду" в хранилище для указанного режима 
    return itr->second.emplace(l_name, temp).second;
  }

    /// \brief удаляет именованную команду \a name из набора событий, 
    /// обрабатываемых режимом \a state
    bool RemoveCallback(StateType l_state,   const std::string& l_name) {
    auto itr = m_callbacks.find(l_state);
    if (itr == m_callbacks.end()) { 
      return false; 
    }
    auto itr2 = itr->second.find(l_name);
    if (itr2 == itr->second.end()) {
      return false; 
    }
    itr->second.erase(l_name);
    return true;
  }
  /// \brief отслеживает возникновения событий 
  void HandleEvent(sf::Event& l_event);
  
  /// \brief отслеживает возникновения событий клавиатуры и мыши
  void Update();

  /// \brief позиция мыши в координатах графического окна
  sf::Vector2i GetMousePos(sf::RenderWindow* l_wind = nullptr) {
    return ( l_wind ?   
                 sf::Mouse::getPosition(*l_wind) : 
                 sf::Mouse::getPosition()); }

 private:
  /// \brief загрузка конфигурационного файла уменованных событий управления 
  ///
  /// Например в фале кофигурации указано:  \n
  ///     ...
  ///     Fullscreen_toggle 5:89            \n
  ///     ...
  ///     Fullscreen_togglee  - Binding::Name "именованное событие пользовательского ввода"     \n
  ///     5:89                - заносится в Binding::m_events, тип pair /EventType, EventInfo/  \n
  /// \n
  ///    в случае ..Fullscreen_toggle 5:89 5:90.. - 5:89 5:90 оба вносится в Binding::m_events  \n
  /// 
  /// \note расположение загружаемого файла указывается в коде реализации функции.
  void LoadBindings();

  StateType   m_currentState;
  Bindings    m_bindings;
  Callbacks   m_callbacks;

  bool m_hasFocus;
};





