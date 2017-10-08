#ifndef GPIO_H_
#define GPIO_H_


#include "STM32/GpioPin.h"
namespace Eni {

	class Gpio {
	public:

		static void initInput(const GpioPin& pin, InputMode inputMode = InputMode::Floating);
	#if defined(STM32)
		static void initOutput(const GpioPin& pin, OutputMode outputMode = OutputMode::PushPull, bool isAlternateFunction = false, PinSpeed speed = PinSpeed::Speed50MHz);
	#else
		static void initOutput(const GpioPin& pin, OutputMode outputMode = OutputMode::PushPull);
	#endif
		static void disconnect(const GpioPin& pin);//Hi-Z

		static void set(const GpioPin& pin);
		static void reset(const GpioPin& pin);
		static void toggle(const GpioPin& pin);

		static bool read(const GpioPin& pin);

	#if defined(STM32)
		static bool lock(const GpioPin& pin);
	#endif

	private:
		static inline void applyConfig(const GpioPin& pin);
	};

}


#endif /* GPIO_H_ */
