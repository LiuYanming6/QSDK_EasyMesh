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
 * DOC: qdf_debugfs
 * This file provides QDF debug file system APIs
 */

#include <qdf_debugfs.h>
#include <i_qdf_debugfs.h>

/* entry for root debugfs directory*/
#if defined(CONFIG_KGDB)
extern void qdf_kgdb_breakpoint()
{
	kgdb_breakpoint();
}
#endif

#if WLAN_DEBUGFS

void qdf_debugfs_remove_file(qdf_dentry_t d)
{
        debugfs_remove(d);
}
struct dentry *qdf_debugfs_create_blob(const char *name,
			     umode_t mode,
			     struct dentry *parent,
			     struct debugfs_blob_wrapper * blob)
{
	return debugfs_create_blob(name, mode, parent, blob);
}

#endif
