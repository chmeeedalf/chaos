
// This namespace is to be provided by the platform
namespace bsp {
	/*!
	 * Set scheduler tick to a rate of hz.
	 */
	void set_systick(int hz);
	
	void init(void) __attribute__((weak));
	void systick_enable(void);
	void systick_disable(void);
};
