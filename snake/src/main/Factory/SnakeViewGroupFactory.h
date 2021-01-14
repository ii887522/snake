// Copyright ii887522

#ifndef SNAKE_SRC_MAIN_FACTORY_SNAKEVIEWGROUPFACTORY_H_
#define SNAKE_SRC_MAIN_FACTORY_SNAKEVIEWGROUPFACTORY_H_

#ifndef TEST

#include <SDL.h>
#include <SDL_ttf.h>
#include <Factory/ViewGroupFactory.h>
#include <View/ViewGroup.h>
#include <Struct/Size.h>
#include <Struct/Point.h>
#include <Any/Reactive.h>
#include "../Any/Enums.h"
#include "MainPageFactory.h"
#include "GamePageFactory.h"

using ii887522::viewify::ViewGroupFactory;
using ii887522::viewify::ViewGroup;
using ii887522::viewify::Size;
using ii887522::viewify::Point;
using ii887522::nitro::Reactive;

namespace ii887522::snake {

constexpr static auto mainPageViewCount{ 15u };
constexpr static auto gamePageViewCount{ 6u };

// See also ../View/ViewGroup.h for more details
template <unsigned int viewCount> class SnakeViewGroupFactory final : public ViewGroupFactory<viewCount> {
  // remove copy semantics
  SnakeViewGroupFactory(const SnakeViewGroupFactory&) = delete;
  SnakeViewGroupFactory& operator=(const SnakeViewGroupFactory&) = delete;

  // remove move semantics
  SnakeViewGroupFactory(SnakeViewGroupFactory&&) = delete;
  SnakeViewGroupFactory& operator=(SnakeViewGroupFactory&&) = delete;

  TTF_Font*const headFont;
  TTF_Font*const bodyFont;
  SDL_Cursor*const pointer;
  Reactive<Path> currentPath;
  MainPageFactory<mainPageViewCount>* mainPageFactory;
  GamePageFactory<gamePageViewCount>* gamePageFactory;

 public:
  // See also ../View/ViewGroup.h for more details
  explicit constexpr SnakeViewGroupFactory() : ViewGroupFactory<viewCount>{ }, headFont{ TTF_OpenFont("res/main/arial.ttf", 64) },
    bodyFont{ TTF_OpenFont("res/main/arial.ttf", 32) }, pointer{ SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND) },
    currentPath{ Path::MAIN }, mainPageFactory{ nullptr }, gamePageFactory{ nullptr } { }

  // Param renderer: it must not be assigned to integer
  ViewGroup<viewCount> make(SDL_Renderer*const renderer, const Size<int>& size) override {
    constexpr auto cellSize{ 16 };
    constexpr auto buttonDuration{ 125u };  // button animation duration
    constexpr Size buttonPadding{ 20, 12 };
    return ViewGroup<viewCount>{ renderer, Point{ 0, 0 }, {
      (mainPageFactory = MainPageFactory<mainPageViewCount>::Builder{ renderer, size, &currentPath, pointer }
        .setHeadFont(headFont)
        .setBodyFont(bodyFont)
        .setButtonDuration(buttonDuration)
        .setButtonPadding(buttonPadding)
        .setCellSize(cellSize)
        .build())
        ->make(),
      (gamePageFactory = GamePageFactory<gamePageViewCount>::Builder{ renderer, size, pointer, &currentPath }
        .setHeadFont(headFont)
        .setBodyFont(bodyFont)
        .setButtonDuration(buttonDuration)
        .setButtonPadding(buttonPadding)
        .setCellSize(cellSize)
        .build())
        ->make()
    } };
  }

  ~SnakeViewGroupFactory() {
    delete gamePageFactory;
    delete mainPageFactory;
    SDL_FreeCursor(pointer);
    TTF_CloseFont(bodyFont);
    TTF_CloseFont(headFont);
  }
};

}  // namespace ii887522::snake

#endif
#endif  // SNAKE_SRC_MAIN_FACTORY_SNAKEVIEWGROUPFACTORY_H_
