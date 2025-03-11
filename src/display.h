#ifndef CNES_DISPLAY_H
#define CNES_DISPLAY_H

#include <cstdint>
#include <SDL2/SDL.h>

namespace cnes {

class PPU;

class Display {
public:
    Display();
    ~Display();

    // 初始化显示系统
    bool init(const char* title, int width, int height, int scale);

    // 连接PPU
    void connect_ppu(PPU* ppu) { ppu_ = ppu; }

    // 更新和渲染画面
    void update_screen(uint8_t* screen_data);

    // 处理事件
    bool handle_events();

    // 清理资源
    void cleanup();

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    uint32_t* pixels_;
    int width_;
    int height_;
    
    // PPU指针
    PPU* ppu_ = nullptr;
};

} // namespace cnes

#endif // CNES_DISPLAY_H