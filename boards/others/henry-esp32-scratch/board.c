#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int henry_board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(henry_board_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
