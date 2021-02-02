// Copyright ii887522

#ifndef TEST

#include <Any/allocator.h>
#include <SDL.h>
#include <Any/Subsystems.h>
#include <Any/App.h>
#include <Struct/Size.h>
#include <Functions/control_flow.h>
#include <Struct/Color.h>
#include "../Factory/SnakeViewGroupFactory.h"

using ii887522::viewify::Subsystems;
using ii887522::viewify::App;
using ii887522::viewify::Size;
using ii887522::viewify::eventLoop;
using ii887522::viewify::Color;

namespace ii887522::snake {

static int main(int, char**) {
  const Subsystems subsystems;
  constexpr auto viewCount{ 2u };
  SnakeViewGroupFactory<viewCount> snakeViewGroupFactory;
  eventLoop(App<viewCount>{ "Snake", Size{ 752, 752 }, Color{ 0u, 0u, 0u }, &snakeViewGroupFactory });
  return EXIT_SUCCESS;
}

}  // namespace ii887522::snake

int main(int argc, char** argv) {
  return ii887522::snake::main(argc, argv);
}

#endif
