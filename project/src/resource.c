#include <peripheral_io.h>
#include <dlog.h>
#include "resource.h"

static resource_s resource_info[PIN_MAX] = { {0, NULL, NULL}, };

resource_s *resource_get_info(int pin_num)
{
	return &resource_info[pin_num];
}

void resource_close_all(void)
{
	int i = 0;
	for (i = 0; i < PIN_MAX; i++) {
		if (!resource_info[i].opened) continue;
		LOGD("GPIO[%d] is closing...", i);

		if (resource_info[i].close)
			resource_info[i].close(i);
	}
}
