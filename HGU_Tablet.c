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
*   File       :   HGU_Tablet.c						*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Wed Apr  8 16:25:58 1998				*
*   Synopsis    : Utilities for accessing the wacom tablet		*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stropts.h>
#include <termio.h>
#include <signal.h>
#include <poll.h>

#include <Alc.h>
#include <HGU_Tablet.h>

static void sigFunc(int sig)
{
  return;
}

static int MySleep(int uSeconds)
{
  struct itimerval value;

  /* set the structure time in microseconds */
  value.it_interval.tv_sec = 0;
  value.it_interval.tv_usec = 0;
  if( uSeconds > 999999 ){
    value.it_value.tv_sec = uSeconds/1000000;
    value.it_value.tv_usec = uSeconds%1000000;
  }
  else
  {
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = uSeconds;
  }    

  /* set the timer and wait for signal */
  if( setitimer(ITIMER_REAL, &value, NULL) == 0 ){
    signal(SIGALRM, sigFunc);
    pause();
    return 0;
  }

  return 1;
}

static int SetTimeoutSignal(int uSeconds)
{
  struct itimerval value;

  /* set the structure time in microseconds */
  value.it_interval.tv_sec = 0;
  value.it_interval.tv_usec = 0;
  if( uSeconds > 999999 ){
    value.it_value.tv_sec = uSeconds/1000000;
    value.it_value.tv_usec = uSeconds%1000000;
  }
  else
  {
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = uSeconds;
  }    

  /* set the timer and wait for signal */
  if( setitimer(ITIMER_REAL, &value, NULL) == 0 ){
    if( uSeconds ){
      signal(SIGALRM, sigFunc);
    }
    return 0;
  }

  return 1;
}

static int SetLineCharacteristics(int fd)
{
  struct termio term_pars;
  speed_t	speed=B9600;

  /*  mode setting */
  ioctl (fd, TCGETA, &term_pars);

  /* terminal parameters - clear any input controls */
  term_pars.c_iflag = 0x0;

  /* terminal parameters - clear any output controls */
  term_pars.c_oflag = 0x0;

  /* local modes - clear any local controls */
  term_pars.c_lflag = 0x0;

  /* line control modes */
  /* set one  stop bit */
  term_pars.c_cflag &= ~CSTOPB;

  /* Set 8 bit characters, enable reading, direct local line */
  term_pars.c_cflag |= (CS8 | CREAD);
  term_pars.c_cflag |= CLOCAL;

  /* set no parity */
  term_pars.c_cflag &= ~(PARENB);

  /* set inbound and outbound hardware flow control */
#ifdef SUNOS5
  term_pars.c_cflag |= (CRTSXOFF | CRTSCTS);
#else /* SUNOS5 */
  term_pars.c_cflag |= CNEW_RTSCTS;
#endif /* SUNOS5 */


  /* set the speed to 9600 */
#ifdef SUNOS5
  term_pars.c_cflag &= ~(CBAUD|CBAUDEXT);
  
  if( speed > CBAUD ){
    term_pars.c_cflag |= CBAUDEXT;
    speed -= (CBAUD + 1);
  }
#else /* SUNOS5 */
  term_pars.c_cflag &= ~(CBAUD);
#endif /* SUNOS5 */
  term_pars.c_cflag |= (speed&CBAUD);

  ioctl (fd, TCSETA, &term_pars);

  return 0;
}

static int eventCounter=0;

