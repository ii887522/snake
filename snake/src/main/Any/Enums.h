// Copyright ii887522

#ifndef SNAKE_SRC_MAIN_ANY_ENUMS_H_
#define SNAKE_SRC_MAIN_ANY_ENUMS_H_

namespace ii887522::snake {

enum class Path : unsigned int {
  MAIN, GAME
};

enum class CellType : unsigned int {
  BACKGROUND, WALL, SNAKE_HEAD, SNAKE_BODY, FOOD
};

}  // namespace ii887522::snake

#endif  // SNAKE_SRC_MAIN_ANY_ENUMS_H_
