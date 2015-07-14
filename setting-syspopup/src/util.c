/*
 * Copyright (c) 2013-2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability
 * of the software, either express or implied, including but not
 * limited to the implied warranties of merchantability, fitness for
 * a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as
 * a result of using, modifying or distributing this software or its derivatives.
 */

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <util.h>

/* using at operation start time */
int export_xml(char *path)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "mode-syspopup");
	xmlDocSetRootElement(doc, root_node);
	xmlKeepBlanksDefault(0);
	xmlIndentTreeOutput = 1;
	int rel = xmlSaveFormatFileEnc(path, doc, "UTF-8", 1);

	if (rel != -1) {
		_DBG("success");
		xmlFreeDoc(doc);
	} else {
		_DBG("save xml file failed");
		return -1;
	}
	/*success */
	return 0;
}

/* using at operation end time */
int remove_xml(char *path)
{
	if (remove(path) == 0) {
		/*sucess */
		_DBG("success");
		return 0;
	}
	_DBG("fail with remove");
	return -1;
}
