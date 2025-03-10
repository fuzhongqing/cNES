#include <iostream>
#include "bus.h"
#include "cartridge.h"
#include "test_rom.h"

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

    // 重置系统
    bus.reset();

    // 主循环
    bool running = true;
    while (running) {
        // 执行一个时钟周期
        bus.clock();

        // TODO: 处理输入事件
        // TODO: 渲染画面
        // TODO: 输出音频
    }

    return 0;
}

