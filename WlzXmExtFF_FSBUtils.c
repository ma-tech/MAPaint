
#include <X11/keysym.h>
#include <Xm/XmAll.h>

#include <Wlz.h>
#include <WlzExtFF.h>
#include <HGU_XmUtils.h>
#include <WlzXm.h>

static void WlzXmFSBSelectImageTypeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	dialog;
  WlzEffFormat	format=(WlzEffFormat) client_data;
  char		strBuf[16];
  const char	*fileExt, *name;
  XmString	xmstr;
  

  /* get the dialog */
  dialog = w;
  while( dialog && (XmIsFileSelectionBox(dialog)==False) ){
    dialog = XtParent(dialog);
  }
  if( dialog == NULL ){
    return;
  }

  /* get the extension string and re-filter */
  if( name = WlzEffStringFromFormat(format,  &fileExt) ){
    sprintf(strBuf, "*.%s*", fileExt);
    xmstr = XmStringCreateSimple(strBuf);
    XtVaSetValues(dialog, XmNpattern, xmstr, NULL);
    XmStringFree( xmstr );
    XmFileSelectionDoSearch( dialog, NULL );
  }

  /* set the user data */
  XtVaSetValues(dialog, XmNuserData, client_data, NULL);

  return;
}

WlzObject *WlzXmReadExtFFObject(
  Widget				dialog,
  XmFileSelectionBoxCallbackStruct	*cbs,
  WlzEffFormat				*dstFmt,
  WlzErrorNum				*dstErr)
{
  WlzObject	*obj=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  FILE		*fp;
  WlzEffFormat	format;
  String	filename;

  /* check file pointer */
  if( (fp = HGU_XmGetFilePointer(dialog, cbs->value,
				 cbs->dir, "rb")) == NULL ){
    errNum = WLZ_ERR_FILE_OPEN;
  }

  /* get the image type */
  if( errNum == WLZ_ERR_NONE ){
    if( dialog ){
      XtVaGetValues(dialog, XmNuserData, &format, NULL);
    }
    else {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }

  /* now read the object */
  if( errNum == WLZ_ERR_NONE ){
    switch( format ){
    case WLZEFF_FORMAT_ICS:
    case WLZEFF_FORMAT_PNM:
    case WLZEFF_FORMAT_BMP:
    case WLZEFF_FORMAT_TIFF:
      if( filename = HGU_XmGetFileStr(dialog, cbs->value, cbs->dir) ){
	obj = WlzEffReadObj(NULL, filename, format, 0, &errNum);
	AlcFree( filename );
      }
      else {
	errNum = WLZ_ERR_PARAM_DATA;
      }
      break;

    case WLZEFF_FORMAT_AM:
    case WLZEFF_FORMAT_IPL:
    case WLZEFF_FORMAT_VTK:
    case WLZEFF_FORMAT_VFF:
    case WLZEFF_FORMAT_SLC:
    case WLZEFF_FORMAT_PIC:
    case WLZEFF_FORMAT_DEN:
    case WLZEFF_FORMAT_WLZ:
    case WLZEFF_FORMAT_JPEG:
      obj = WlzEffReadObj(fp, NULL, format, 0, &errNum);
      break;

    case WLZEFF_FORMAT_RAW:
      /* need more info for this */
      errNum = WLZ_ERR_FILE_FORMAT;
      break;

    default:
      obj = NULL;
      errNum = WLZ_ERR_FILE_FORMAT;
      break;
    }
    fclose(fp);
  }

  /* assign return type and error */
  if( dstFmt ){
    *dstFmt = format;
  }
  if( dstErr ){
    *dstErr = errNum;
  }
  return obj;
}


Widget WlzXmCreateExtFFObjectFSB(
  Widget	parent,
  String	name,
  XtCallbackProc	proc,
  XtPointer		client_data)
{
  Widget	dialog=NULL;
  Visual	visual;
  Arg		arg[1];
  MenuItem	*menuItems;
  Widget	form, menu;
  int		i, numFormats;
  XmString	xmstr;

  /* create the file-selection-box, add standard callbacks */
  dialog = XmCreateFileSelectionDialog(parent, name, arg, 0);
  XtAddCallback(dialog, XmNokCallback, proc, client_data);
  XtAddCallback(dialog, XmNokCallback, PopdownCallback, NULL);
  XtAddCallback(dialog, XmNcancelCallback, PopdownCallback, NULL);
  XtAddCallback(dialog, XmNmapCallback, FSBPopupCallback, NULL);

  /* add form for image-format selection */
  form = XtVaCreateManagedWidget("formatFormRC", xmRowColumnWidgetClass,
				 dialog,
				 XmNborderWidth,	0,
				 XmNnumColumns,		1,
				 NULL);

  /* build the choice menu from the ExtFF library */
  numFormats = WlzEffNumberOfFormats();
  menuItems = (MenuItem *) AlcCalloc(numFormats+1, sizeof(MenuItem));
  for(i=1; i <= numFormats; i++){
    menuItems[i-1].name = (char *) WlzEffStringFromFormat(i, NULL);
    menuItems[i-1].wclass = &xmPushButtonGadgetClass;
    menuItems[i-1].callback = WlzXmFSBSelectImageTypeCb;
    menuItems[i-1].callback_data = (XtPointer) i;
    menuItems[i-1].tear_off_model = XmTEAR_OFF_DISABLED;
    menuItems[i-1].radio_behavior = False;
    menuItems[i-1].always_one = False;
    menuItems[i-1].subitems = NULL;
  }
  menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "formatMenu", 0,
			 XmTEAR_OFF_DISABLED, menuItems);
  xmstr = XmStringCreateSimple("Image Type:");
  XtVaSetValues(menu,
		XmNlabelString,		xmstr,
		NULL);
  XtManageChild( menu );
  XmStringFree(xmstr);

  /* set woolz as the default */
  WlzXmExtFFObjectFSBSetType(dialog, WLZEFF_FORMAT_WLZ);

  return dialog;
}

WlzErrorNum WlzXmExtFFObjectFSBSetType(
  Widget	dialog,
  WlzEffFormat	format)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	menu, button;
  char		strBuf[32];
  const char	*buttonName, *fileExt;
  XmString	xmstr;

  /* set the pattern string and re-filter */
  buttonName = WlzEffStringFromFormat(format, &fileExt);
  sprintf(strBuf, "*.%s", fileExt);
  xmstr = XmStringCreateSimple(strBuf);
  XtVaSetValues(dialog, XmNpattern, xmstr, NULL);
  XmStringFree( xmstr );
  XmFileSelectionDoSearch( dialog, NULL );
  
  /* now set the menu entry */
  if( menu = XtNameToWidget(dialog, "*formatMenu") ){
    sprintf(strBuf, "*.%s", buttonName);
    if( button = XtNameToWidget(menu, strBuf) ){
      XtVaSetValues(menu, XmNmenuHistory, button, NULL);
    }
  }
  XtVaSetValues(dialog, XmNuserData, (XtPointer) format, NULL);
  
  return errNum;
}
