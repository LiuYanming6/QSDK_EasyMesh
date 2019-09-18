/*
 * auxwget.h
 *
 *  Created on: Aug 7, 2009
 *      Author: dmounday
 */

#ifndef AUXWGET_H_
#define AUXWGET_H_

int wget_GetFile(const char *url, const char *filename, EventHandler callback, void *handle);
int wget_PostXml(const char *url, char *data, int len, EventHandler callback, void *handle);
int wget_PostXmlFile(const char *url, char *data, int len, const char *filename, EventHandler callback, void *handle);

#endif /* AUXWGET_H_ */
