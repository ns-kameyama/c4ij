#include <std15.h>
/*
./bin2bas --nolineno --startaddress 4096 obj/c4ij.bin
POKE#1000 ...
*/

__attribute__ ((section(".main")))
int main(int param, int ram, int rom, int (*divfunc)()) {
	int res;
	uint8_t buf[0x100];

	// PS/2キーボードマップ変更
	disable_irq();
	const int sec7 = 7; // last sector of IchigoJam OS
	const int sec13 = 13;
	
	// erase buffer
	flash_prepare(sec13, sec13);
	flash_erase(sec13, sec13);

	// copy sec7 + newps2map -> sec
	for (int i = 0; i < 16; i++) {

		if (i == 5) { 
			memcpy(buf, (uint8_t*)(ram + 0x1100), 0x100);
    } else if (i == 6) {
			memcpy(buf, (uint8_t*)(ram + 0x1200), 0x100);
		} else {
			memcpy(buf, (uint8_t*)(sec7 * 0x1000 + 0x100 * i), 0x100);
		}

		flash_prepare(sec13, sec13);
		flash_copyRAMtoFlash((uint8_t*)(sec13 * 0x1000 + 0x100 * i), buf, 0x100); // 256byteずつ
	}

	// erase sector7
	flash_prepare(sec7, sec7);
	flash_erase(sec7, sec7);

	// copy sec -> sector7
	for (int i = 0; i < 16; i++) {
		memcpy(buf, (void*)(sec13 * 0x1000 + 0x100 * i), 0x100);
		flash_prepare(sec7, sec7);
		flash_copyRAMtoFlash((uint8_t*)(sec7 * 0x1000 + 0x100 * i), buf, 0x100); // 256byteずつ
	}

	// erase sec
	flash_prepare(sec13, sec13);
	res = flash_erase(sec13, sec13);

	enable_irq();
	return res;
}
