/*!
\mainpage Пример создания mainPage и relatedPages для проектов Doxygen
\version 1.0
\date 14.11.17
\author kozubv \n
\tableofcontents
	здесь приведено описания использования комманд Doxygen для созадния:
	- главной страницы
	- дополнительных страниц
	- вставок кода с примерами

\section mainpgsec1 обзор MainPage
- MainPage имеет собственную вкладку в главном меню. 
- может иметь \\section <name>
- может иметь \\subpage <name> \n
- \\subpage <name> Может быть просто заменен на \\ref <name> 

например: \n
а это \\ref   : \ref page1 \n
а это \\ref   : \ref page2 \n
а это \\ref   : \ref page3 \n
по сути одна и таже тема.\n
использование \\section дает возможность ссылаться на имя секции через \\ref \n
если такая возможность не упала, то проще использовать header'ы, как на след строчке ->

## обзор RelatedPages и ссылок
relatedPages - являются отдельно создаваемыми страницами описания \n
например создадим новую страницу: \n
\note page - нужно создавать в отдельном блоке комментариев, т.к. могут зациклиться перекрестные ссылки. \n
после создания страницы, можно продолжить писать mainPage \subpage relpg1

## обзор Examples
\note для использования вставок кода примеров надо определить переменную Doxygen EXAMPLE_PATH 
код можно вставлять способами:
-\\code ... \\endcode
-\\inline - вставляет полностью файл
-\\snippet <file_name> (block_id) - вставляет помеченный кусок кода.

код помечается следующим образом :
\verbatim 
\\! [block_ib]
	>> некоторый код
	...
	>> некоторый код
\\! [block_ib]
\endverbatim
и будет так : \snippet mainpage.h block_id_1

*/


/*! \page page1 A documentation page
  \tableofcontents
  Leading text.
  \section sec An example section
  This page contains the subsections \ref subsection1 and \ref subsection2.
  For more info see page \ref page2.
  \subsection subsection1 The first subsection
  Text.
  \subsection subsection2 The second subsection
  More text.
*/

/*! \page page2 Another page
  Even more info.
*/


/*! \page page3 Веселая отдельная страница
	на главную \ref index 

	\page relpg1 Вот и создали мы страницу
	\section sect1 секция принадлежащая странице relpg1
	описалово настранице и возможно ссылка на главную \ref index \n
*/

//! [block_id_1]
int summ(int a, int b) {
	// комменты стандартного с++
	int c;
	/// Doxy comment
	c = a + b;
	return c;
}
//! [block_id_1]