/*
 * Copyright (c) 2024 Analog Devices Inc.
 * Copyright (c) 2024 BayLibre SAS
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>

#include <time.h>
#include <string.h>

#include <iio/iio.h>
#include <iio/iio-backend.h>
#include <tinyiiod.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

/*
 * iio_create_context():iio.h
 * iio_create_context_from_xml():iio-backend.h
 * iio_context_create_from_backend():iio-backend.h
 */

//int (*scan)(const struct iio_context_params *params,
//	    struct iio_scan *ctx, const char *args);

struct iio_context *zephyr_create_context(const struct iio_context_params *params,
					  const char *uri)
{
	printk("zephyr_create_context()\n");
	return iio_ptr(-ENOSYS);
}

ssize_t zephyr_read_attr(const struct iio_attr *attr,
			 char *dst, size_t len)
{
	printk("zephyr_read_attr()\n");
	return -ENOSYS;
}

ssize_t zephyr_write_attr(const struct iio_attr *attr,
			  const char *src, size_t len)
{
	printk("zephyr_write_attr()\n");
	return -ENOSYS;
}

const struct iio_device *zephyr_get_trigger(const struct iio_device *dev)
{
	printk("zephyr_get_trigger()\n");
	return iio_ptr(-ENOSYS);
}

//int (*set_trigger)(const struct iio_device *dev,
//		const struct iio_device *trigger);

//void (*shutdown)(struct iio_context *ctx);

//int (*get_version)(const struct iio_context *ctx, unsigned int *major,
//		unsigned int *minor, char git_tag[8]);

//int (*set_timeout)(struct iio_context *ctx, unsigned int timeout);

struct iio_buffer_pdata *zephyr_create_buffer(const struct iio_device *dev,
					  unsigned int idx,
					  struct iio_channels_mask *mask)
{
	printk("zephyr_create_buffer()\n");
	return iio_ptr(-ENOSYS);
}

void zephyr_free_buffer(struct iio_buffer_pdata *pdata)
{
	printk("zephyr_free_buffer()\n");
}

//int (*enable_buffer)(struct iio_buffer_pdata *pdata,
//		     size_t nb_samples, bool enable, bool cyclic);

//void (*cancel_buffer)(struct iio_buffer_pdata *pdata);

//ssize_t (*readbuf)(struct iio_buffer_pdata *pdata,
//		   void *dst, size_t len);

//ssize_t (*writebuf)(struct iio_buffer_pdata *pdata,
//		    const void *src, size_t len);

//struct iio_block_pdata *(*create_block)(struct iio_buffer_pdata *pdata,
//					size_t size, void **data);

//void (*free_block)(struct iio_block_pdata *pdata);

//int (*enqueue_block)(struct iio_block_pdata *pdata,
//		     size_t bytes_used, bool cyclic);

//int (*dequeue_block)(struct iio_block_pdata *pdata, bool nonblock);

//int (*get_dmabuf_fd)(struct iio_block_pdata *pdata);

//struct iio_event_stream_pdata *(*open_ev)(const struct iio_device *dev);

//void (*close_ev)(struct iio_event_stream_pdata *pdata);

//int (*read_ev)(struct iio_event_stream_pdata *pdata,
//	       struct iio_event *out_event,
//	       bool nonblock);

// TODO(PM): opps selected based on no-OS POC
static const struct iio_backend_ops zephyr_ops = {
	.create = zephyr_create_context,
	.read_attr = zephyr_read_attr,
	.write_attr = zephyr_write_attr,
	.get_trigger = zephyr_get_trigger,
	.create_buffer = zephyr_create_buffer,
	.free_buffer = zephyr_free_buffer,
};

const struct iio_backend iio_zephyr_backend = {
	.api_version = IIO_BACKEND_API_V1,
	.name = "zephyr",
	.uri_prefix = "zephyr:",
	.ops = &zephyr_ops,
	.default_timeout_ms = 1000,
};

ssize_t zephyr_iiod_read(struct iiod_pdata *pdata, void *buf, size_t size)
{
	struct device *uart_dev = (struct device *)pdata;
	size_t nbytes = 0;
	char *head = buf;
	char c;
	int ret;

	printk("zephyr_iiod_read()\n");
	while (size > 0) {
		ret = uart_poll_in(uart_dev, &c);
		printk("uart_poll_in() -> %i\n", ret);
		*head++ = c;
		size--;
		nbytes++;
	}

	return nbytes;
}

ssize_t zephyr_iiod_write(struct iiod_pdata *pdata, const void *buf,
			  size_t size)
{
	struct device *uart_dev = (struct device *)pdata;
	size_t i;
    unsigned char *cbuf = buf;

	printk("zephyr_iiod_write()\n");
	for (i = 0; i < size; i++) {
		// TODO(PM): Do not blindly cast
		uart_poll_out(uart_dev, cbuf[i]);
	}
	return i;
}

int main(void)
{
	struct iio_context *ctx;
	char *description = "Zephyr RTOS backend";
	char *xml = NULL;
	int ret = 0;

	const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

	if (!device_is_ready(uart_dev)) {
		printk("device_is_ready(uart_dev) -> :-(");
		return 0;
	}

	ctx = iio_context_create_from_backend(&iio_zephyr_backend, description);
	printk("iio_create_context() -> %s\n", strerror(-iio_err(ctx)));

	xml = iio_context_get_xml(ctx);
	printk("iio_context_get_xml() -> %s\n", strerror(-iio_err(ctx)));

	ret = iiod_interpreter(ctx, NULL, zephyr_iiod_read, zephyr_iiod_write,
			       xml, strlen(xml));
	printk("iio_interpreter() -> %i\n", ret);

	iio_context_destroy(ctx);

	return 0;
}
