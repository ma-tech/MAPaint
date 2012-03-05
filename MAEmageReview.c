#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAEmageReview_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAEmageReview.c
* \author	Richard Baldock
* \date		April 2009
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \ingroup      MAPaint
* \brief        procedures to support EMAGE review of domains
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <MAPaint.h>

extern char *HGU_XmPathToDirectory(
  char	*pathStr);

extern char *HGU_XmPathToFile(
  char	*pathStr);

extern Widget create_view_window_dialog(
  Widget	topl,
  double	theta,
  double        phi,
  WlzDVertex3	*fixed);

extern void read_reference_object_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

WlzErrorNum  installReferenceFromBibfile(
  String		fileStr)
{
  FILE			*fp;
  WlzErrorNum		errNum=WLZ_ERR_READ_EOF;

  /* open file */
  if((fp = fopen(fileStr, "r"))){
    BibFileRecord	*bibfileRecord;
    BibFileError	bibFileErr;
    char		*errMsg;

    /* read the bibfile */
    bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
    while( bibFileErr == BIBFILE_ER_NONE ){

      /* read reference file and install */
      if( !strncmp(bibfileRecord->name, "MAPaintReferenceFile", 20) ){
	int		index;
	char		*fileStr;
	WlzEffFormat	fileType;
	XmFileSelectionBoxCallbackStruct	cbs;

	errNum = WlzEffBibParseFileRecord(bibfileRecord, &index,
					  &fileStr, &fileType);
	if( errNum == WLZ_ERR_NONE ){
	  XmString	xmstr;
	  /* install new reference file - no confirm required */
	  xmstr = XmStringCreateLtoR(fileStr, XmSTRING_DEFAULT_CHARSET);
	  cbs.value = xmstr;
	  cbs.dir = NULL;
	  read_reference_object_cb(globals.topl, (XtPointer) fileType,
				   &cbs);
	  XmStringFree(xmstr);
	}
      }

      /* get next record */
      BibFileRecordFree(&bibfileRecord);
      bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
    }
    (void) fclose(fp);
  }

  return errNum;
}

