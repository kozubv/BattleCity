/// \defgroup Visualisation Визуализация
///  \brief Набор элементов, обеспечивающих графическое отображение и управление им.

/// \file 
/// \ingroup Visualisation Визуализация
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include <SFML\Graphics.hpp>

/// набор именованных файлов изображений для текстур
using TexteresPaths = std::unordered_map<std::string, std::string>;

/// набор именованых текстур
using Textures = std::unordered_map<std::string, sf::Texture*>;

/// \ingroup Visualisation
/// \brieaf предварительно загружает все указанные в списке текстуры,
/// и предоставляет к ним доступ по уникальному имени
/// \details Пример файла c перечнем текстур для программы:
/// \include ..\resources\textures.cfg
/// Пример использования : \snippet SpriteManager.cpp demo_code_texture_manager
class TextureManager {
 public:
  TextureManager(TextureManager&)    = delete;
  TextureManager& operator=(TextureManager&)  = delete;

  /// Предоставляет доступ к загруженной текстуре по
  /// \param id_name уникальное имя текстуры
  static const sf::Texture* Get(const std::string& id_name);

 private:
  TextureManager();
  ~TextureManager();
  void LoadPaths(const std::string& );
  void LoadTextures();

  TexteresPaths  m_paths;                  ///< набор файлов с текстурами
  Textures    m_textures;                  ///< набор загруженных текстур
};


/// \brief Хранит уникальное имя изображения и его координаты на содержащей текстуре
class SpriteInfo
{
 public:
  SpriteInfo();
  SpriteInfo(const sf::Texture* texture, const sf::IntRect& rect);
  const sf::Texture*  m_texture;    // указатель на текстуру
  sf::IntRect         m_rect;      
};
std::istream& operator>>(std::istream&, SpriteInfo&);
std::ostream& operator<<(std::ostream&, const SpriteInfo&);


/// набор именованных изображений и их информации
using SpriteInfoMap = std::unordered_map<std::string, SpriteInfo>;

/// \ingroup Visualisation
/// \brief предоставляет копию спрайта по уникальному имени
/// \details Перед попуткой получить спрайт, необхоимо загрудить файл с перечнем спрайтов.
///   На основании имени, под которым спрайт отмечен в файле-перечне, можно получть его экземпляр.
/// Пример файла c перечнем изображений:
/// \include ..\resources\sprites.cfg
class SpriteManager {
 public:
  SpriteManager(SpriteManager&) = delete;
  SpriteManager& operator=(SpriteManager&) = delete;
  
  /// предоставляет экземпляр спрайта, соответствующего имени
  /// \param sprite_name имя спрайта, указанное в файле-перечне
  static sf::Sprite Get(const std::string& sprite_name);
  //static const SpriteInfo* GetInfo(const std::string&);

 private:
  SpriteManager();

  /// загружает SpriteInfo из файла-перечня спрайтов
  void LoadSprites();
  SpriteInfoMap m_spriteInfoMap;
};

// функции для тестирования 
void test_TextureManager();
void test_SpriteManager();
void test_SpriteManager_createSprite();
