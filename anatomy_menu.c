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
*   File       :   anatomy_menu.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Feb 16 08:23:00 1998				*
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

static MenuItem anatomy_menu_itemsP[] = {	/* anatomy_menu items */
  NULL,
};

MenuItem	*anatomy_menu_items = &(anatomy_menu_itemsP[0]);

typedef struct _AnatObjItem {
  String	shortName;
  String	fullName;
  WlzObject	*domain;
} AnatObjItem;

static AnatObjItem *anatObjItems=NULL;
static int numAnatObjItems=0;
static int maxAnatObjItems=0;

String getAnatNameFromCoord(
  int k,
  int l,
  int p)
{
  String 	name="no anatomy";
  int		i;

  for(i=0; i < numAnatObjItems; i++){
    if( anatObjItems[i].domain && WlzInsideDomain(anatObjItems[i].domain,
			(double) p, (double) l, (double) k, NULL) ){
      return anatObjItems[i].shortName;
    }
  }

  return name;
}


void read_anatomy_cb(
  Widget	widget,
  XtPointer	client_data,
  XtPointer	call_data)
{
  String	fileStr = (String) client_data;
  char		anatStr[64];
  FILE		*fp;
  WlzObject	*anatObj;

  /* open the object file and read the domain */
  if( fileStr ){
    if( (fp = fopen(fileStr, "r")) == NULL ){
      /* should say something here */
      return;
    }
  }
  else {
    /* should say something here */
    return;
  }
  if( (anatObj = WlzReadObj(fp, NULL)) == NULL ){
    /* should say something here */
    (void) fclose(fp);
    return;
  }
  (void) fclose(fp);

  /* display the object in 3D */
  MAOpenGLDisplayDomainIndex(anatObj, globals.current_domain);

  /* set the anatomy as a domain */
  if( globals.current_domain > globals.cmapstruct->num_overlays )
  {
    set_grey_values_from_domain( anatObj, globals.obj,
				globals.current_col, 255);
  }
  else
  {
    set_grey_values_from_domain(anatObj, globals.obj, globals.current_col,
				globals.cmapstruct->ovly_planes);
  }
  WlzFreeObj(anatObj);
  if( paint_key ){
    display_view_cb(widget, (XtPointer) paint_key, call_data);
  }
  display_all_views_cb(widget, (XtPointer) NULL, call_data);

  /* set the domain menu entry */
  sscanf(XtName(widget), "%s", anatStr);
  set_domain_menu_entry(globals.current_domain, anatStr);

  return;
}

void freeAnatomyMenuItems(
  MenuItem	*menu_items)
{
  int	itemIndex;

  /* termination condition */
  if( menu_items == NULL ){
    return;
  }

  /* for each item remove the sub-items */
  itemIndex = 0;
  while( menu_items[itemIndex].name ){
    /* should be able to free the name but the widget
       is still using it */

    /* free any sub-items */
    if( menu_items[itemIndex].subitems ){
      freeAnatomyMenuItems(menu_items[itemIndex].subitems);
    }
    itemIndex++;
  }

  /* free this array */
  AlcFree((void *) menu_items);

  return;
}

