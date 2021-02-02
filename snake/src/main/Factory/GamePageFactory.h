// Copyright ii887522

#ifndef SNAKE_SRC_MAIN_FACTORY_GAMEPAGEFACTORY_H_
#define SNAKE_SRC_MAIN_FACTORY_GAMEPAGEFACTORY_H_

#ifndef TEST

#include <Any/Reactive.h>
#include <Reactive/BinaryReactive.h>
#include <ViewGroup/Page.h>
#include <Struct/Point.h>
#include <Struct/Rect.h>
#include <ViewGroup/Modal.h>
#include <Struct/Paint.h>
#include <Struct/Color.h>
#include <Struct/Size.h>
#include <Image/Text.h>
#include <Text/Score.h>
#include <View/Button.h>
#include <View/BorderView.h>
#include <Any/Map.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <random>
#include <chrono>  // NOLINT(build/c++11)
#include "../Any/Enums.h"
#include "../View/Snake.h"
#include "../View/Food.h"

using std::runtime_error;
using std::default_random_engine;
using std::chrono::high_resolution_clock;
using ii887522::nitro::Reactive;
using ii887522::nitro::BinaryReactive;
using ii887522::viewify::Page;
using ii887522::viewify::Point;
using ii887522::viewify::Rect;
using ii887522::viewify::Modal;
using ii887522::viewify::Paint;
using ii887522::viewify::Color;
using ii887522::viewify::Size;
using ii887522::viewify::Text;
using ii887522::viewify::Score;
using ii887522::viewify::Button;
using ii887522::viewify::BorderView;
using ii887522::viewify::Map;

