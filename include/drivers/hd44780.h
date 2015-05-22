#ifndef _HD44780_H_
#define _HD44780_H_

#include <sys/device.h>

#define HD44780_CAPS_2LINES     (1<<0)
#define HD44780_CAPS_5X10       (1<<1)

#define HD44780_ENTRY_INC       (1<<1)
#define HD44780_ENTRY_SHIFT     (1<<0)

#define HD44780_ONOFF_DISPLAY_ON      (1<<2)
#define HD44780_ONOFF_CURSOR_ON       (1<<1)
#define HD44780_ONOFF_BLINKING_ON     (1<<0)

#define HD44780_SHIFT_DISPLAY   (1<<3)
#define HD44780_SHIFT_CURSOR    (0<<3)
#define HD44780_SHIFT_RIGHT     (1<<2)
#define HD44780_SHIFT_LEFT      (0<<2)

#define HD44780_LINE_OFFSET	0x40

/* local defines */
enum hd44780_commands {
	CMD_CLEAR =             0x01,
	CMD_HOME =              0x02,
	CMD_ENTRY_MODE =        0x04, /* args: I/D, S */
	CMD_DISPLAY_ON_OFF =    0x08, /* args: D, C, B */
	CMD_SHIFT =             0x10, /* args: S/C, R/L, -, - */
	CMD_FUNCTION_SET =      0x20, /* args: DL, N, F, -, -; only valid at reset */
	CMD_CGRAM_ADDR =        0x40,
	CMD_DDRAM_ADDR =        0x80,
};

struct hd44780_gpio_bus_softc {
	int rs, rw, e;
	int d4, d5, d6, d7;
};

struct hd44780_softc {
	int Te; /* TcycE/2 in us, must be >= 1 */
	int caps;
};

//DRIVER(hd44780_gpio_bus, void);
//DRIVER(hd44780, hd44780_gpio_bus);

class hd44780 {
	public:
		virtual int read(int rs) const = 0;
		virtual int write(int rs, int cmd) const = 0;
		void clear() const;
		void home() const;
		void entry_mode(int mode) const;
		void onoff(int features) const;
		void shift(int shft) const;
		void set_position(int pos) const;
		void write(int data) const;
		void print(const char *str) const;
		int init_lcd() const;
	protected:
		int get_caps() const;
	private:
		void cmd(int c) const;
};

#endif
