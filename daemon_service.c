// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "daemon_service.h"
#include "common.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <glib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define PROCESS_NUM 4
struct process {
  char *name;
  char *args;
};
static const struct process process_list[PROCESS_NUM] = {
    {"dbserver", ""},
    {"netserver", ""},
    {"storage_manager", ""},
    {"mediaserver", "-S -c /oem/usr/share/mediaserver/mediaserver.conf"}};

static bool program_running(const struct process *service) {
  FILE *fp;
  char cmd[128], buf[1024];
  char *pLine;

  assert(service);
  assert(service->name);
  snprintf(cmd, sizeof(cmd), "/bin/ps | /bin/grep  %s | /bin/grep -v grep",
           service->name);
  fp = popen(cmd, "r");
  if (!fp)
    return false;
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    pLine = strtok(buf, "\n");
    while (pLine) {
      if (strstr(pLine, service->name)) {
        pclose(fp);
        return true;
      }
      pLine = strtok(NULL, "\n");
    }
  }
  pclose(fp);
  return false;
}

static void program_run(const struct process *service) {
  char cmd[64] = {0};

  assert(service);
  assert(service->name && service->args);
  snprintf(cmd, sizeof(cmd), "%s %s &", service->name, service->args);
  LOG("start: %s\n", cmd);
  system(cmd);
}

static void program_kill(const struct process *service) {
  char cmd[64] = {0};

  assert(service);
  assert(service->name && service->args);

  snprintf(cmd, sizeof(cmd), "killall %s", service->name);
  system(cmd);
}

static void program_force_kill(const struct process *service) {
  char cmd[64] = {0};

  assert(service);
  assert(service->name && service->args);

  snprintf(cmd, sizeof(cmd), "killall -9 %s", service->name);
  system(cmd);
}

static gboolean program_check_and_run(gpointer user_data) {
  for (int i = 0; i < PROCESS_NUM; i++) {
    if (!program_running(&process_list[i]))
      program_run(&process_list[i]);
  }
  return TRUE;
}

static gint timeout_tag;

int daemon_services_start(unsigned int timer_ms) {
  program_check_and_run(NULL);

  timeout_tag = g_timeout_add(timer_ms, program_check_and_run, NULL);
  return 0;
}

int daemon_services_stop(void) {
  const int wait_time_ms = 100;
  int wait_count = 10;

  if (!timeout_tag)
    return -1;

  g_source_remove(timeout_tag);
  timeout_tag = 0;

  while (--wait_count > 0) {
    /* killall process */
    for (int i = 0; i < PROCESS_NUM; i++) {
      if (program_running(&process_list[i]))
        program_kill(&process_list[i]);
    }
    usleep(wait_time_ms * 1000);
    /* check process */
    bool succeed = true;
    int i;
    for (i = 0; i < PROCESS_NUM; i++)
      if (program_running(&process_list[i]))
        break;
    if (i == PROCESS_NUM)
      break;
  }

  /* if failed to killall process, force killall proces */
  if (wait_count == 0) {
    for (int i = 0; i < PROCESS_NUM; i++) {
      if (program_running(&process_list[i]))
        program_force_kill(&process_list[i]);
    }
    usleep(100 * 1000);
  }
  return 0;
}
