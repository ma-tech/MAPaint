#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   Mouse Atlas MAPaint					*
*   File       :   domainControls.c					*
*************************************************************************
*   Author Name :  richard						*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Nov 17 14:01:04 2003				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <MAPaint.h>

static unsigned char emage_ovly_cols[33][3] = 
{
/* greys */ 
  {0,0,0},
/* wash overlays */ 
  {128,96,0}, {255,255,0}, {0,0,255}, {0,255,0}, {0,255,255},
/* solid overlays */
  {255,0,0}
};
static int emageNumSolidOverlays=1;
static int emageNumWashOverlays=5;
static int emageCurrentDomain=6;
static double emageContrast[]={0.0,0.25,0.45,0.25,0.25,0.25};
static int emagePriority[]={1,6,2,3,4,5};
static char *emageName[]={
  "Not Examined",
  "Moderate Expression",
  "Weak Expression",
  "Possible Expression",
  "Not Detected",
  "Strong Expression"
};
static char *emageFilename[]={
  "notExamined.wlz",
  "moderate.wlz",
  "weak.wlz",
  "possible.wlz",
  "notDetected.wlz",
  "strong.wlz"
};


void setEMAGEDomainsAndColoursCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  PaintCmapStruct	*cmpstr;
  int			i, j;
  Widget		slider;

  /* check if any views selected for painting */
  if( paint_key ){
    HGU_XmUserMessage(globals.topl,
		      "Set EMAGE defaults:\n"
		      "    There is a view window selected\n"
		      "    for painting. Please quit painting\n"
		      "    before selecting the EMAGE domain\n"
		      "    defaults.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* provide warning */
  if( !HGU_XmUserConfirm(globals.topl,
			 "Set EMAGE defaults:\n"
			 "    Selecting this option will reset\n"
			 "    domain colours and file-names and\n"
			 "    you will be asked if you want to\n"
			 "    clear domains (recommended).\n\n"
			 "Do you want to continue?",
			 "yes", "cancel", 1) ){
    return;
  }

  /* clear all domains */
  clear_all_domains_cb(globals.topl, NULL, NULL);

  /* set the colormap and transparencies
     assume initialisation has been done once so only need
     to change the emage values */
  cmpstr = globals.cmapstruct;
  cmpstr->num_overlays = emageNumWashOverlays;
  cmpstr->num_solid_overlays = emageNumSolidOverlays;
  for(i=1; i <= 6; i++){
    cmpstr->ovly_red[i] = emage_ovly_cols[i][0];
    cmpstr->ovly_green[i] = emage_ovly_cols[i][1];
    cmpstr->ovly_blue[i] = emage_ovly_cols[i][2];
    cmpstr->ovly_contrast[i] = emageContrast[i];
  }
  set_paint_colormap(globals.canvas, globals.cmap, cmpstr);

  /* set the domain & file names, priorities, and current domain */
  globals.current_domain = emageCurrentDomain;
  for(i=1; i <=6; i++){
    globals.domain_name[i] = emageName[i-1];
    globals.domain_filename[i] = emageFilename[i-1];
    globals.priority_to_domain_lut[i] = emagePriority[i-1];
  }

  /* set the slider value */
  if( slider =
     XtNameToWidget(globals.topl,
		    "*domain_controls_dialog*numDomainsSlider") ){
    float fval = 6.0;
    HGU_XmSetSliderValue(slider, fval);
  }  

  /* set labels and colours */
  setMenuLabelsAndColors();

  /* set the emage flag and top-level title */
  globals.emageFlg = True;
  set_topl_title(NULL);

  return;
}

static unsigned char emap_ovly_cols[33][3] = 
{
/* greys */ 
  {0,0,0},
/* wash overlays */ 
  {255,0,0}, {0,0,255}, {0,255,0}, {255,255,0}, {0,255,255},
/* solid overlays */
  {255,0,0}, {255,129,0}, {255,184,0}, {255,255,0}, {158,255,0}
};
static int emapNumSolidOverlays=5;
static int emapNumWashOverlays=5;
static int emapCurrentDomain=1;
static double emapContrast[]={0.0,0.25,0.25,0.25,0.25,0.25};
static int emapPriority[]={1,2,3,4,5,6,7,8,9,10};
static char *emapName[]={
  "domain_1.wlz",
  "domain_2.wlz",
  "domain_3.wlz",
  "domain_4.wlz",
  "domain_5.wlz",
  "domain_6.wlz",
  "domain_7.wlz",
  "domain_8.wlz",
  "domain_9.wlz",
  "domain_10.wlz"
};
static char *emapFilename[]={
  "domain_1.wlz",
  "domain_2.wlz",
  "domain_3.wlz",
  "domain_4.wlz",
  "domain_5.wlz",
  "domain_6.wlz",
  "domain_7.wlz",
  "domain_8.wlz",
  "domain_9.wlz",
  "domain_10.wlz"
};

char *getEMAGE_Name(
  int	domain)
{
  if((domain > 0) &&
     (domain <= (emapNumSolidOverlays+emapNumWashOverlays)) ){
    return emageName[domain-1];
  }
  return NULL;
}

char *getEMAGE_Filename(
  int	domain)
{
  if((domain > 0) &&
     (domain <= (emapNumSolidOverlays+emapNumWashOverlays)) ){
    return emageFilename[domain-1];
  }
  return NULL;
}

void setEMAPDomainsAndColoursCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  PaintCmapStruct	*cmpstr;
  int			i, j;
  Widget		slider;

  /* check if any views selected for painting */
  if( paint_key ){
    HGU_XmUserMessage(globals.topl,
		      "Set EMAP defaults:\n"
		      "    There is a view window selected\n"
		      "    for painting. Please quit painting\n"
		      "    before selecting the EMAP domain\n"
		      "    defaults.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* provide warning */
  if( !HGU_XmUserConfirm(globals.topl,
			 "Set EMAP defaults:\n"
			 "    Selecting this option will reset\n"
			 "    domain colours and file-names and\n"
			 "    you will be asked if you want to\n"
			 "    clear domains (recommended).\n\n"
			 "Do you want to continue?",
			 "yes", "cancel", 1) ){
    return;
  }

  /* clear all domains */
  clear_all_domains_cb(globals.topl, NULL, NULL);

  /* set the colormap and transparencies
     assume initialisation has been done once so only need
     to change the emap values */
  cmpstr = globals.cmapstruct;
  cmpstr->num_overlays = emapNumWashOverlays;
  cmpstr->num_solid_overlays = emapNumSolidOverlays;
  for(i=1; i <= 10; i++){
    cmpstr->ovly_red[i] = emap_ovly_cols[i][0];
    cmpstr->ovly_green[i] = emap_ovly_cols[i][1];
    cmpstr->ovly_blue[i] = emap_ovly_cols[i][2];
    cmpstr->ovly_contrast[i] = emapContrast[i];
  }
  set_paint_colormap(globals.canvas, globals.cmap, cmpstr);

  /* set the domain & file names, priorities, and current domain */
  globals.current_domain = emapCurrentDomain;
  for(i=1; i <= 10; i++){
    globals.domain_name[i] = emapName[i-1];
    globals.domain_filename[i] = emapFilename[i-1];
    globals.priority_to_domain_lut[i] = emapPriority[i-1];
  }

  /* set the slider value */
  if( slider =
     XtNameToWidget(globals.topl,
		    "*domain_controls_dialog*numDomainsSlider") ){
    float fval = 10.0;
    HGU_XmSetSliderValue(slider, fval);
  }  

  /* set labels and colours */
  setMenuLabelsAndColors();

  /* set the emap flag and top-level title */
  globals.emageFlg = False;
  set_topl_title(NULL);

  return;
}

void setMenuLabelsAndColors(void)
{
  int		i, num_overlays;
  XmString	xmstr;
  Widget	widget;
  char		str_buf[64];
  Pixel		pixel;
  int		data_index, col_index;

  num_overlays = globals.cmapstruct->num_overlays + 
    globals.cmapstruct->num_solid_overlays;
  for(i=1; i < 33; i++)
  {

    /* get new  label and background colour */
    data_index = globals.priority_to_domain_lut[i];
    xmstr = XmStringCreateSimple( globals.domain_name[data_index] );
    col_index = globals.cmapstruct->ovly_cols[data_index] +
      globals.cmapstruct->gmax - globals.cmapstruct->gmin;
    if( data_index > globals.cmapstruct->num_overlays )
    {
      col_index = globals.cmapstruct->ovly_cols[data_index];
    }
    pixel = HGU_XGetColorPixel(XtDisplay(globals.topl), globals.cmap,
			       (float) globals.colormap[0][col_index]/255.0,
			       (float) globals.colormap[1][col_index]/255.0,
			       (float) globals.colormap[2][col_index]/255.0);

    /* get the widgets by priority and reset resources */
    /* domain menu */
    sprintf(str_buf, "*menubar*domain_menu*select*domain_%d", i);
    if( widget = XtNameToWidget(globals.topl, str_buf) ){
      if( i <= num_overlays ){
	XtVaSetValues(widget,
		      XmNlabelString, xmstr,
		      XmNbackground, pixel,
		      XmNset, (globals.priority_to_domain_lut[i] ==
			       globals.current_domain)?True:False,
		      NULL);
	XtManageChild(widget);
      }
      else {
	XtUnmanageChild(widget);
      }
    }

    /* dominance array */
    sprintf(str_buf, "*domain_controls_dialog*dominance_form*%d", i);
    if( widget = XtNameToWidget(globals.topl, str_buf) ){
      if( i <= num_overlays ){
	XtVaSetValues(widget, XmNbackground, pixel, NULL);
	XtManageChild(widget);
      }
      else {
	XtUnmanageChild(widget);
      }
    }

    /* colormap option menu */
    sprintf(str_buf, "*colormap_dialog*overlay*%d", i);
    if( widget = XtNameToWidget(globals.topl, str_buf) ){
      if( i <= num_overlays ){
	XtVaSetValues(widget, XmNbackground, pixel, NULL);
	XtManageChild(widget);
      }
      else {
	XtUnmanageChild(widget);
      }
    }

    /* domain surgery */
    sprintf(str_buf, "*surgery_dialog*dest_domain_rc*domain %d", i);
    if( widget = XtNameToWidget(globals.topl, str_buf) ){
      if( i <= num_overlays ){
	XtVaSetValues(widget,
		      XmNlabelString, xmstr,
		      XmNbackground, pixel,
		      NULL);
	XtManageChild(widget);
      }
      else {
	XtUnmanageChild(widget);
      }
    }
    sprintf(str_buf, "*surgery_dialog*src_domain*domain %d", i);
    if( widget = XtNameToWidget(globals.topl, str_buf) ){
      if( i <= num_overlays ){
	XtVaSetValues(widget,
		      XmNlabelString, xmstr,
		      XmNbackground, pixel,
		      NULL);
	XtManageChild(widget);
      }
      else {
	XtUnmanageChild(widget);
      }
    }

    /* domain review */
    sprintf(str_buf, "*review_dialog*dest_domain_rc*domain %d", i);
    if( widget = XtNameToWidget(globals.topl, str_buf) ){
      if( i <= num_overlays ){
	XtVaSetValues(widget,
		      XmNlabelString, xmstr,
		      XmNbackground, pixel,
		      NULL);
	XtManageChild(widget);
      }
      else {
	XtUnmanageChild(widget);
      }
    }
    sprintf(str_buf, "*review_dialog*src_domain*domain %d", i);
    if( widget = XtNameToWidget(globals.topl, str_buf) ){
      if( i <= num_overlays ){
	XtVaSetValues(widget,
		      XmNlabelString, xmstr,
		      XmNbackground, pixel,
		      NULL);
	XtManageChild(widget);
      }
      else {
	XtUnmanageChild(widget);
      }
    }

    XmStringFree(xmstr);
  }

  return;
}

static char dragTranslations[] = 
"#override <Btn2Down>: StartDragDrop(DomainDominance)";

#define DOMAIN_NAME_ATOM	"STRING"

static Boolean DD_DragConvertProc(
  Widget	w,
  Atom		*selection,
  Atom		*target,
  Atom		*type_return,
  XtPointer	*value_return,
  unsigned long	*length_return,
  int		*format_return)
{
  Atom		export_target, motif_drop;
  Widget	source_w;
  String	name;

  /* get export atom for final type check */
  export_target = XmInternAtom(XtDisplay(w), DOMAIN_NAME_ATOM, False);
  motif_drop = XmInternAtom(XtDisplay(w), "_MOTIF_DROP", False);

  /* check selection */
  if( *selection != motif_drop )
  {
    return( False );
  }

  /* get the source widget and copy its name */
  XtVaGetValues(w, XmNclientData, (XtPointer) &source_w, NULL);
  if( source_w == NULL )
  {
    return( False );
  }
  name = (String) malloc(sizeof(char) * (strlen(XtName(source_w)) + 2));
  if( name == NULL )
  {
    return( False );
  }
  (void) strcpy( name, XtName(source_w) );

  /* set up the return values */
  *type_return = export_target;
  *value_return = (XtPointer) name;
  *length_return = strlen(name) + 1;
  *format_return = 8;

  return True;
}

void RotateArray(
  char		*val,
  size_t	size,
  int		from_index,
  int		to_index)
{
  char		*tmp;
  int		i;

  tmp = (char *) AlcMalloc( size );
  memcpy( (void *) tmp, (const void *) (val + from_index*size), size );

  if( from_index < to_index )
  {
    for(i=from_index; i < to_index; i++)
    {
      memcpy((void *) (val + i*size), (const void *) (val + (i+1)*size), size);
    }
  }
  else if( from_index > to_index )
  {
    for(i=from_index; i > to_index; i--)
    {
      memcpy((void *) (val + i*size), (const void *) (val + (i-1)*size), size);
    }
  }

  memcpy( (void *) (val + to_index*size), (const void *) tmp, size );
  AlcFree( (void *) tmp );

  return;
}

static int DD_GetIndex(
  String	str)
{
  int	rtnIndex=-1, i, j, numDomains;

  /* first check against domain names */
  numDomains = globals.cmapstruct->num_overlays + 
    globals.cmapstruct->num_solid_overlays;
  for(i=1; i <= numDomains; i++){
    if( strncmp(str, globals.domain_name[i],
		strlen(globals.domain_name[i])) == 0 ){
      for(j=1; j <= numDomains; j++){
	if( i == globals.priority_to_domain_lut[j] ){
	  rtnIndex = j;
	  break;
	}
      }
      break;
    }
  }

  /* check if a simple number */
  if( rtnIndex == -1 ){
    if( sscanf(str, "%d", &rtnIndex) < 1 )
    {
      rtnIndex = -1;;
    }
  }

  /* check value */
  if( (rtnIndex < 1) || (rtnIndex > numDomains) ){
    rtnIndex = -1;
  }

  return rtnIndex;
}

static void DD_TransferProc(
  Widget	w,
  XtPointer	client_data,
  Atom		*seltype,
  Atom		*type,
  XtPointer	value,
  unsigned long	*length,
  int		format)
{
  int		source_index, dest_index;
  int		maxIndex;

  /* extract the domain indices */
  if( (source_index = DD_GetIndex((String) value)) < 1 ){
    return;
  }
  if( (dest_index = DD_GetIndex(XtName((Widget) client_data))) < 1 ){
    return;
  }

  /* rotate the priority_to_domain_lut  */
  RotateArray((char *) &(globals.priority_to_domain_lut[0]),
	      sizeof(int), source_index, dest_index);

  /* reset the menu labels and background colours */
  setMenuLabelsAndColors();

  return;
}

static void DD_FinishCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	icon = NULL;

  XtVaGetValues(w, XmNsourceCursorIcon, &icon, NULL);

  if( icon )
  {
    XtDestroyWidget( icon );
  }

  return;
}

#define domainbm_width 55
#define domainbm_height 17
static unsigned char domainbm_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x1f, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x10, 0xc2, 0x01, 0x03, 0x00, 0x00, 0x03, 0x10, 0xe2,
   0x03, 0x03, 0x00, 0x00, 0x00, 0x10, 0x72, 0xc7, 0x73, 0xfe, 0x38, 0x7b,
   0x10, 0x3a, 0xee, 0xfb, 0xfe, 0x7d, 0xfb, 0x10, 0x1a, 0x6c, 0xdb, 0xb6,
   0x65, 0xdb, 0x10, 0x3a, 0x6e, 0xdb, 0xb6, 0x79, 0xdb, 0x10, 0x72, 0x67,
   0xdb, 0xb6, 0x6d, 0xdb, 0x10, 0xe2, 0xe3, 0xfb, 0xb6, 0x7d, 0xdb, 0x10,
   0xc2, 0xc1, 0x73, 0xb6, 0xdd, 0xdb, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xfe, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void DD_StartDomainDominanceDrag(
Widget		w,
XEvent		*event,
String		*params,
Cardinal	*num_params)
{
  Atom		export_list[1];
  Pixmap	icon;
  int		n;
  Arg		args[12];
  Widget	drag_icon, drag_context;
  Pixel		fg, bg;
  int		depth;

  /* if painting then ignore this request - should be fixed elsewhere */
  if( paint_key != NULL )
  {
    return;
  }

  /* get the atom for data target type */
  export_list[0] = XmInternAtom(XtDisplay(w), DOMAIN_NAME_ATOM, False);

  /* get foreground and background */
  XtVaGetValues(w, XmNbackground, &bg, XmNforeground, &fg, NULL);

  /* specify resources for the drag context */
  n = 0;
  XtSetArg(args[n], XmNblendModel, XmBLEND_ALL); n++;
  XtSetArg(args[n], XmNcursorBackground, bg); n++;
  XtSetArg(args[n], XmNcursorForeground, fg); n++;
  XtSetArg(args[n], XmNexportTargets, export_list); n++;
  XtSetArg(args[n], XmNnumExportTargets, 1); n++;
  XtSetArg(args[n], XmNdragOperations, XmDROP_MOVE); n++;
  XtSetArg(args[n], XmNconvertProc, DD_DragConvertProc); n++;
  XtSetArg(args[n], XmNclientData, w); n++;

  /* start the drag and register a finish callback */
  drag_context = XmDragStart(w, event, args, n);

  return;
}

void DD_DomainDominanceHandleCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmDropProcCallbackStruct	*cbs = (XmDropProcCallbackStruct *) call_data;
  int				i, n;
  Arg				args[8];
  Atom				import_target, *export_targets;
  Cardinal			num_export_targets;
  Widget			drag_context;
  Boolean			domain_name = False;
  XmDropTransferEntryRec	transfer_entries[1];

  /* get the required import target */
  import_target = XmInternAtom(XtDisplay(w), DOMAIN_NAME_ATOM, False);

  drag_context = cbs->dragContext;

  /* get the export targets from the drag context and check type */
  n = 0;
  XtSetArg(args[n], XmNexportTargets, &export_targets); n++;
  XtSetArg(args[n], XmNnumExportTargets, &num_export_targets); n++;
  XtGetValues(drag_context, args, n);
  for(i=0; i < num_export_targets; i++)
  {
    if( export_targets[i] == import_target )
    {
      domain_name = True;
      break;
    }
  }

  /* if not HGU_PAINT_DOMAIN_NAME or not XmDROP_MOVE then fail */
  n = 0;
  if( (domain_name != True) || (cbs->dropAction != XmDROP) ||
      (cbs->operation != XmDROP_COPY) )
  {
    XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
    XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
  }
  else
  {
    transfer_entries[0].target = import_target;
    transfer_entries[0].client_data = (XtPointer) w;
    XtSetArg(args[n], XmNdropTransfers, transfer_entries); n++;
    XtSetArg(args[n], XmNnumDropTransfers, 1); n++;
    XtSetArg(args[n], XmNtransferProc, DD_TransferProc); n++;
  }

  XmDropTransferStart(cbs->dragContext, args, n);

  return;
}