static TabletErrNum decodeWacomByteBuffer(
  HGUTablet	*tablet,
  unsigned char	*buffer,
  int		numBytes,
  TabletEvent	*event)
{
  TabletErrNum	errNum=TABLET_ERR_NONE;
  int	i;
  int	i_high, i_mid, i_low;
  int	iBuf[10];

  /* increment event counter */
  eventCounter++;
  event->eventNum = eventCounter;

  /* copy to unsigned int to get unambiguous bit operations */
  for(i=0; i < numBytes; i++){
    iBuf[i] = buffer[i];
  }

  /* check proximity, pointer device and button press */
  if( iBuf[0]&WACOM_IV_PROXIMITY ){
    event->proximity = 1;
  }
  else {
    event->proximity = 0;
  }

  if( iBuf[0]&WACOM_IV_STYLUS ){
    event->device = TABLET_DEVICE_STYLUS;
  }
  else {
    event->device = TABLET_DEVICE_4BUTTON_CURSOR;
  }

  if( iBuf[0]&WACOM_IV_BUTTON_PRESS ){
    event->buttonPressed = 1;
  }
  else {
    event->buttonPressed = 0;
  }

  /* get coordinates */
  i_high =  iBuf[0]&WACOM_IV_X_HIGH_BITS;
  i_mid =  iBuf[1]&WACOM_IV_X_MID_BITS;
  i_low =  iBuf[2]&WACOM_IV_X_LOW_BITS;
  event->x = (i_high<<14) + (i_mid<<7) + i_low;

  i_high =  iBuf[3]&WACOM_IV_X_HIGH_BITS;
  i_mid =  iBuf[4]&WACOM_IV_X_MID_BITS;
  i_low =  iBuf[5]&WACOM_IV_X_LOW_BITS;
  event->y = (i_high<<14) + (i_mid<<7) + i_low;

  /* get the button number */
  if( event->buttonPressed ){
    event->buttons = iBuf[3]&WACOM_IV_BUTTON_BITS;
    event->buttons = event->buttons>>3;
  }
  else {
    event->buttons = 0;
  }

  /* get the pressure */
  event->pressure = iBuf[6]&WACOM_IV_PRESSURE_BITS;
  if( iBuf[6]&WACOM_IV_PRESSURE_SIGN ){
    event->pressure = -(1 + (~iBuf[6])&WACOM_IV_PRESSURE_BITS);
  }

  /* get the tilts */
  if( numBytes > 7 ){
    event->xTilt = iBuf[7]&WACOM_IV_X_TILT_BITS;
    if( iBuf[7]&WACOM_IV_X_TILT_SIGN ){
      event->xTilt = -(1 + (~iBuf[7])&WACOM_IV_X_TILT_BITS);
    }

    event->yTilt = iBuf[8]&WACOM_IV_Y_TILT_BITS;
    if( iBuf[8]&WACOM_IV_Y_TILT_SIGN ){
      event->yTilt = -(1 + (~iBuf[8])&WACOM_IV_Y_TILT_BITS);
    }
  }
  else {
    event->xTilt = 0;
    event->yTilt = 0;
  }

  /* check if tilt correction required */
  if( tablet->cntrlMode2&TABLET_CORRECT_TILT_MASK ){
    event->x += tablet->xTiltLut[event->xTilt+64];
    event->y += tablet->yTiltLut[event->yTilt+64];
  }

  /* check if transform correction required */
  if( tablet->cntrlMode2&TABLET_TRANSFORM_MASK ){
    event->X = (event->x * tablet->xTrans[0] +
		event->y * tablet->xTrans[1] + tablet->xTrans[2]);
    event->Y = (event->x * tablet->yTrans[0] +
		event->y * tablet->yTrans[1] + tablet->yTrans[2]);
  }
  else {
    event->X = event->x;
    event->Y = event->y;
  }

  return errNum;
}
 