MenuItem *createAnatomyMenuItems(
  String	name,
  String	dirStr)
{
  MenuItem	*items=NULL;
  struct dirent *dp;
  DIR		*dfd;
  int		numDirs, dirIndex;
  String	newDirStr;
  String	objFileStr=NULL, infoFileStr=NULL;
  String	str;
  struct stat	statBuf;
  char		strBuf[256];

  /* open the directory and count the subdirectories */
  if( (dfd = opendir(dirStr)) == NULL ){
    return NULL;
  }
  numDirs = 0;
  while( (dp = readdir(dfd)) != NULL ){
    if((strcmp(dp->d_name, ".") == 0) ||
       (strcmp(dp->d_name, "..") == 0)){
      continue;
    }
    newDirStr = (String) AlcMalloc(strlen(dp->d_name)+strlen(dirStr)+2);
    sprintf(newDirStr, "%s/%s", dirStr, dp->d_name);
    if((stat(newDirStr, &statBuf) == -1) ||
       ((statBuf.st_mode&S_IFMT) != S_IFDIR) ){
      /* its an ordinary file - check for object or info file */
      sprintf(strBuf, "%s.wlz", name);
      if( strcmp(dp->d_name, strBuf) == 0 ){
	FILE	*fp;
	objFileStr = newDirStr;

	/* add it to the list */
	if( numAnatObjItems >= maxAnatObjItems ){
	  maxAnatObjItems += 64;
	  anatObjItems = (AnatObjItem *)
	    AlcRealloc(anatObjItems,  sizeof(AnatObjItem)*maxAnatObjItems);
	}
	anatObjItems[numAnatObjItems].shortName = 
	  (String)AlcMalloc(strlen(name)+1);
	strcpy(anatObjItems[numAnatObjItems].shortName, name);
	anatObjItems[numAnatObjItems].fullName = 
	  (String)AlcMalloc(strlen(dirStr)+1);
	strcpy(anatObjItems[numAnatObjItems].fullName, dirStr);
	if( fp = fopen(objFileStr, "r") ){
	  anatObjItems[numAnatObjItems].domain = WlzReadObj(fp, NULL);
	  (void) fclose(fp);
	}
	numAnatObjItems++;

	continue;
      }
      
      sprintf(strBuf, "%s.html", name);
      if( strcmp(dp->d_name, strBuf) == 0 ){
	infoFileStr = newDirStr;
	continue;
      }
    }
    AlcFree((void *) newDirStr);
    numDirs++;
  }
  rewinddir(dfd);

  /* allocate space */
  items = (MenuItem *) AlcMalloc(sizeof(MenuItem) * (numDirs+4));

  /* fill in items, calling recursively as required, attaching
     object filename if present */
  dirIndex = 0;
  /* the information button */
  if( infoFileStr ){
    str = (String) AlcMalloc(strlen(name) + 8);
    sprintf(str, "%s info.", name);
    items[dirIndex].name = str;
    items[dirIndex].wclass = &xmPushButtonGadgetClass;
    items[dirIndex].mnemonic = '\0';
    items[dirIndex].accelerator = NULL;
    items[dirIndex].accel_text = NULL;
    items[dirIndex].item_set = False;
    items[dirIndex].callback = NULL;
    items[dirIndex].callback_data = infoFileStr;
    items[dirIndex].help_callback = NULL;
    items[dirIndex].help_callback_data = NULL;
    items[dirIndex].tear_off_model = XmTEAR_OFF_DISABLED;
    items[dirIndex].radio_behavior = False;
    items[dirIndex].always_one = False;
    items[dirIndex].subitems = NULL;
    dirIndex++;
  }

  /* select the woolz domain */
  if( objFileStr ){
    str = (String) AlcMalloc(strlen(name) + 8);
    sprintf(str, "%s domain", name);
    items[dirIndex].name = str;
    items[dirIndex].wclass = &xmPushButtonGadgetClass;
    items[dirIndex].mnemonic = '\0';
    items[dirIndex].accelerator = NULL;
    items[dirIndex].accel_text = NULL;
    items[dirIndex].item_set = False;
    items[dirIndex].callback = read_anatomy_cb;
    items[dirIndex].callback_data = objFileStr;
    items[dirIndex].help_callback = NULL;
    items[dirIndex].help_callback_data = NULL;
    items[dirIndex].tear_off_model = XmTEAR_OFF_DISABLED;
    items[dirIndex].radio_behavior = False;
    items[dirIndex].always_one = False;
    items[dirIndex].subitems = NULL;
    dirIndex++;
  }

  /* dividing line */
  if( dirIndex && numDirs){
    items[dirIndex].name = "";
    items[dirIndex].wclass = &xmSeparatorGadgetClass;
    items[dirIndex].mnemonic = '\0';
    items[dirIndex].accelerator = NULL;
    items[dirIndex].accel_text = NULL;
    items[dirIndex].item_set = False;
    items[dirIndex].callback = NULL;
    items[dirIndex].callback_data = NULL;
    items[dirIndex].help_callback = NULL;
    items[dirIndex].help_callback_data = NULL;
    items[dirIndex].tear_off_model = XmTEAR_OFF_DISABLED;
    items[dirIndex].radio_behavior = False;
    items[dirIndex].always_one = False;
    items[dirIndex].subitems = NULL;
    dirIndex++;
  }

  /* now the subdirectories */
  while( (dp = readdir(dfd)) != NULL ){
    if((strcmp(dp->d_name, ".") == 0) ||
       (strcmp(dp->d_name, "..") == 0)){
      continue;
    }
    newDirStr = (String) AlcMalloc(strlen(dp->d_name)+strlen(dirStr)+2);
    sprintf(newDirStr, "%s/%s", dirStr, dp->d_name);
    if((stat(newDirStr, &statBuf) == -1) ||
       ((statBuf.st_mode&S_IFMT) != S_IFDIR) ){
      /* its an ordinary file */
      continue;
    }
    str = (String) AlcMalloc(strlen(dp->d_name) + 1);
    sprintf(str, "%s", dp->d_name);
    items[dirIndex].name = str;
    items[dirIndex].wclass = &xmCascadeButtonWidgetClass;
    items[dirIndex].mnemonic = '\0';
    items[dirIndex].accelerator = NULL;
    items[dirIndex].accel_text = NULL;
    items[dirIndex].item_set = False;
    items[dirIndex].callback = NULL;
    items[dirIndex].callback_data = NULL;
    items[dirIndex].help_callback = NULL;
    items[dirIndex].help_callback_data = NULL;
    items[dirIndex].tear_off_model = XmTEAR_OFF_DISABLED;
    items[dirIndex].radio_behavior = False;
    items[dirIndex].always_one = False;
    items[dirIndex].subitems = createAnatomyMenuItems(dp->d_name,
						       newDirStr);
    dirIndex++;
    
    AlcFree((void *) newDirStr);
  }

  /* set the last item to NULL */
  items[dirIndex].name = NULL;

  /* close directory and free space */
  closedir(dfd);
  if( dirIndex == 0 ){
    AlcFree((void *) items);
    items = NULL;
  }

  return items;
}

