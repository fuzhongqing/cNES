#include <iostream>
#include "bus.h"
#include "cartridge.h"
#include "test_rom.h"
#include "display.h"

using namespace cnes;

int main(int argc, char* argv[]) {
    Bus bus;
    CPU cpu;
    PPU ppu;
    APU apu;
    Display display;

    Cartridge cartridge;
    if (!cartridge.load_from_memory(TestROM::get_test_rom_data())) {
        std::cerr << "ROM加载失败" << std::endl;
        return -1;
    }

    bus.connect_cartridge(&cartridge);
    bus.connect_cpu(&cpu);
    bus.connect_apu(&apu);
    bus.connect_ppu(&ppu);
    bus.reset();

    if (!display.init("cNES", 256, 240, 3)) {
        std::cerr << "显示系统初始化失败" << std::endl;
        return -1;
    }

    bool running = true;
    while (running) {
        running = display.handle_events();

        bus.clock();

        if (ppu.frame_complete()) {
          uint8_t* screen_data = ppu.get_screen();
            
          display.update_screen(screen_data);
          ppu.clear_frame_complete();
        }
    }

    return 0;
}