void installViewFromBibfile(
  String		fileStr,
  ThreeDViewStruct	*view_struct)
{
  WlzThreeDViewStruct	*wlzViewStr= view_struct->wlzViewStr;
  FILE			*fp;
  double		oldScale, newScale, newZeta;
  Display		*dpy = XtDisplay(view_struct->canvas);
  Window		win  = XtWindow(view_struct->canvas);
  Widget		slider;

  /* check view_struct */
  if( view_struct == NULL ){
    /* can do nothing */
    return;
  }

  /* open file */
  if((fp = fopen(fileStr, "r"))){
    BibFileRecord	*bibfileRecord;
    BibFileError	bibFileErr;
    char		*errMsg;

    /* read the bibfile */
    bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
    while( bibFileErr == BIBFILE_ER_NONE ){

      if( !strncmp(bibfileRecord->name, "Wlz3DSectionViewParams", 22) ){
	oldScale = wlzViewStr->scale;

	/* going to reset the section view therefore need to check
	   in any existing domains (discard?) and get the new
	   domains for the new view.
	   Note re-setting done more than once therefore wrap the
	   whole process with the install and re-acquire the domains.
	*/
	/* check for domain changes and clear undo lists */
	if( domainChanged() ){
	  if( HGU_XmUserConfirm(
		view_struct->dialog,
		"The destination image is about to change.\n"
		"    Do you want to install the domains\n"
		"    changed in this view?\n\n"
		"Select Install or Ignore below",
		"Install", "Ignore", 0) ){
	    installViewDomains(view_struct);
	  }
	}
	clearUndoDomains();

	WlzEffBibParse3DSectionViewParamsRecord(bibfileRecord, wlzViewStr);
	view_struct->controlFlag &= ~MAPAINT_FIXED_LINE_SET;

	/* reset the sliders and mode control */
	slider = XtNameToWidget(view_struct->dialog, "*.theta_slider");
	HGU_XmSetSliderValue
	  (slider, (float) (wlzViewStr->theta * 180.0 / WLZ_M_PI));
	slider = XtNameToWidget(view_struct->dialog, "*.phi_slider");
	HGU_XmSetSliderValue
	  (slider, (float) (wlzViewStr->phi * 180.0 / WLZ_M_PI));
	slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
	HGU_XmSetSliderValue
	  (slider, (float) (wlzViewStr->zeta * 180.0 / WLZ_M_PI));

	/* extract the scale and the mode to reset the GUI */
	newScale = wlzViewStr->scale;
	newZeta = wlzViewStr->zeta;
	wlzViewStr->scale = oldScale;
	setViewMode(view_struct, wlzViewStr->view_mode);
	setViewScale(view_struct, newScale, 0, 0);

	/* check the section transform - trap mode/zeta bug
	   note setViewMode will reset zeta to whatever is
	   appropriate for the mode */
	if( fabs(newZeta - wlzViewStr->zeta) > 0.01 ){
	  switch( wlzViewStr->view_mode ){
	  case WLZ_UP_IS_UP_MODE:
	    /* probably should have been in zeta-mode */
	    if( HGU_XmUserConfirm(
		  view_struct->dialog,
		  "Warning: The view-mode and roll angle are inconsistent\n"
		  "    Selecting absolute mode will probably fix the\n"
		  "    problem. To check, select Absolute below and check\n"
		  "    if the input tie-points are correct.\n\n"
		  "    If they are not then re-read the bibfile and\n"
		  "    select up-is-up.\n\n"
		  "    If this still fails then save any work and ask\n"
		  "    emap-tech@hgu.mrc.ac.uk\n"
		  "    If the tie-points are correct then save a new\n"
		  "    bibfile to record the correct values\n\n"
		  "Select Absolute or Up-is-Up below:",
		  "Absolute", "Up-is-Up", 0) ){
	      wlzViewStr->view_mode = WLZ_ZETA_MODE;
	      wlzViewStr->zeta = newZeta;
	    }
	    break;

	  case WLZ_STATUE_MODE:
	    /* probably should have been in zeta-mode */
	    if( HGU_XmUserConfirm(view_struct->dialog,
				  "Warning: The view-mode and roll angle\n"
				  "    are inconsistent. Selecting\n"
				  "    absolute mode will probably fix\n"
				  "    the problem. To check select\n"
				  "    absolute below and check if the\n"
				  "    input tie-points are correct.\n"
				  "    If they are not then re-read the\n"
				  "    bibfile and select statue\n"
				  "    If this still fails then ask\n"
				  "    emap-tech@hgu.mrc.ac.uk for help\n\n"
				  "    If the tie-points are correct then\n"
				  "    save the bibfile to record the\n"
				  "    correct settings\n\n"
				  "Select Absolute or Statue below:",
				  "Absolute", "Statue", 0) ){
	      wlzViewStr->view_mode = WLZ_ZETA_MODE;
	      wlzViewStr->zeta = newZeta;
	    }
	    break;

	  case WLZ_ZETA_MODE:
	    /* should not happen, silently just reset slider */
	    break;

	  default: /* set zeta mode - what else? */
	    wlzViewStr->view_mode = WLZ_ZETA_MODE;
	    wlzViewStr->zeta = newZeta;
	    break;
	  }

	  setViewMode(view_struct, wlzViewStr->view_mode);
	  slider = XtNameToWidget(view_struct->dialog, "*.zeta_slider");
	  HGU_XmSetSliderValue
	    (slider, (float) (wlzViewStr->zeta * 180.0 / WLZ_M_PI));
	  reset_view_struct( view_struct );
	}


	/* redisplay */
	XClearWindow(dpy, win);
	display_view_cb(view_struct->dialog, (XtPointer) view_struct, NULL);
      }


      BibFileRecordFree(&bibfileRecord);
      bibFileErr = BibFileRecordRead(&bibfileRecord, &errMsg, fp);
    }
    (void) fclose(fp);
  }

  return;
}

