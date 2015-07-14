/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <glib.h>
#include <pthread.h>

#include "setting-storage-async-worker.h"

typedef struct {
	pthread_t tid;
	int alive;

	async_fn fn; /*'fn' Must be MT-safe */
	int fn_ret;
	callback_fn cb;
	SettingStorageUG *ad;

	Ecore_Idler *worker_idler;
} storageUg_worker;

static GHashTable *storage_worker_hashT;

void storageUg_thread_testcancel()
{
	int ret;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_testcancel();
	ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	if (0 != ret) {
		SETTING_TRACE_ERROR("pthread_setcancelstate() Fail(%d)", ret);
		pthread_exit(NULL);
	}
}

static Eina_Bool _async_worker_idler(void *data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("---------------------------------------------thread join BEGIN ");
	storageUg_worker *worker = data;

	retv_if(NULL == data, ECORE_CALLBACK_CANCEL);

	pthread_join(worker->tid, NULL);
	worker->alive = FALSE;

	if (worker->cb)
		worker->cb(worker->fn_ret, worker->ad);



	SETTING_TRACE("---------------------------------------------thread join END ");
	SETTING_TRACE_END;
	return ECORE_CALLBACK_CANCEL;
}

static void *_async_worker_thread(void *data)
{
	int ret;
	storageUg_worker *worker = data;

	retv_if(NULL == data, NULL);

	ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	if (0 != ret) {
		SETTING_TRACE_ERROR("pthread_setcancelstate() Fail(%d)", ret);
		pthread_exit(NULL);
	}

	worker->fn_ret = worker->fn(worker->ad);

	storageUG_STOP_POINT;

	worker->worker_idler = ecore_idler_add(_async_worker_idler, worker);
	if (worker->worker_idler == NULL) {
		SETTING_TRACE_ERROR("worker->worker_idler == NULL");
		worker->alive = FALSE;
		/*----------------------------------- */
		/* work around */
		/*----------------------------------- */
		_async_worker_idler(worker);

	}
	pthread_exit(NULL);
}

static void _async_worker_hash_free_key(gpointer data)
{
	storageUg_worker *worker = data;

	if (worker->alive) {
		pthread_cancel(worker->tid);
		pthread_join(worker->tid, NULL);
	}

	if (worker->worker_idler)
		ecore_idler_del(worker->worker_idler);

	free(worker);
}


bool storageUg_worker_is_running(void *data)
{
	storageUg_worker *worker = data;
	SETTING_TRACE("*** worker->alive ------> %d ", worker->alive);
	return worker->alive;
}

void *storageUg_start_async_worker(async_fn fn, callback_fn cb,
                                   SettingStorageUG *ad)
{
	int ret;
	storageUg_worker *worker;

	retv_if(NULL == fn, NULL);

	if (NULL == storage_worker_hashT) {
		storage_worker_hashT = g_hash_table_new_full(NULL, NULL,
		                                             _async_worker_hash_free_key, NULL);
	}

	worker = calloc(1, sizeof(storageUg_worker));
	if (NULL == worker) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return NULL;
	}
	worker->fn = fn;
	worker->cb = cb;
	worker->ad = ad;

	g_hash_table_add(storage_worker_hashT, worker);

	ret = pthread_create(&worker->tid, NULL, _async_worker_thread, worker);
	warn_if(ret, "phread_create() Fail(%d)", ret);

	worker->alive = TRUE;
	return worker;
}

void storageUg_stop_async_worker(void *worker_id)
{
	storageUg_worker *worker = worker_id;

	ret_if(NULL == worker);

	if (g_hash_table_contains(storage_worker_hashT, worker_id))
		g_hash_table_remove(storage_worker_hashT, worker_id);

	if (0 == g_hash_table_size(storage_worker_hashT)) {
		g_hash_table_destroy(storage_worker_hashT);
		storage_worker_hashT = NULL;
	}
}

void storageUg_stop_async_worker_all(void)
{
	g_hash_table_destroy(storage_worker_hashT);
	storage_worker_hashT = NULL;
}
