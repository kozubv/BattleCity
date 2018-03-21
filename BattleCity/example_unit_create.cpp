void test_factory_create() {
  using namespace std;
  cout << "start game entity creating :" << endl;

    // загрузка путей к файлам настроек
    SharedContext(R"(..\resources\paths.cfg)");

    // загрузка файлов настроек для анимаций
  SpriteSheetInfo::LoadSpriteSheetInfo(SharedContext::getFilePath("SpriteSheet"));

    // загрузка коллекции подтипов танков
  Tank::factory::LoadCollection(SharedContext::getFilePath("Tanks"));
  Tank::factory::Create('a', 0);
  Tank::factory::Create('b', 0);
  Tank::factory::Create('c', 0);
  Tank::factory::Create('d', 0);
  cout << "Tanks create" << endl;

    // загрузка коллекции подтипов снарядов
    Bullet::factory::LoadCollection(SharedContext::getFilePath("Bullet"));
  Bullet::factory::Create('A', 0);
  Bullet::factory::Create('B', 0);
  Bullet::factory::Create('C', 0);
  Bullet::factory::Create('D', 0);
  cout << "Bullets create" << endl;

    // загрузка коллекции подтипов препятствий
    Barrier::factory::LoadCollection(SharedContext::getFilePath("Barrier"));
  Barrier::factory::Create('-', 0);
  Barrier::factory::Create('+', 0);
  Barrier::factory::Create('*', 0);
  Barrier::factory::Create('/', 0);
  Barrier::factory::Create('=', 0);
  cout << "Barriers create" << endl;

    // загрузка коллекции подтипов бонусов
    Bonus::factory::LoadCollection(SharedContext::getFilePath("Bonus"));
  Bonus::factory::Create('c', 0);
  Bonus::factory::Create('b', 0);
  Bonus::factory::Create('s', 0);
  Bonus::factory::Create('t', 0);
  Bonus::factory::Create('h', 0);
  Bonus::factory::Create('w', 0);
  Bonus::factory::Create('f', 0);
    cout << "Bonus create" << endl;

    // Пример стандартной проблемы глобальных переменных. (неуправляемый порядок инициализации и удаления)
    //
    // т.к. элементы GameEntity::g_entity_list ссылаются на элементы статических контейнеров 
    // Factory<T1, T2>::s_current_list, если удаление Factory<T1, T2>::s_current_list произойдет
    // раньше, чем будет вызван GameEntity::g_entity_list.clear(), произйдет обращение
    // к недействиетельным итераторам. что является ошибкой. 
    //
    // Поэтому в программе освобождение g_entity_list производится явно.
    GameEntity::g_entity_list.clear();
    
  cout << "after clearing" << endl;
  getchar();
}
