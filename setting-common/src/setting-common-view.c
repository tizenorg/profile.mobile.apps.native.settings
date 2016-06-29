/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
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
#include <setting-common-view.h>

#include <Elementary.h>

#include <setting-common-data-error.h>
#include <setting-debug.h>

#define MAX_VIEWNODE_NUM	15
typedef struct _SettingViewNode {
	setting_view *view;
	setting_view *topview;
} SettingViewNode;
static setting_view *g_cur_view; /* use by 'End Key' process */
static SettingViewNode g_view_node_table[MAX_VIEWNODE_NUM];
static int g_view_node_table_cur_size = 0;

/**
 * @brief Set current loaded view
 *
 * @param view The view to set
 * @return #0 on success, else on failed
 */
EXPORT_PUBLIC int setting_view_node_set_cur_view(setting_view *view)
{
	g_cur_view = view;
	return 0;
}
/**
 * @brief Get current loaded view
 *
 * @return current view
 */
EXPORT_PUBLIC
setting_view *setting_view_node_get_cur_view()
{
	return g_cur_view;
}

/**
 * @brief intialize the node table
 *
 * @return #0 on success, else on failed
 */
EXPORT_PUBLIC int setting_view_node_table_intialize()
{
	g_cur_view = NULL;
	g_view_node_table_cur_size = 0;
	return 0;
}

/**
 * @brief Register a node for a viwe and its top view
 *
 * @param view The current view
 * @param view The top view of current view
 * @return #0 on success, else on failed
 */
EXPORT_PUBLIC int setting_view_node_table_register(setting_view *view,
		setting_view *topview)
{
	/*SETTING_TRACE_BEGIN; */
	if (g_view_node_table_cur_size >= MAX_VIEWNODE_NUM)
		return SETTING_RETURN_FAIL;

	int idx = 0;
	for (; idx < g_view_node_table_cur_size; idx++) {
		if (view == g_view_node_table[idx].view
				&& topview == g_view_node_table[idx].topview) {
			SETTING_TRACE("view node has been registered, ignore");
			return SETTING_RETURN_FAIL;
		}
	}

	g_view_node_table[g_view_node_table_cur_size].view = view;
	g_view_node_table[g_view_node_table_cur_size].topview = topview;
	g_view_node_table_cur_size++;
	return 0;
}

/**
 * @brief callback invoked when pressed hard end key
 *
 * @Deprecated
 */
EXPORT_PUBLIC
int setting_view_cb_at_endKey(void *cb)
{
	SETTING_TRACE_BEGIN;
	int idx = 0;
	SettingViewNode *viewnode = NULL;
	for (; idx < g_view_node_table_cur_size; idx++) {
		if (g_cur_view == g_view_node_table[idx].view) {
			viewnode = &(g_view_node_table[idx]);
			break;
		}
	}
	if (viewnode && viewnode->view && viewnode->topview)
		setting_view_change(viewnode->view, viewnode->topview, cb);

	return 0;
}

/**
 * @brief Get top view of certain view
 *
 * @param view The certain view
 * @return #view's top view on success, else NULL on failed
 */
EXPORT_PUBLIC
setting_view *setting_view_get_topview(setting_view *view)
{
	SETTING_TRACE_BEGIN;
	retv_if(NULL == view, NULL);
	int idx = 0;
	SettingViewNode *viewnode = NULL;
	for (; idx < g_view_node_table_cur_size; idx++) {
		if (view == g_view_node_table[idx].view) {
			viewnode = &(g_view_node_table[idx]);
			break;
		}
	}
	if (viewnode && viewnode->topview)
		return viewnode->topview;
	else
		return NULL;

	SETTING_TRACE_END;
}

/**
 * @brief Replace top view of the certain view
 *
 * @param view The certain view
 * @param topview The new top view
 */
EXPORT_PUBLIC
void setting_view_update_topview(setting_view *view, setting_view *topview)
{
	SETTING_TRACE_BEGIN;
	ret_if(NULL == view);
	int idx = 0;
	SettingViewNode *viewnode = NULL;
	for (; idx < g_view_node_table_cur_size; idx++) {
		if (view == g_view_node_table[idx].view) {
			viewnode = &(g_view_node_table[idx]);
			break;
		}
	}
	if (viewnode)
		viewnode->topview = topview;
}

/**
 * @brief Callback of view creating
 *
 * @param view The created view
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 */
EXPORT_PUBLIC
int setting_view_create(setting_view *view, void *cb)
{
	LAUNCH_SETTING_IN();
	/* error check */
	setting_retvm_if(!view || !cb, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"Invalid arguement");

	int ret = SETTING_RETURN_FAIL;

	/*error handle:create only when the view doesn't exit */
	if (!view->is_create && view->create)
		ret = view->create(cb);

	LAUNCH_SETTING_OUT();
	return ret;
}

/**
 * @brief Callback of view destroying
 *
 * @param view The view being destroyed
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 */
EXPORT_PUBLIC int setting_view_destroy(setting_view *view, void *cb)
{
	/* error check */
	setting_retvm_if(!view || !cb, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"Invalid arguement");

	int ret = SETTING_RETURN_FAIL;

	/*error handle:destroy only when the view exits */
	if (view->is_create && view->destroy)
		ret = view->destroy(cb);

	return ret;
}
/**
 * @brief Callback of view updating
 *
 * @param view The view being updated
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 * @warning the function should be invoked on the view which will be toppest
 *	view
 */
EXPORT_PUBLIC int setting_view_update(setting_view *view, void *cb)
{
	/* error check */
	setting_retvm_if(!view || !cb, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"Invalid arguement");

	int ret = SETTING_RETURN_FAIL;

	/*error handle:update only when the view exits */
	if ((view->is_create == TRUE) && view->update)
		ret = view->update(cb);

	return ret;
}

/**
 * @brief Callback of view cleanuping
 *
 * @param view The view being cleanuped
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 * @warning the function should be invoked on the view which will be covered
 */
EXPORT_PUBLIC
int setting_view_cleanup(setting_view *view, void *cb)
{
	/* error check */
	setting_retvm_if(!view || !cb, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"Invalid arguement");

	int ret = SETTING_RETURN_FAIL;

	/*error handle:cleanup only when the view exits */
	if (view->is_create && view->cleanup)
		ret = view->cleanup(cb);

	return ret;
}

/**
 * @brief Callback of view changing
 *
 * @param from_view The view being covered
 * @param to_view The view go to cover
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 * @warning the function should be invoked to change views
 */
EXPORT_PUBLIC
int setting_view_change(setting_view *from_view, setting_view *to_view,
		void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(!from_view || !to_view || !cb,
			SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"Invalid arguement");

	int ret = SETTING_RETURN_SUCCESS;
	/*from a parent view to his child view, don't need cleanup
	 * parent view;
	 *from a child view to his parent view, need cleanup child
	 * view */
	if (from_view != setting_view_get_topview(to_view))
		ret = setting_view_cleanup(from_view, cb);

	retv_if(ret == SETTING_RETURN_FAIL, SETTING_RETURN_FAIL);

	if (to_view->is_create == 1)
		ret = setting_view_update(to_view, cb);
	else
		ret = setting_view_create(to_view, cb);

	g_cur_view = to_view; /* compute new value of g_cur_view. */
	SETTING_TRACE_END;
	return ret;
}
