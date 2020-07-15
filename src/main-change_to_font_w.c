#include <std15.h>

__attribute__ ((section(".main")))
int main(int param, int ram, int rom, int (*divfunc)()) {
	int res;
	uint8_t buf[0x100];

	// change flash!
	disable_irq();
	const int sec6 = 6;
	const int sec13 = 13;

	flash_prepare(sec13, sec13);
	flash_erase(sec13, sec13);

	// copy sec6 + newdata -> sec
	for (int i = 0; i < 16; i++) {
	// i ==  6 : Keyboard
	// i ==  7 : Keyboard
	// i ==  8 : PCG #00-#1F  i == 12 : PCG #80-#9F
	// i ==  9 : PCG #20-#3F  i == 13 : PCG #A0-#BF
	// i == 10 : PCG #40-#5F  i == 14 : PCG #C0-#DF
	// i == 11 : PCG #60-#7F  i == 15 : PCG #E0-#FF

		if (i == 13) {
			memcpy(buf, (uint8_t*)(ram + 0x1100), 0x100);
		} else if (i == 14) {
			memcpy(buf, (uint8_t*)(ram + 0x1200), 0x100);
		} else {
			memcpy(buf, (uint8_t*)(sec6 * 0x1000 + 0x100 * i), 0x100);
		}

		flash_prepare(sec13, sec13);
		flash_copyRAMtoFlash((uint8_t*)(sec13 * 0x1000 + 0x100 * i), buf, 0x100);
	}

	// erase sector6
	flash_prepare(sec6, sec6);
	flash_erase(sec6, sec6);

	// copy sec -> sector6
	for (int i = 0; i < 16; i++) {
		memcpy(buf, (void*)(sec13 * 0x1000 + 0x100 * i), 0x100);
		flash_prepare(sec6, sec6);
		flash_copyRAMtoFlash((uint8_t*)(sec6 * 0x1000 + 0x100 * i), buf, 0x100);
	}

	// erase sec13
	flash_prepare(sec13, sec13);
	res = flash_erase(sec13, sec13);

	enable_irq();
	return res;
}