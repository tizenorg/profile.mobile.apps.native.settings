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
/**
 *@defgroup setting-common-view
 *each UG is able to have multiple views.
 */
#ifndef __SETTING_COMMON_VIEW_H__
#define __SETTING_COMMON_VIEW_H__

#define EXPORT_PUBLIC __attribute__ ((visibility ("default")))

typedef struct _setting_view {
	int (*create)(void *cb);
	int (*destroy)(void *cb);
	int (*update)(void *cb);
	int (*cleanup)(void *cb);
	int (*langchanged)(void *cb);

	int is_create;		/*  1:exist */

} setting_view;
/**
 * @brief Set current loaded view
 *
 * @param view The view to set
 * @return #0 on success, else on failed
 */
int setting_view_node_set_cur_view(setting_view *view);

/**
 * @brief Get current loaded view
 *
 * @return current view
 */
setting_view *setting_view_node_get_cur_view();

/**
 * @brief intialize the node table
 *
 * @return #0 on success, else on failed
 */
int setting_view_node_table_intialize();

/**
 * @brief Register a node for a viwe and its top view
 *
 * @param view The current view
 * @param view The top view of current view
 * @return #0 on success, else on failed
 */
int setting_view_node_table_register(setting_view *view,
									 setting_view *topview);

/**
 * @brief callback invoked when pressed hard end key
 *
 * @Deprecated
 */
int setting_view_cb_at_endKey(void *cb);

/**
 * @brief Get top view of certain view
 *
 * @param view The certain view
 * @return #view's top view on success, else NULL on failed
 */
setting_view *setting_view_get_topview(setting_view *view);

/**
 * @brief Replace top view of the certain view
 *
 * @param view The certain view
 * @param topview The new top view
 */
extern void setting_view_update_topview(setting_view *view, setting_view *topview);

/**
 * @brief Callback of view creating
 *
 * @param view The created view
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 */
extern int setting_view_create(setting_view *view, void *cb);

/**
 * @brief Callback of view updating
 *
 * @param view The view being updated
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 * @warning the function should be invoked on the view which will be toppest view
 */
extern int setting_view_update(setting_view *view, void *cb);

/**
 * @brief Callback of view destroying
 *
 * @param view The view being destroyed
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 */
extern int setting_view_destroy(setting_view *view, void *cb);

/**
 * @brief Callback of view cleanuping
 *
 * @param view The view being cleanuped
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 * @warning the function should be invoked on the view which will be covered
 */
extern int setting_view_cleanup(setting_view *view, void *cb);

/**
 * @brief Callback of view changing
 *
 * @param from_view The view being covered
 * @param to_view The view go to cover
 * @param cb The view data passed between all callbacks
 * @return #0 on success, else on failed
 * @warning the function should be invoked to change views
 */
extern int setting_view_change(setting_view *from_view, setting_view *to_view,
							   void *cb);

#endif				/* __SETTING_COMMON_VIEW_H__ */
