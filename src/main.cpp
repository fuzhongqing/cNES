#include <iostream>
#include "bus.h"
#include "cartridge.h"
#include "test_rom.h"
#include "display.h"

using namespace cnes;

int main(int argc, char* argv[]) {
    // 创建系统总线
    Bus bus;

    // 创建并加载卡带
    Cartridge cartridge;
    if (argc == 2) {
        // 如果提供了ROM文件路径，尝试加载
        if (!cartridge.load(argv[1])) {
            std::cout << "Failed to load ROM file: " << argv[1] << std::endl;
            return -1;
        }
        std::cout << "Loaded ROM file: " << argv[1] << std::endl;
    } else {
        // 如果没有提供ROM文件，使用测试ROM
        std::vector<uint8_t> test_rom_data = TestROM::get_test_rom_data();
        if (!cartridge.load_from_memory(test_rom_data)) {
            std::cout << "Failed to load test ROM" << std::endl;
            return -1;
        }
        std::cout << "Using test ROM" << std::endl;
    }

    // 连接卡带到总线
    bus.connect_cartridge(&cartridge);

    PPU* ppu = bus.ppu();

    // 重置系统
    bus.reset();

    // 创建并初始化显示系统
    Display display;
    if (!display.init("cNES", 256, 240, 3)) {
        std::cout << "显示系统初始化失败" << std::endl;
        return -1;
    }

    // 主循环
    bool running = true;
    while (running) {
        // 处理事件
        running = display.handle_events();

        // 执行一个时钟周期
        bus.clock();

        // 更新画面
        if (ppu->frame_complete()) {
            // 获取PPU输出的屏幕数据并更新显示
            display.update_screen(ppu->get_screen());
            ppu->clear_frame_complete();
        }
    }

    return 0;
}