void DD_DomainDominanceDropSite(
  Widget	site)
{
  Atom			import_list[1];
  Arg			args[10];
  int			n;

  /* setup arguments for dominance drag and drop */
  n = 0;
  import_list[0] = XmInternAtom(XtDisplay(globals.topl),
				DOMAIN_NAME_ATOM, False);
  XtSetArg(args[n], XmNimportTargets, import_list); n++;
  XtSetArg(args[n], XmNnumImportTargets, 1); n++;
  XtSetArg(args[n], XmNdropSiteOperations, XmDROP_COPY); n++;
  XtSetArg(args[n], XmNdropProc, DD_DomainDominanceHandleCb); n++;

  /* register as a drop site */
  XmDropSiteRegister(site, args, n);

  return;
}

static void resetDominanceCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int	i;

  for(i=0; i < 33; i++)
  {
    globals.priority_to_domain_lut[i] = i;
  }
  setMenuLabelsAndColors();

  return;
}

static void resetAllDomainParamsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		i;
  Widget	slider;

  /* check if any views selected for painting */
  if( paint_key ){
    HGU_XmUserMessage(globals.topl,
		      "Reset domain defaults:\n"
		      "    There is a view window selected\n"
		      "    for painting. Please quit painting\n"
		      "    before selecting the EMAGE domain\n"
		      "    defaults.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* provide warning */
  if( !HGU_XmUserConfirm(globals.topl,
			 "Reset domain defaults:\n"
			 "    Selecting this option will reset\n"
			 "    domain colours and file-names and\n"
			 "    you will be asked if you want to\n"
			 "    clear domains (recommended).\n\n"
			 "Do you want to continue?",
			 "yes", "cancel", 1) ){
    return;
  }

  /* clear all domains */
  clear_all_domains_cb(globals.topl, NULL, NULL);

  /* dominance */
  for(i=0; i < 33; i++)
  {
    globals.priority_to_domain_lut[i] = i;
  }

  /* domain names and file-names, etc. */
  MAPaintDomainGetResourceValues();
  MAPaintColormapGetResourceValues();
  if( slider =
     XtNameToWidget(globals.topl,
		    "*domain_controls_dialog*numDomainsSlider") ){
    float fval = globals.cmapstruct->num_overlays +
      globals.cmapstruct->num_solid_overlays;
    HGU_XmSetSliderValue(slider, fval);
  }

  if( slider =
     XtNameToWidget(globals.topl,
		    "*domain_controls_dialog*OpenGLDisplayDomainStep") ){
    float fval = globals.OpenGLDisplayDomainStep;
    HGU_XmSetSliderValue(slider, fval);
  }

  /* OpenGL step */
  globals.OpenGLDisplayDomainStep = 2;

  setMenuLabelsAndColors();

  /* set the emage flag and top-level title */
  globals.emageFlg = False;
  set_topl_title(NULL);

  return;
}

static void numDomainsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider=w, shell;
  int		val;

  while( strcmp(XtName(slider), "numDomainsSlider") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }
  val = (int) HGU_XmGetSliderValue( slider );
  if( val < 5 ){
    val = 5;
  }
  if( val > 32 ){
    val = 32;
  }
  globals.cmapstruct->num_solid_overlays = val - 5;
  setMenuLabelsAndColors();

  /* set the emage flag and top-level title */
  globals.emageFlg = False;
  set_topl_title(NULL);

  return;
}

