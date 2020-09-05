#include <stdlib.h>
#include <unistd.h>
#include <peripheral_io.h>
#include <dlog.h>
#include "../resource_internal.h"
#include "../voice_control_panel_main.h"

void resource_close_relay(int pin_num)
{
	if (!resource_get_info(pin_num)->opened) return;

	LOGD("Relay is finishing...");
	peripheral_gpio_close(resource_get_info(pin_num)->sensor_h);
	resource_get_info(pin_num)->opened = 0;
}

int resource_write_relay(int pin_num, int write_value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (!resource_get_info(pin_num)->opened) {
		ret = peripheral_gpio_open(pin_num, &resource_get_info(pin_num)->sensor_h);
		retv_if(!resource_get_info(pin_num)->sensor_h, -1);

		ret = peripheral_gpio_set_direction(resource_get_info(pin_num)->sensor_h, PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
		retv_if(ret != 0, -1);

		resource_get_info(pin_num)->opened = 1;
		resource_get_info(pin_num)->close = resource_close_relay;
	}

	ret = peripheral_gpio_write(resource_get_info(pin_num)->sensor_h, write_value);
	retv_if(ret < 0, -1);

#ifdef DEBUG
	LOGD("Relay Value : %s", write_value ? "ON":"OFF");
#endif

	return 0;
}
