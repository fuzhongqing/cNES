#include "display.h"

namespace cnes {

Display::Display()
    : window_(nullptr)
    , renderer_(nullptr)
    , texture_(nullptr)
    , pixels_(nullptr)
    , width_(0)
    , height_(0) {
}

Display::~Display() {
    cleanup();
}

bool Display::init(const char* title, int width, int height, int scale) {
    width_ = width;
    height_ = height;

    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    // 创建窗口
    window_ = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width * scale,
        height * scale,
        SDL_WINDOW_SHOWN
    );

    if (!window_) {
        return false;
    }

    // 创建渲染器
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    // 创建纹理
    texture_ = SDL_CreateTexture(
        renderer_,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!texture_) {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    // 创建像素缓冲区
    pixels_ = new uint32_t[width * height];

    return true;
}

void Display::update_screen(uint8_t* screen_data) {
    // 将PPU的输出数据转换为RGB格式
    for (int i = 0; i < width_ * height_; i++) {
        // 这里简单地将PPU的输出转换为灰度值
        pixels_[i] = screen_data[i] | (screen_data[i] << 8) | (screen_data[i] << 16);
    }

    // 更新纹理
    SDL_UpdateTexture(texture_, NULL, pixels_, width_ * sizeof(uint32_t));

    // 清除渲染器
    SDL_RenderClear(renderer_);

    // 将纹理复制到渲染器
    SDL_RenderCopy(renderer_, texture_, NULL, NULL);

    // 显示渲染的内容
    SDL_RenderPresent(renderer_);
}

bool Display::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }
    return true;
}

void Display::cleanup() {
    if (pixels_) {
        delete[] pixels_;
        pixels_ = nullptr;
    }
    if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

} // namespace cnes