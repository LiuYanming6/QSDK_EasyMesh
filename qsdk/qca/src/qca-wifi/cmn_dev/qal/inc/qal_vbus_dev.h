/*
 * Copyright (c) 2019 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * DOC: qal_vbus_dev
 * QCA abstraction layer (QAL) virtual bus management APIs
 */

#if !defined(__QDF_VBUS_DEV_H)
#define __QDF_VBUS_DEV_H

/* Include Files */
#include <qdf_types.h>

struct qdf_vbus_resource;
struct qdf_vbus_rstctl;
struct qdf_dev_clk;
struct qdf_pfm_hndl;
struct qdf_pfm_drv;

/**
 * qal_vbus_get_iorsc() - acquire io resource
 * @devnum: Device Number
 * @flag: Property bitmap for the io resource
 * @devname: Device name string
 *
 * This function will allocate the io resource for a device
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
qal_vbus_get_iorsc(int devnum, uint32_t flag, char *devname);

/**
 * qdf_vbus_release_iorsc() - release io resource
 * @devnum: Device Number
 *
 * This function will release the io resource attached to a device
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
qal_vbus_release_iorsc(int devnum);

/**
 * qal_vbus_get_resource() - get resource
 * @pfhndl: Device handle
 * @rsc: Resource handle
 * @restype: Resource type
 * @residx: Resource index
 *
 * This function will acquire a particular resource and attach it to the device
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
qal_vbus_get_resource(struct qdf_pfm_hndl *pfhndl,
		      struct qdf_vbus_resource **rsc, uint32_t restype,
		      uint32_t residx);

/**
 * qal_vbus_get_irq() - get irq
 * @pfhndl: Device handle
 * @str: Device identifier
 * @irq: irq number
 *
 * This function will acquire an irq for the device
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
qal_vbus_get_irq(struct qdf_pfm_hndl *pfhndl, const char *str, int *irq);

/**
 * qal_vbus_register_driver() - register driver
 * @pfdev: Device handle
 *
 * This function will initialize a device
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
qal_vbus_register_driver(struct qdf_pfm_drv *pfdev);

/**
 * qal_vbus_deregister_driver() - deregister driver
 * @pfdev: Device handle
 *
 * This function will deregister the driver for a device
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
qal_vbus_deregister_driver(struct qdf_pfm_drv *pfdev);
#endif