WlzErrorNum emageReviewInstallFromBibfile(
  String	bibfileStr,
  ThreeDViewStruct	*view_struct)
{
  String	dirStr, str;

  /* check file string */
  if( bibfileStr == NULL ){
    /* something wrong */
    return WLZ_ERR_PARAM_NULL;
  }

  /* change current working directory */
  if((dirStr = HGU_XmPathToDirectory(bibfileStr))){
    if( chdir((const char *) dirStr) ){
      /* report an error */
      HGU_XmUserError(view_struct->dialog,
		      "Rapid-Map or Express-Map install:\n"
		      "  Failed to change current working\n"
		      "  directory (folder). For the next\n"
		      "  save please check the domain file\n"
		      "  names.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
    XtFree(dirStr);
  }

  /* read  bibfile for the reference object and view */
  switch( installReferenceFromBibfile(bibfileStr) ){
  default:
    /* should record an error here */
    return WLZ_ERR_PARAM_DATA;

  case WLZ_ERR_NONE:
    break;

  case WLZ_ERR_READ_EOF:
    /* try and decode reference image from bib-file filename
       search for ".ts", then model number, then type,
       then add directory prefix /opt/MouseAtlas/EMAP_models/
    */
    if((str = strstr(bibfileStr, ".ts"))){
      char	fileStrBuf[128], modelType;
      int	stage, modelNum;
      XmString	xmstr;
      XmFileSelectionBoxCallbackStruct	cbs;
      struct stat	buf;

      if( sscanf(str, ".ts%d.%d.%c", &stage, &modelNum, &modelType) == 3 ){
	/* generate the reference object string */
	switch( modelType ){
	case 'l':
	  sprintf(fileStrBuf, "/opt/MouseAtlas/EMAP_models/ts%02d/embryo_%d_WM_left.wlz",
		  stage, modelNum);
	  break;
	case 'r':
	  sprintf(fileStrBuf, "/opt/MouseAtlas/EMAP_models/ts%02d/embryo_%d_WM_right.wlz",
		  stage, modelNum);
	  break;
	case 'b':
	  /* number of options here */
	  sprintf(fileStrBuf, "/opt/MouseAtlas/EMAP_models/ts%02d/embryo_%d_3D",
		  stage, modelNum);
	  break;
	default:
	  return  WLZ_ERR_PARAM_DATA;
	}
	/* now install the object - check existence first */
	if( stat(fileStrBuf, &buf) == 0 ){
	  xmstr = XmStringCreateLtoR(fileStrBuf, XmSTRING_DEFAULT_CHARSET);
	  cbs.value = xmstr;
	  cbs.dir = NULL;
	  read_reference_object_cb(globals.topl, (XtPointer) WLZEFF_FORMAT_WLZ,
				   &cbs);
	  XmStringFree(xmstr);
	}
	else {
	  return  WLZ_ERR_PARAM_DATA;
	}
      }
      else {
	return  WLZ_ERR_PARAM_DATA;
      }
    }
    else {
      /* give up */
      return  WLZ_ERR_PARAM_DATA;
    }
    break;
  }

  if( view_struct == NULL ){
    /* create a view */
    WlzDVertex3		fixed;
    double		theta, phi;
    Widget		widget;

    fixed.vtX = (globals.obj->domain.p->kol1 + globals.obj->domain.p->lastkl)/2;
    fixed.vtY = (globals.obj->domain.p->line1 + globals.obj->domain.p->lastln)/2;
    fixed.vtZ = (globals.obj->domain.p->plane1 + globals.obj->domain.p->lastpl)/2;
    theta = 0.0;
    phi = 0.0;
    widget = create_view_window_dialog(globals.topl, theta, phi, &fixed);
    view_struct = global_view_list->view_struct;
    installViewFromBibfile(bibfileStr, view_struct);
    XtManageChild( widget );
    XtPopup(XtParent(widget), XtGrabNone);
  }
  else {
    installViewFromBibfile(bibfileStr, view_struct);
  }

  return WLZ_ERR_NONE;
}

void emageReviewCb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
  String	bibfileStr;
  ThreeDViewStruct	*view_struct;
  ViewListEntry		*vl1, *vl2, *vl3;
  int			i, num_overlays;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* get bibfile from file browser */
  if( !(bibfileStr = 
	HGU_XmUserGetFilenameT(globals.topl,
			      "Please select the EMAGE submission bibfile.\n",
			      "OK", "cancel", NULL,
			      globals.origDir, "*.bib", NULL)) ){
    return;
  }

  /* grab the paint-key if necessary clear any domains */
  if( paint_key ){
    view_struct = paint_key;
    paint_key = NULL;
    clearUndoDomains();
    installViewDomains(view_struct);
  }
  else {
    if( global_view_list ){
      view_struct = global_view_list->view_struct;
    }
    else {
      view_struct = NULL;
    }
  }

  /* query save existing domains then clear */
  if( HGU_XmUserConfirm(globals.topl,
			"Before swtching to a new submission do you\n"
			"want to save existing domains?\n",
			"Yes", "No", 1) ){
    /* install any domains currently in a painting view */
    save_all_domains_cb(w, NULL, call_data);
  }
  /* don't need to clear because a new reference object is being installed */
  /* clear_all_domains_cb(w, NULL, call_data); */

  /* delete all views except 1 */
  vl1 = global_view_list;
  vl3 = NULL;
  while( vl1 != NULL ){
    if( vl1->view_struct != view_struct ){
      free_view_struct( vl1->view_struct );
      AlcFree( (void *) vl1->view_struct );
      vl2 = vl1->next;
      AlcFree((void *) vl1);
      vl1 = vl2;
    }
    else {
      vl3 = vl1;
      vl1 = vl3->next;
      vl3->next = NULL;
    }
  }
  global_view_list = vl3;

  /* put into emage mode if required */
  if( !globals.emageFlg ){
    setEMAGEDomainsAndColoursCb(w, client_data, call_data);
  }

  /* install model from bibfile and change directory */
  emageReviewInstallFromBibfile(bibfileStr, view_struct);

  /* read domains
     assume here that the files must be in the local directory and
     that the filenames are set. Note CWD already changed */
  num_overlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;
  for(i=1; i <= num_overlays; i++){
    if( globals.domain_filename[i] ){
      char	*fileStr;
      FILE	*fp;
      WlzObject	*obj;

      /* strip directory component so file is read from CWD */
      fileStr = HGU_XmPathToFile(globals.domain_filename[i]);
      /*AlcFree(globals.domain_filename[i]); not safe */
      globals.domain_filename[i] = fileStr;

      /* now read domain */
      if((fp = fopen(fileStr, "rb"))){
	if((obj = WlzReadObj(fp, &errNum))){
	  setDomain(obj, i, fileStr);
	  WlzFreeObj(obj);
	}
	fclose(fp);
      }
    }
  }

  /* reset views */
  display_all_views_cb(w, NULL, call_data);

  return;
}

