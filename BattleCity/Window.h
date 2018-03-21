/// \file
/// \ingroup SGML_Game_Book
#pragma once

#include <string>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "EventManager.h"

/// \brief Абстракция графического окна ОС. 
/// 
/// Отделяет конкретную используемую библиотеку (SFML в данном случае) от 
/// использования функционала окна ОС в коде программы. Обеспечивает : 
/// разворачивание, закрытие, отслеживание фокуса мыши, начало и конец отображения
/// графики, обновление программы и изображения,
/// обработку событий пользовательского ввода при помощи EventManager.
/// \see EventManager
class Window {
 public:
  Window();
  Window(const std::string& title, const sf::Vector2u& size);
  ~Window();

  void BeginDraw();
  void EndDraw();

  void Update();

  bool IsDone();                                ///< флаг закрытия окна
  bool IsFullscreen();                          ///< флаг оуна во весь экран
  bool IsFocused();                             ///< флаг фокуса окна

  /// развернуть окно во весь экран
  void ToggleFullscreen(EventDetails* l_details);

  /// Закрыть окно и завершить программу
  void Close(EventDetails* l_details = nullptr);

  sf::RenderWindow* GetRenderWindow();
  EventManager*     GetEventManager();
  sf::Vector2u      GetWindowSize();

 private:
  /// \brief установка надписи и размера окна.
  void Setup(const std::string& title, const sf::Vector2u& size);
  void Create();

  sf::RenderWindow  m_window;                      ///< экземпляр окна
  EventManager      m_eventManager;                ///< обрабочик событий пользовательского ввода
  sf::Vector2u      m_windowSize;
  std::string       m_windowTitle;
  bool              m_isDone;
  bool              m_isFullscreen;
  bool              m_isFocused;
};