namespace ii887522::snake {

// Not Thread Safe: it must only be used in main thread
// See also View.h for more details
template <unsigned int viewCount> class GamePageFactory final {
  // remove copy semantics
  GamePageFactory(const GamePageFactory&) = delete;
  GamePageFactory& operator=(const GamePageFactory&) = delete;

  // remove move semantics
  GamePageFactory(GamePageFactory&&) = delete;
  GamePageFactory& operator=(GamePageFactory&&) = delete;

 public:
  // Not Thread Safe
  class Builder final {
    // remove copy semantics
    Builder(const Builder&) = delete;
    Builder& operator=(const Builder&) = delete;

    // remove move semantics
    Builder(Builder&&) = delete;
    Builder& operator=(Builder&&) = delete;

    SDL_Renderer*const renderer;
    const Size<int> size;
    Reactive<Path>& currentPath;
    TTF_Font* headFont;
    bool hasSetHeadFont;
    TTF_Font* bodyFont;
    bool hasSetBodyFont;
    int cellSize;
    bool hasSetCellSize;
    unsigned int buttonDuration;  // button animation duration
    bool hasSetButtonDuration;
    SDL_Cursor*const pointer;
    Size<int> buttonPadding;
    bool hasSetButtonPadding;

   public:
    // Param renderer: it must not be assigned to integer
    // Param pointer: it must not be assigned to integer
    explicit Builder(SDL_Renderer*const renderer, const Size<int>& size, SDL_Cursor*const pointer, Reactive<Path>*const currentPath) :
      renderer{ renderer }, size{ size }, currentPath{ *currentPath }, headFont{ nullptr }, hasSetHeadFont{ false },
      bodyFont{ nullptr }, hasSetBodyFont{ false }, cellSize{ 0 }, hasSetCellSize{ false }, buttonDuration{ 0u },
      hasSetButtonDuration{ false }, pointer{ pointer }, hasSetButtonPadding{ false } { }

    // Param value: it must not be assigned to integer
    // Must Call Time(s): At least 1
    constexpr Builder& setHeadFont(TTF_Font*const value) {
      headFont = value;
      hasSetHeadFont = true;
      return *this;
    }

    // Param bodyFont: it must not be assigned to integer
    // Must Call Time(s): At least 1
    constexpr Builder& setBodyFont(TTF_Font*const value) {
      bodyFont = value;
      hasSetBodyFont = true;
      return *this;
    }

    // Must Call Time(s): At least 1
    constexpr Builder& setCellSize(const int value) {
      cellSize = value;
      hasSetCellSize = true;
      return *this;
    }

    // Button animation duration
    // Must Call Time(s): At least 1
    constexpr Builder& setButtonDuration(const unsigned int value) {
      buttonDuration = value;
      hasSetButtonDuration = true;
      return *this;
    }

    // Must Call Time(s): At least 1
    constexpr Builder& setButtonPadding(const Size<int>& value) {
      buttonPadding = value;
      hasSetButtonPadding = true;
      return *this;
    }

    GamePageFactory* build() {
      if (!hasSetHeadFont) throw runtime_error{ "GamePageFactory headFont is required!" };
      if (!hasSetBodyFont) throw runtime_error{ "GamePageFactory bodyFont is required!" };
      if (!hasSetCellSize) throw runtime_error{ "GamePageFactory cellSize is required!" };
      if (!hasSetButtonDuration) throw runtime_error{ "GamePageFactory buttonDuration is required!" };
      if (!hasSetButtonPadding) throw runtime_error{ "GamePageFactory buttonPadding is required!" };
      return new GamePageFactory{ *this };
    }

    friend class GamePageFactory;
  };

 private:
  SDL_Renderer*const renderer;
  const Rect<int> rect;
  const Rect<int> wallRect;
  Reactive<Path>& currentPath;
  TTF_Font*const headFont;
  TTF_Font*const bodyFont;
  const int cellSize;
  const unsigned int buttonDuration;  // button animation duration
  BinaryReactive<bool, bool, bool> isLosingModalShowing;
  BinaryReactive<bool, bool, bool> isWinningModalShowing;
  BinaryReactive<bool, bool, bool> isModalShowing;
  BinaryReactive<bool, bool, bool> canScoreReset;
  SDL_Cursor*const pointer;
  const Size<int> buttonPadding;
  default_random_engine randomEngine;
  Reactive<bool> isSnakeEatFood;
  Map<CellType> map;

  explicit constexpr GamePageFactory(const Builder& builder, const int headerHeight = 64) : renderer{ builder.renderer },
    rect{ Rect{ Point{ 0, 0 }, builder.size } },
    wallRect{ Rect{ Point{ rect.position.x, rect.position.y + headerHeight }, Size{ rect.size.w, rect.size.h - headerHeight } } },
    currentPath{ builder.currentPath }, headFont{ builder.headFont }, bodyFont{ builder.bodyFont }, cellSize{ builder.cellSize },
    buttonDuration{ builder.buttonDuration }, isLosingModalShowing{ false }, isWinningModalShowing{ false },
    isModalShowing{ &isLosingModalShowing, &isWinningModalShowing, [](const bool*const left, const bool*const right) {
      return *left || *right;
    } },
    canScoreReset{ &isLosingModalShowing, &isWinningModalShowing, [](const bool*const left, const bool*const right) {
      return !(*left || *right);
    } }, pointer{ builder.pointer }, buttonPadding{ builder.buttonPadding },
    randomEngine{ static_cast<unsigned int>(high_resolution_clock::now().time_since_epoch().count()) }, isSnakeEatFood{ false },
    map{ Size{ static_cast<unsigned int>(wallRect.size.w / cellSize), static_cast<unsigned int>(wallRect.size.h / cellSize) } } { }

  constexpr unsigned int getBackgroundCellCount() const {
    return (wallRect.size.w / cellSize - 2u) * (wallRect.size.h / cellSize - 2u);
  }

 public:
  Page<Path, viewCount>* make() {
    constexpr Size modalSize{ 512, 256 };
    constexpr auto modalDuration{ 125u };  // modal animation duration
    constexpr Point backButtonPosition{ 88, 156 };
    constexpr Point playAgainButtonPosition{ 232, 156 };
    return new Page<Path, viewCount>{ renderer, Point{ 0, 0 }, Path::GAME, &currentPath, {
      new BorderView<CellType>{
        renderer, wallRect.position, Paint{ wallRect.size, Color{ 255u, 0u, 0u } }, cellSize, &map, CellType::WALL
      },
      Snake::Builder{ renderer, wallRect, cellSize, randomEngine, map, [this]() {
        isLosingModalShowing.set(true);
      } }
        .setDead(&isModalShowing)
        .setHasEatFood(&isSnakeEatFood)
        .build(),
      new Food{ renderer, wallRect, cellSize, randomEngine, map, &isSnakeEatFood },
      new Modal<5u>{
        renderer, rect.size, Point{ (rect.size.w - modalSize.w) >> 1u, (rect.size.h - modalSize.h) >> 1u },
        Paint{ modalSize, Color{ 255u, 192u, 192u } }, &isLosingModalShowing, modalDuration, {
          Text::Builder{ renderer, headFont, Point{ 48, 32 }, "GAME OVER!", Color{ 255u, 0u, 0u } }
            .setA(0u)
            .setDuration(modalDuration)
            .build(),
          Button::Builder{ renderer, backButtonPosition, Paint{ Size{ 114, 64 }, Color{ 0u, 0u, 255u } } }
            .setA(0u)
            .setADuration(modalDuration)
            .setLightnessDuration(buttonDuration)
            .setOnMouseMove([this]() {
              SDL_SetCursor(pointer);
            })
            .setOnMouseOver([]() {})
            .setOnMouseOut([]() {
              SDL_SetCursor(SDL_GetDefaultCursor());
            })
            .setOnClick([this]() {
              isLosingModalShowing.set(false);
              currentPath.set(Path::MAIN);
            })
            .build(),
          Text::Builder{ renderer, bodyFont, backButtonPosition + buttonPadding, "Back", Color{ 255u, 255u, 255u } }
            .setA(0u)
            .setDuration(modalDuration)
            .build(),
          Button::Builder{ renderer, playAgainButtonPosition, Paint{ Size{ 192, 64 }, Color{ 255u, 0u, 0u } } }
            .setA(0u)
            .setADuration(modalDuration)
            .setLightnessDuration(buttonDuration)
            .setOnMouseMove([this]() {
              SDL_SetCursor(pointer);
            })
            .setOnMouseOver([]() {})
            .setOnMouseOut([]() {
              SDL_SetCursor(SDL_GetDefaultCursor());
            })
            .setOnClick([this]() {
              isLosingModalShowing.set(false);
            })
            .build(),
          Text::Builder{ renderer, bodyFont, playAgainButtonPosition + buttonPadding, "Play again", Color{ 255u, 255u, 255u } }
            .setA(0u)
            .setDuration(modalDuration)
            .build()
        }
      },
      new Modal<5u>{
        renderer, rect.size, Point{ (rect.size.w - modalSize.w) >> 1u, (rect.size.h - modalSize.h) >> 1u },
        Paint{ modalSize, Color{ 192u, 255u, 192u } }, &isWinningModalShowing, modalDuration, {
          Text::Builder{ renderer, headFont, Point{ 96, 32 }, "YOU WIN!", Color{ 0u, 192u, 0u } }
            .setA(0u)
            .setDuration(modalDuration)
            .build(),
          Button::Builder{ renderer, backButtonPosition, Paint{ Size{ 114, 64 }, Color{ 0u, 0u, 255u } } }
            .setA(0u)
            .setADuration(modalDuration)
            .setLightnessDuration(buttonDuration)
            .setOnMouseMove([this]() {
              SDL_SetCursor(pointer);
            })
            .setOnMouseOver([]() {})
            .setOnMouseOut([]() {
              SDL_SetCursor(SDL_GetDefaultCursor());
            })
            .setOnClick([this]() {
              isWinningModalShowing.set(false);
              currentPath.set(Path::MAIN);
            })
            .build(),
          Text::Builder{ renderer, bodyFont, backButtonPosition + buttonPadding, "Back", Color{ 255u, 255u, 255u } }
            .setA(0u)
            .setDuration(modalDuration)
            .build(),
          Button::Builder{ renderer, playAgainButtonPosition, Paint{ Size{ 192, 64 }, Color{ 0u, 192u, 0u } } }
            .setA(0u)
            .setADuration(modalDuration)
            .setLightnessDuration(buttonDuration)
            .setOnMouseMove([this]() {
              SDL_SetCursor(pointer);
            })
            .setOnMouseOver([]() {})
            .setOnMouseOut([]() {
              SDL_SetCursor(SDL_GetDefaultCursor());
            })
            .setOnClick([this]() {
              isWinningModalShowing.set(false);
            })
            .build(),
          Text::Builder{ renderer, bodyFont, playAgainButtonPosition + buttonPadding, "Play again", Color{ 255u, 255u, 255u } }
            .setA(0u)
            .setDuration(modalDuration)
            .build()
        }
      },
      // Begin glowing views
      Score::Builder{
        renderer, bodyFont, Point{ rect.size.w >> 1u, 14 }, Color{ 255u, 255u, 255u }, static_cast<unsigned int>(getBackgroundCellCount() * .75f), [this]() {
          isWinningModalShowing.set(true);
        }
      }
        .setCanIncrement(&isSnakeEatFood)
        .setCanReset(&canScoreReset)
        .build()
      // End glowing views
    } };
  }
};

}  // namespace ii887522::snake

#endif
#endif  // SNAKE_SRC_MAIN_FACTORY_GAMEPAGEFACTORY_H_
