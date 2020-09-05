#ifndef __RCC_RESOURCE_INTERNAL_H__
#define __RCC_RESOURCE_INTERNAL_H__

#include <peripheral_io.h>

#include "resource/resource_led_internal.h"
#include "resource/resource_relay_internal.h"

#define PIN_MAX 40

struct _resource_s {
	int opened;
	peripheral_gpio_h sensor_h;
	void (*close) (int);
};
typedef struct _resource_s resource_s;

typedef void (*resource_read_cb)(double value, void *data);

struct _resource_read_cb_s {
	resource_read_cb cb;
	void *data;
	int pin_num;
};
typedef struct _resource_read_cb_s resource_read_s;

extern resource_s *resource_get_info(int pin_num);
extern void resource_close_all(void);

#endif /* __RCC_RESOURCE_INTERNAL_H__ */
