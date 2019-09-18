/*
 * Copyright (c) 2008-2010, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __ATD_TGT_WSUPP_H
#define __ATD_TGT_WSUPP_H

#include <linux/version.h>

#include <a_base_types.h>

/* exported functions/structures */
typedef void * wsupp_handle_t;

/**
 * @brief Initialize the wsupp helper for target device
 *
 * @param dev
 * @param htc_handle
 *
 * @return
 */
wsupp_handle_t
atd_tgt_wsupp_helper_init(void *tdev, htc_handle_t htc_handle);

/**
 * @brief Cleanup the wsupp helper
 *
 * @param wsupp_handle
 */
void
atd_tgt_wsupp_helper_cleanup(wsupp_handle_t wsupp_handle);


/**
 * @brief Send WMI commands to wsupp
 *
 * @param wsupp_handle
 */
int
atd_tgt_wsupp_helper_wmi_send(wsupp_handle_t wsupp_handle,
        int cmdid, char *data, int datalen);

/**
 * @brief Send WMI response
 *
 * @param wsupp_handle
 */
int
atd_tgt_wsupp_response(void *ctx, char *data, int datalen);


/* private functions/structures */
#define wh_assert(expr)  do {    \
    if(unlikely(!(expr))) {                                 \
        printk(KERN_ERR "Assertion failed! %s:%s %s:%d\n",   \
              #expr, __FUNCTION__, __FILE__, __LINE__);      \
        dump_stack();                                      \
        panic("Take care of the assert first\n");          \
    }     \
}while(0)

/* worker functions for wsupp helper */

typedef void (*wsupp_helper_work_fn_t)(void *);

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19)
typedef struct work_struct wsupp_helper_work_t;
static inline void wsupp_helper_sched_work(wsupp_helper_work_t *work)
{
    schedule_work(work);
}
static inline void
wsupp_helper_init_work(wsupp_helper_work_t *work, wsupp_helper_work_fn_t func,
        void *arg)
{
    INIT_WORK(work, func, arg);
}
#else
typedef struct {
    struct work_struct      work;
    wsupp_helper_work_fn_t  fn;
    void                    *arg;
} wsupp_helper_work_t;
static inline void wsupp_helper_sched_work(wsupp_helper_work_t *work)
{
    schedule_work(&work->work);
}
static inline void wsupp_helper_work_defer_func(struct work_struct *work)
{
    wsupp_helper_work_t *wh_work = container_of(work, wsupp_helper_work_t, work);
    wh_work->fn(wh_work->arg);
}
static inline void
wsupp_helper_init_work(wsupp_helper_work_t *work, wsupp_helper_work_fn_t func,
        void *arg)
{
    work->fn = func ;
    work->arg  = arg  ;
    INIT_WORK(&work->work, wsupp_helper_work_defer_func);
}
#endif

#endif
