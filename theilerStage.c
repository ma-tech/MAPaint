#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas Project					*
*   File       :   theilerStage.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Feb 20 09:45:59 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <MAPaint.h>

void theiler_stage_setup_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  String	newDir;

  /* popup a dialog to get the new theiler reconstructions
     directory */
  newDir = HGU_XmUserGetFilename(globals.topl,
				 "Please input the source directory for the\n"
				 "Edinburgh Mouse Atlas reconstructions.\n"
				 "If you have the CD-ROM this could be:\n"
				 "unix:  /cdrom/ema/reconstructions\n"
				 "Win95: e:\\reconstructions\n",
				 "OK", "cancel", globals.base_theiler_dir,
				 globals.base_theiler_dir, NULL);
  if( newDir ){
    /* clear the old */
    if( globals.base_theiler_dir ){
      AlcFree((void *) globals.base_theiler_dir);
    }
    globals.base_theiler_dir = newDir;

    /* clear the theiler stage so that it is not read in again */
    if( globals.theiler_stage ){
      AlcFree((void *) globals.theiler_stage);
      globals.theiler_stage = NULL;
    }

    /* reset the menu */
    theiler_menu_init(globals.topl);
  }

  return;
}


static void set_theiler_stage_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  String	file_str;
  FILE		*fp;
  WlzObject	*obj;
  
  /* set up current theiler stage global */
  if( globals.theiler_stage ){
    AlcFree((void *) globals.theiler_stage);
  }
  if( globals.theiler_stage = (String) AlcMalloc(strlen(XtName(w)) + 1) ){
    sprintf(globals.theiler_stage, "%s", XtName(w));
  }
  else {
    return;
  }

  /* attempt to read the theiler stage object */
  file_str = (String)
    AlcMalloc(strlen(globals.base_theiler_dir) +
	      strlen(globals.theiler_stage)*2 + 10);
  sprintf(file_str, "%s/%s/%s.wlz", globals.base_theiler_dir,
	  globals.theiler_stage, globals.theiler_stage);
  if( (fp = fopen(file_str, "r")) ){
    if( obj = WlzReadObj( fp, NULL ) ){
      install_paint_reference_object( obj );
      set_topl_title(globals.theiler_stage);
    }
    (void) fclose( fp );
  }

  /* check for a theiler stage feedback object */
  sprintf(file_str, "%s/%s/%s_fb.wlz", globals.base_theiler_dir,
	  globals.theiler_stage, globals.theiler_stage);

  /* open the domain object file and put it in as a 3D feedback option */
  if( (fp = fopen(file_str, "r")) ){
    if( obj = WlzReadObj( fp, NULL ) ){
      if( globals.fb_obj ){
	WlzFreeObj( globals.fb_obj );
      }
      globals.fb_obj = obj;
    }
    (void) fclose( fp );
  }
  AlcFree((void *) file_str);
  setup_ref_display_list_cb(w, NULL, NULL);

  /* setup the anatomy menus */
  set_anatomy_menu(globals.topl);

  return;
}

void theiler_menu_init(
  Widget	topl)
{
  Widget	file_menu, menu_parent, menu, widget;
  MenuItem	*menu_items;
  struct dirent *dp;
  DIR		*dfd;
  FILE		*fp;
  int		stage, num_stages;
  char		*file_str;

  /* get the cascade button to be parent to the menu */
  menu_parent = XtNameToWidget(topl, "*file_menu*theiler_stage");

  /* open the required directory and count the files */
  if( (dfd = opendir(globals.base_theiler_dir)) == NULL ){
    /* failed - should give a warning */
    return;
  }
  num_stages = 0;
  while( (dp = readdir(dfd)) != NULL ){
    if( strncmp(dp->d_name, "ts", 2) == 0 ){
      num_stages++;
    }
  }
  rewinddir(dfd);

  /* create a menu items list and new menu */
  menu_items = (MenuItem *) AlcMalloc(sizeof(MenuItem) * (num_stages+1));
  stage = 0;
  file_str = (String)
    AlcMalloc(strlen(globals.base_theiler_dir) +
	      5*2 + 10);
  while( (dp = readdir(dfd)) != NULL ){
    if( strncmp(dp->d_name, "ts", 2) == 0 ){

      /* check for reference image else ignore */
      sprintf(file_str, "%s/%s/%s.wlz", globals.base_theiler_dir,
	      dp->d_name, dp->d_name);
      if( (fp = fopen(file_str, "r")) == NULL ){
	continue;
      }
      fclose(fp);
      menu_items[stage].name = (String) AlcMalloc(strlen(dp->d_name)+2);
      strcpy(menu_items[stage].name, dp->d_name);
      menu_items[stage].wclass = &xmPushButtonGadgetClass;
      menu_items[stage].mnemonic = '\0';
      menu_items[stage].accelerator = NULL;
      menu_items[stage].accel_text = NULL;
      menu_items[stage].item_set = False;
      menu_items[stage].callback = set_theiler_stage_cb;
      menu_items[stage].callback_data = NULL;
      menu_items[stage].help_callback = NULL;
      menu_items[stage].help_callback_data = NULL;
      menu_items[stage].tear_off_model = XmTEAR_OFF_DISABLED;
      menu_items[stage].radio_behavior = False;
      menu_items[stage].always_one = False;
      menu_items[stage].subitems = NULL;
      stage++;
    }
  }
  AlcFree(file_str);
  closedir(dfd);
  menu_items[stage].name = NULL;
  menu = HGU_XmBuildPulldownMenu(menu_parent,
				 XmTEAR_OFF_DISABLED, False, False,
				 menu_items);

  /* if the theiler stage is set then call the required callbacks */
  if( globals.theiler_stage ){
    if( widget = XtNameToWidget(menu, globals.theiler_stage) ){
      set_theiler_stage_cb(widget, NULL, NULL);
    }
  }

  return;
}