static void OGLDispDomainStepCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider=w;
  int		val;

  while( strcmp(XtName(slider), "OpenGLDisplayDomainStep") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }
  val = (int) HGU_XmGetSliderValue( slider );
  if( val < 1 ){
    val = 1;
  }
  if( val > 10 ){
    val = 10;
  }
  globals.OpenGLDisplayDomainStep = val;

  return;
}


static ActionAreaItem   domain_control_actions[] = {
{"emap_domains",	setEMAPDomainsAndColoursCb,		NULL},
{"emage_domains",	setEMAGEDomainsAndColoursCb,		NULL},
{"reset_all",	resetAllDomainParamsCb,		NULL},
{"Dismiss",     NULL,           NULL},
{"Help",        NULL,           NULL},
};

Widget create_domain_controls_dialog(
  Widget	topl)
{
  Widget		dialog, control, dominance, row_column,  form;
  Widget		frame, widget, slider;
  int			i, n, num_overlays;
  Pixel			pixel;
  char			str_buf[16];
  float			fval;


  /* create the controls dialog */
  dialog = HGU_XmCreateStdDialog(topl, "domain_controls_dialog",
				 xmFormWidgetClass,
				 domain_control_actions, 5);

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }
  control = XtNameToWidget( dialog, "*.control" );

  num_overlays = globals.cmapstruct->num_overlays +
    globals.cmapstruct->num_solid_overlays;

  /* add in frame for the dominance control */
  frame = XtVaCreateManagedWidget("dominance_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment,  	XmATTACH_FORM,
				  XmNleftAttachment, 	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  NULL);

  form = XtVaCreateManagedWidget("dominance_form", xmFormWidgetClass,
				 frame,
				 XmNborderWidth,	0,
				 XmNchildType, XmFRAME_WORKAREA_CHILD,
				 NULL);

  dominance = XtVaCreateManagedWidget("dominance", xmLabelWidgetClass,
				      frame,
				      XmNborderWidth,	0,
				      XmNchildType, XmFRAME_TITLE_CHILD,
				      NULL);

  row_column = XtVaCreateManagedWidget("dominance_rc", xmRowColumnWidgetClass,
				       form,
				       XmNborderWidth,		0,
				       XmNorientation,		XmVERTICAL,
				       XmNpacking,		XmPACK_COLUMN,
				       XmNnumColumns,		12,
				       XmNresizeWidth,		True,
				       XmNresizeHeight,		True,
				       XmNtopAttachment,	XmATTACH_FORM,
				       XmNleftAttachment,	XmATTACH_FORM,
				       NULL);

  /* create the dominance buttons */
  for(i=1; i <= 32; i++)
  {
    sprintf(str_buf, "%d", i);
    widget = XtVaCreateManagedWidget (str_buf, xmPushButtonWidgetClass,
				      row_column,
				      XmNshadowThickness,	2,
				      NULL);
    if( i > num_overlays ){
      XtUnmanageChild(widget);
    }
    DD_DomainDominanceDropSite(widget);
  }

  /* add the dominance reset button */
  widget = XtVaCreateManagedWidget("resetDominance", xmPushButtonWidgetClass,
				   form,
				   XmNtopAttachment,	XmATTACH_WIDGET,
				   XmNtopWidget,	row_column,
				   XmNleftAttachment,	XmATTACH_FORM,
				   NULL);
  XtAddCallback(widget, XmNactivateCallback, resetDominanceCb,
		NULL);

  /* now a frame for adjusting number of domains etc */
  frame = XtVaCreateManagedWidget("domain_parameters_frame",
				  xmFrameWidgetClass, control,
				  XmNtopAttachment,  	XmATTACH_WIDGET,
				  XmNtopWidget,		frame,
				  XmNleftAttachment, 	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  NULL);

  form = XtVaCreateManagedWidget("domain_params_form", xmFormWidgetClass,
				 frame,
				 XmNborderWidth,	0,
				 XmNchildType, XmFRAME_WORKAREA_CHILD,
				 NULL);

  dominance = XtVaCreateManagedWidget("domain_parameters", xmLabelWidgetClass,
				      frame,
				      XmNborderWidth,	0,
				      XmNchildType, XmFRAME_TITLE_CHILD,
				      NULL);

  /* slider for the domain numbers */
  fval = num_overlays;
  slider = HGU_XmCreateHorizontalSlider("numDomainsSlider", form,
					fval, 5.0, 32.0, 0,
					numDomainsCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  /* slider for the 3D display sampling */
  fval = globals.OpenGLDisplayDomainStep;
  slider = HGU_XmCreateHorizontalSlider("OpenGLDisplayDomainStep", form,
					fval, 1.0, 10.0, 0,
					OGLDispDomainStepCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  widget = slider;

  return( dialog );
}
