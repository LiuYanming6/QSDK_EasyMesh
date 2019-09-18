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
 * DOC: qdf_debugfs.h
 * This file provides OS abstraction for debug filesystem APIs.
 */

#ifndef _QDF_DEBUGFS_H
#define _QDF_DEBUGFS_H

#include <qdf_status.h>
#include <i_qdf_debugfs.h>

/* representation of qdf dentry */
typedef __qdf_dentry_t qdf_dentry_t;

#if defined(CONFIG_KGDB)
extern void qdf_kgdb_breakpoint(void);
extern void kgdb_breakpoint(void);
#endif

#if WLAN_DEBUGFS

/**
 * qdf_debugfs_remove_file() - remove debugfs file
 * @d: debugfs node
 *
 */
void qdf_debugfs_remove_file(qdf_dentry_t d);

/**
 * qdf_debugfs_remove_file() - create a debugfs file that is used to read a
 * binary blob
 * @name: A pointer to a string containing the name of the file to create.
 * @mode: The permission that the file should have.
 * @parent: A pointer to the parent dentry for this file. This should be a
 * directory dentry if set. If this parameter is NULL, then the file
 * will be created in the root of the debugfs filesystem.
 * @blob: A pointer to a struct debugfs_blob_wrapper which contains a pointer
 * to the blob data and the size of the data.
*/
struct dentry *qdf_debugfs_create_blob(const char *name,
			     umode_t mode,
			     struct dentry *parent,
			     struct debugfs_blob_wrapper * blob);
#else

static inline void qdf_debugfs_remove_file(qdf_dentry_t d)
{
}

static inline struct dentry *qdf_debugfs_create_blob(const char *name,
					   umode_t mode,
					   struct dentry *parent,
					   struct debugfs_blob_wrapper * blob)
{
	return NULL;
}
#endif /* WLAN_DEBUGFS */
#endif /* _QDF_DEBUGFS_H */
