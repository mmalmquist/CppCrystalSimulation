#include <gtk/gtk.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <iostream>
#include <memory>

#include "CrystalModel.hpp"
#include "CrystalView.hpp"
#include "CrystalControl.hpp"

#include "root_directory.h"

static std::unique_ptr<CrystalControl> m_cc;

static void
close_window_cb(void)
{
  gtk_main_quit();
}

static int
cli_sim(size_t size)
{
  int m_r_start = size/2;
  int m_r_escape = 11 * m_r_start / 10;
  int m_bath_width = 2 * (m_r_escape + 2);
  CrystalModel cm(m_bath_width, m_r_start, m_r_escape);
  while (cm.crystallize_one_ion()) {
  }
  std::cout << cm.to_string() << std::endl;
  return EXIT_SUCCESS;
}

static int
gui_sim(int argc,
	char *argv[],
	size_t size)
{
  if (!gtk_init_check(&argc, &argv)) {
    fprintf(stderr, "Failed to init GTK. Exiting...\n");
    return EXIT_FAILURE;
  }
  
  int m_r_start = size/2;
  int m_r_escape = 11 * m_r_start / 10;
  int m_bath_width = 2 * (m_r_escape + 2);
  std::shared_ptr<CrystalModel> cm = std::make_shared<CrystalModel>(m_bath_width, m_r_start, m_r_escape);
  std::shared_ptr<CrystalView> cv = std::make_shared<CrystalView>(cm);
  
  m_cc = std::make_unique<CrystalControl>(cm, cv);
  
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (!gtk_builder_add_from_file(builder, SYS_PATH("/res/crystal_experiment.ui"), &error)) {
    fprintf(stderr, "Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return EXIT_FAILURE;
  }
  
  m_cc->init_ui(builder);

  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "app-window")),
		   "destroy", G_CALLBACK(close_window_cb), NULL);
  
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(G_OBJECT(builder));
  
  gtk_main();
  return EXIT_SUCCESS;
}

int
main(int argc,
     char *argv[])
{
  int size = 0;
  char mode[32]; memset(mode, 0, 32);
  for (int i = 1; i < argc; ++i) {
    if (strncmp("mode=", argv[i] , 5) == 0) {
      strncpy(mode, argv[i]+5, 32);
    } else if (strncmp(argv[i], "size=", 5) == 0) {
      size = atoi(argv[i] + 5);
    }
  }
  if (strlen(mode) == 0) {
    strncpy(mode, "cli", 32);
    printf("INFO: mode has been set to '%s'\n", mode);
  }
  if (size < 20) {
    size = 20;
    printf("INFO: size has been set to '%d'\n", size);
  }

  if (strncmp("cli", mode, 3) == 0) {
    return cli_sim(size);
  } else if (strncmp("gui", mode, 3) == 0) {
    return gui_sim(argc-2, argv, size);
  } else {
    printf("usage: '%s mode=[cli/gui] size=[<value>]'\n", argv[0]);
  }
  return EXIT_SUCCESS;
}
