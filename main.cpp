// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <gdbus.h>
#include <glib.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "daemon_service.h"
#include "db_manager.h"
#include "system_manager.h"

int main(int argc, char **argv) {
  GMainLoop *main_loop;

  /* 1. check db file */
  db_manager_init_db();

  /* 2. start services and check services status periodically*/
  daemon_services_start(SERVICE_CHECK_PERIOD_MS);

  /* 3. start system services, process system command */
  system_manager_init();

  main_loop = g_main_loop_new(NULL, FALSE);

  g_main_loop_run(main_loop);
  g_main_loop_unref(main_loop);

  return 0;
}
