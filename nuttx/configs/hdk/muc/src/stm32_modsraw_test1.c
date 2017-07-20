/*
 * Copyright (c) 2016 Motorola Mobility, LLC.
 * All rights reserved.
 *
  * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <debug.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arch/board/mods.h>

#include <nuttx/arch.h>
#include <nuttx/device.h>
#include <nuttx/device_raw.h>
#include <nuttx/power/pm.h>

#include "stm32_tim.h"

#define BLINKY_ACTIVITY    10
#define BLINKY_TIM          6
#define BLINKY_TIM_FREQ  1000
#define BLINKY_PERIOD    1000

#define LED_ON              0
#define LED_OFF             1

static struct stm32_tim_dev_s *tim_dev;

struct blinky_info {
    struct device *gDevice;
    raw_send_callback gCallback;
};


static int read_button_state(void) 
{
    int value = stm32_gpioread(GPIO_MODS_BUTTON);
    dbg("Button State %d \n", value);
    return value;
}


/**
 * Call the base raw device to send our data
 */ 
static int blinky_send(struct device *dev, uint32_t len, uint8_t data[])
{
    
    struct blinky_info *info = NULL;

    uint8_t *resp_msg;

    if (len == 0) return -EINVAL;

    if (!dev || !device_get_private(dev)) {
        return -ENODEV;
    }
    info = device_get_private(dev);

    // allocate memory for response message 
    resp_msg = zalloc(len);
    if (!resp_msg) {
        return -ENOMEM;
    }

    // copy payload data and send the message
    memcpy((void *)resp_msg, data, len);

    if (info->gCallback) {
        info->gCallback(info->gDevice, len, resp_msg);
    }
    free(resp_msg); 
    return OK;
}

/**
 * Call the base raw device to send our data
 */
static int blinky_send_bulk(struct device *dev, uint32_t len)
{

    struct blinky_info *info = NULL;

    uint8_t *resp_msg;

    if (len == 0) return -EINVAL;

    if (!dev || !device_get_private(dev)) {
        return -ENODEV;
    }
    info = device_get_private(dev);

    // allocate memory for response message 
    resp_msg = zalloc(len);
    if (!resp_msg) {
        return -ENOMEM;
    }

    // copy payload data and send the message
    //memcpy((void *)&resp_msg, data, len);

    if (info->gCallback) {
        info->gCallback(info->gDevice, len, resp_msg);
    }
    free(resp_msg);
    return OK;
}

static int blinky_set_gpio(uint8_t gpio_ind, uint8_t value) 
{
    int gpio;
    if (gpio_ind == 2 || gpio_ind == '2')
       gpio = GPIO_MODS_DEMO_1;
    else if (gpio_ind == 3 || gpio_ind == '3')
       gpio = GPIO_MODS_DEMO_2;
    else 
       return -EINVAL;

    int enabled = (value == 1 || value == '1');
	
    gpio_set_value(gpio, enabled);

    dbg("SET GPIO %d %d \n", gpio, enabled);

    return 0;
}

static int pow(int base, int exp) {
  int res = 1;
  int i;
  for(i = 0; i < exp; i++)
    res *= base;
  return res;
}


static int my_recv(struct device *dev, uint32_t len, uint8_t data[])
{
   
    uint8_t msg[] = {0xFF, 0xFA};
    int res = OK;
    
    if (len == 1) {
    	if (data[0] == 0 || data[0] == '0')
            gpio_direction_out(GPIO_MODS_LED_DRV_3, LED_OFF);
    	else if (data[0] == 1 || data[0] == '1')
            gpio_direction_out(GPIO_MODS_LED_DRV_3, LED_ON);
        return res;

    }

    return -EINVAL;
}

static int my_register_callback(struct device *dev,
                                    raw_send_callback callback)
{

    struct blinky_info *info = NULL;
    if (!dev || !device_get_private(dev)) {
        return -ENODEV;
    }
    info = device_get_private(dev);

    info->gCallback = callback;
    return 0;
}

static int my_unregister_callback(struct device *dev)
{
    struct blinky_info *info = NULL;

    if (!dev || !device_get_private(dev)) {
        return -ENODEV;
    }
    info = device_get_private(dev);

    info->gCallback = NULL;	
    return 0;
}

static int my_setup(struct device *dev)
{
    gpio_direction_out(GPIO_MODS_LED_DRV_3, LED_OFF);
    gpio_direction_out(GPIO_MYGPIO2, 0);
    gpio_direction_out(GPIO_MYGPIO3, 1);
    gpio_direction_in(GPIO_MODS_BUTTON);

    struct blinky_info *info;

    if (!dev) {
        return -EINVAL;
    }

    info = zalloc(sizeof(*info));
    if (!info) {
        return -ENOMEM;
    }

    info->gDevice = dev;
    device_set_private(dev, info);

    return 0;
}

/**
 * remove function called when device is unregistered.
 */
static void my_remove(struct device *dev)
{
    struct blinky_info *info = NULL;

    if (!dev || !device_get_private(dev)) {
        return;
    }
    info = device_get_private(dev);

    free(info);
    device_set_private(dev, NULL);
}


static struct device_raw_type_ops test1_type_ops = {
    .recv = my_recv,
    .register_callback = my_register_callback,
    .unregister_callback = my_unregister_callback,
};

static struct device_driver_ops test1_driver_ops = {
    .probe = my_setup,
    .remove = my_remove, 
    .type_ops = &test1_type_ops,
};

struct device_driver mods_raw_test1_driver = {
    .type = DEVICE_TYPE_RAW_HW,
    .name = "mods_test1",
    .desc = "MOD Test1 by Luan",
    .ops = &test1_driver_ops,
};
