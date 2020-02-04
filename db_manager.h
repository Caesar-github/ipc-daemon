// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __DB_MANAGER_H__
#define __DB_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

int db_manager_init_db(void);
int db_manager_reset_db(void);
int db_manager_import_db(char *db_path);
int db_manager_export_db(char *db_path);

#ifdef __cplusplus
}
#endif

#endif