HGUTablet *TabletOpen(
  TabletType	type,
  char		*ttyStr,
  TabletErrNum	*dstErr)
{
  int 		fd;
  TabletErrNum	errNum=TABLET_ERR_NONE;
  int		quitFlag, i;
  char		*versionStr;
  HGUTablet	*tablet=NULL;
  char		buffer[64];

  /* open the serial line note the timeout is set to avoid blocking
     on open and the second SetTimeoutSignal clears the old if unexpired */
  SetTimeoutSignal(500000);
  if( (fd = open(ttyStr, O_RDWR)) < 0 ){
    errNum = TABLET_ERR_TTY_STR;
  }
  else {
    SetTimeoutSignal(0);
    if( SetLineCharacteristics(fd) ){
      errNum = TABLET_ERR_LINE_CHARACTERISTICS;
    }
  }

  /* now check out the tablet */
  if( errNum == TABLET_ERR_NONE ){
    switch( type ){
    case WACOM_IV_TABLET_TYPE:
      /* write initialisation command - sleep 20 milliseconds
	 to allow reset */
      write(fd, (const void *) "#\r", 2);
      MySleep(20000);

      /* attempt to read tablet model - also protected by timeout */
      SetTimeoutSignal(500000);
      quitFlag = 0;
      write(fd, (const void *) "~#\r", 3);
      while( !quitFlag ){
	if( read(fd, (void *) &buffer[0], 1) > 0 ){
	  if( buffer[0] == '~' ){
	    if( read(fd, (void *) &buffer[1], 1) < 1 ){
	      errNum = TABLET_ERR_NO_RESPONSE;
	      break;
	    }
	    if( buffer[1] == '#' ){
	      int i;
	      for(i=2; i < 128; i++){
		if( read(fd, (void *) &buffer[i], 1) < 1){
		  errNum = TABLET_ERR_NO_RESPONSE;
		  break;
		}
		if( (buffer[i] == '\r') || (buffer[i] == '\n') ){
		  buffer[i] = '\0';
		  versionStr = (char *)
		    AlcMalloc(strlen((char *) &buffer[2]) + 2);
		  strcpy(versionStr, (char *) &buffer[2]);
		  break;
		}
	      }
	      quitFlag = 1;
	    }
	  }
	  /* set the point rate to about 40 Hz */
	  write(fd, (const void *) "IT5\r", 4);
	}
	else {
	  errNum = TABLET_ERR_NO_RESPONSE;
	  break;
	}
      }
      SetTimeoutSignal(0);
      break;

    default:
      errNum = TABLET_ERR_TYPE;
      break;
    }
  }

  /* create a tablet structure and set values */
  if( errNum == TABLET_ERR_NONE ){
    tablet = (HGUTablet *) AlcMalloc(sizeof(HGUTablet));
    tablet->type = type;
    tablet->ttyStr = (char *) AlcMalloc(strlen(ttyStr)+2);
    strcpy(tablet->ttyStr, ttyStr);
    tablet->fd = fd;
    tablet->versionStr = versionStr;
    tablet->cntrlMode1 = 0;
    tablet->cntrlMode2 = 0;
    /* this must be tablet type dependent */
    for(i=0; i < 128; i++){
      tablet->xTiltLut[i] = 0;
      tablet->yTiltLut[i] = 0;
    }
    tablet->xTrans[0] = 1.0;
    tablet->xTrans[1] = 0.0;
    tablet->xTrans[2] = 0.0;
    tablet->yTrans[0] = 0.0;
    tablet->yTrans[1] = 1.0;
    tablet->yTrans[2] = 0.0;
  }

  /* stop the tablet transmitting */
  if( errNum == TABLET_ERR_NONE ){
    errNum = TabletStop(tablet);
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return tablet;
}

TabletErrNum TabletClose(
  HGUTablet	*tablet)
{
  TabletErrNum	errNum=TABLET_ERR_NONE;

  /* close the serial line */
  SetTimeoutSignal(500000);
  if( close(tablet->fd) < 0 ){
    errNum = TABLET_ERR_NO_RESPONSE;
  }
  SetTimeoutSignal(0);

  /* free space */
  if( tablet->ttyStr ){
    AlcFree((void *) tablet->ttyStr);
  }
  if( tablet->versionStr ){
    AlcFree((void *) tablet->versionStr);
  }

  AlcFree((void *) tablet);
  return errNum;
}

TabletErrNum TabletStop(
  HGUTablet	*tablet)
{
  TabletErrNum	errNum=TABLET_ERR_NONE;

  /* stop the tablet */
  switch( tablet->type ){
  case WACOM_IV_TABLET_TYPE:
    SetTimeoutSignal(500000);
    if( write(tablet->fd, (const void *) "SP\rSP\rSP\rSP\r", 12) < 0 ){
      errNum = TABLET_ERR_NO_RESPONSE;
    }
    else {
      tablet->transmitting = 0;
    }
    SetTimeoutSignal(0);
    break;

  default:
    errNum = TABLET_ERR_TYPE;
    break;
  }

  return errNum;
}

TabletErrNum TabletSetMode(
  HGUTablet	*tablet,
  unsigned int	modeFlg)
{
  TabletErrNum	errNum=TABLET_ERR_NONE;

  /* start the tablet */
  switch( tablet->type ){
  case WACOM_IV_TABLET_TYPE:
    SetTimeoutSignal(500000);

    /* set multimode */
    if( modeFlg&TABLET_MULTIMODE_MASK ){
      if( write(tablet->fd, (const void *) "MU1\rMU1\rMU1\r", 12) < 0 ){
	errNum = TABLET_ERR_NO_RESPONSE;
      }
      else {
	tablet->cntrlMode1 |= TABLET_MULTIMODE_MASK;
      }
    }
      
    /* set tiltmode */
    if( modeFlg&TABLET_TILTMODE_MASK ){
      if( write(tablet->fd, (const void *) "FM1\rFM1\rFM1\r", 12) < 0 ){
	errNum = TABLET_ERR_NO_RESPONSE;
      }
      else {
	tablet->cntrlMode1 |= TABLET_TILTMODE_MASK;
      }
    }
      
    SetTimeoutSignal(0);
    break;

  default:
    errNum = TABLET_ERR_TYPE;
    break;
  }

  return errNum;
}

TabletErrNum TabletStart(
  HGUTablet	*tablet)
{
  TabletErrNum	errNum=TABLET_ERR_NONE;

  /* start the tablet */
  switch( tablet->type ){
  case WACOM_IV_TABLET_TYPE:
    SetTimeoutSignal(500000);
    if( write(tablet->fd, (const void *) "ST\rST\rST\rST\r", 12) < 0 ){
      errNum = TABLET_ERR_NO_RESPONSE;
    }
    else {
      tablet->transmitting = 1;
    }
    SetTimeoutSignal(0);
    break;

  default:
    errNum = TABLET_ERR_TYPE;
    break;
  }

  return errNum;
}

TabletErrNum TabletNextEvent(
  HGUTablet	*tablet,
  TabletEvent	*event)
{
  TabletErrNum	errNum=TABLET_ERR_NONE;
  int		count;
  unsigned char	buffer[10];

  /* check if transmitting */
  if( !tablet->transmitting ){
    return TABLET_ERR_UNSPECIFIED;
  }

  /* read for required number of bytes after the start byte */
  buffer[0] = '\0';
  switch( tablet->type ){
  case WACOM_IV_TABLET_TYPE:
    while( !(((unsigned int) buffer[0])&WACOM_IV_FIRST_BYTE) ){
      if( read(tablet->fd, (void *) &buffer[0], 1) < 1 ){
	errNum = TABLET_ERR_NO_RESPONSE;
	break;
      }
    }

    if( errNum == TABLET_ERR_NONE ){
      count = (tablet->cntrlMode1&TABLET_TILTMODE_MASK) ? 9 : 7;
      if( read(tablet->fd, (void *) &buffer[1], (count-1)) < (count-1) ){
	errNum = TABLET_ERR_NO_RESPONSE;
      }
      else {
	errNum = decodeWacomByteBuffer(tablet, &buffer[0], count, event);
      }
    }
    break;

  default:
    errNum = TABLET_ERR_TYPE;
    break;
  }    

  return errNum;
}

TabletErrNum TabletClearEvents(
  HGUTablet	*tablet)
{
  TabletErrNum	errNum=TABLET_ERR_NONE;
  struct pollfd	fds;
  char		c;

  /* stop the tablet */
  if( tablet->transmitting ){
    errNum = TabletStop(tablet);
  }

  /* poll the file descriptor to check for waiting bytes */
  fds.fd = tablet->fd;
  fds.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
  while( poll(&fds, 1, 100) > 0 ){
    if( fds.revents & fds.events ){
      read(tablet->fd, (void *) &c, 1);
    }
  }

  /* restart the tablet */
  TabletStart(tablet);

  return errNum;
}