/* set the anatomy menu by creating a hierarchy of MenuItem
   structures then call HGU_XmBuildPulldownMenu */
void set_anatomy_menu(
  Widget	topl)
{
  struct dirent *dp;
  DIR		*dfd;
  Widget	menu_button, menu;
  MenuItem	*menu_items;
  String	theilerDir, dirStr;
  
  /* open the current theiler stage directory */
  if( globals.theiler_stage ){
    theilerDir = (String) AlcMalloc(strlen(globals.base_theiler_dir) +
				    strlen(globals.theiler_stage) + 2);
    sprintf(theilerDir, "%s/%s", globals.base_theiler_dir,
	    globals.theiler_stage);
    if( (dfd = opendir(theilerDir)) == NULL ){
      /* failed - should give a warning */
      AlcFree((void *) theilerDir);
      return;
    }
  }
  else {
    return;
  }

  /* check the anatomy directory exists it is defined to have the name
     "anatomy" and must be a directory */
  while( (dp = readdir(dfd)) != NULL ){
    if( strcmp(dp->d_name, "anatomy") == 0 ){
      struct stat statBuf;

      dirStr = (String) AlcMalloc(strlen(theilerDir) +
				  strlen(dp->d_name) + 2);
      (void) sprintf(dirStr, "%s/%s", theilerDir, dp->d_name);

      /* check it is a directory */
      if((stat(dirStr, &statBuf) == -1) ||
	 ((statBuf.st_mode&S_IFMT) != S_IFDIR) ){
	AlcFree((void *) dirStr);
	dp = NULL;
      }
      break;
    }
  }
  if( dp == NULL ){
    /* no anatomy directory, should give warning */
    closedir(dfd);
    AlcFree((void *) theilerDir);
    return;
  }

  /* recursively create the menu item arrays */
  menu_button = XtNameToWidget(topl, "*menubar*anatomy_menu");
  menu_items = createAnatomyMenuItems(dp->d_name, dirStr);

  /* if the menu_button has a sub-menu then destroy it */
  XtVaGetValues(menu_button, XmNsubMenuId, &menu, NULL);
  if( menu ){
    XtDestroyWidget( menu );
  }

  /* create the menu */
  menu = HGU_XmBuildPulldownMenu(menu_button, XmTEAR_OFF_DISABLED,
				 False, False, menu_items);
  XtSetSensitive(menu_button, True);

  /* free space */
  freeAnatomyMenuItems(menu_items);
  AlcFree((void *) dirStr);
  AlcFree((void *) theilerDir);
  closedir(dfd);

  return;
}

/* anatomy_menu initialisation procedure */
void anatomy_menu_init(
  Widget	topl)
{
  /* do nothing here because if the theiler stage has been set
     then the theiler_menu_init procedure will have set up the anatomy
     menu via set_theiler_stage_cb */
  return;
}
