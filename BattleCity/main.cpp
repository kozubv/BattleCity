#include "Game.h"
#include "SpriteManager.h"
#include "GuiControl.h"
#include "Units.h"
#include "GameBattleCity.h"
#include "Animating.h"

void main(int argc, void** argv[]) {
  // Program entry point.
  Game game;
  while (!game.GetWindow()->IsDone()) {
    game.Update();
    game.Render();
    game.LateUpdate();
  }
}
