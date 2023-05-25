/* ===================================================================*/
/*            Basic Container Sample program                          */
/* ------------------------------------------------------------------ */
/*                                                                    */
/*  Functional Description:                                           */
/*                                                                    */
/*    This program demonstrates an application using a basic          */
/*    container window.  All views of the container are utilized.     */
/*    This sample program is not intended to show all of the          */
/*    container's features or function, but to provide a simple       */
/*    example of how to program to the container utilizing some       */
/*    of the more basic and common functions.  The function           */
/*    shown in this sample includes:                                  */
/*                                                                    */
/*    - setting up a container window                                 */
/*    - setting up various container views                            */
/*    - switching between the container views                         */
/*    - allocating record memory                                      */
/*    - setting up records                                            */
/*    - inserting records                                             */
/*    - freeing resources when the application is closed              */
/*                                                                    */
/* ===================================================================*/
#define INCL_WINWINDOWMGR
#define INCL_WINSYS
#define INCL_WINPOINTERS
#define INCL_WINSTDCNR
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "cnrbas.h"

int main(void)
{
  HAB   hab;
  HMQ   hmq;
  QMSG  qmsg;
  HWND  hwndFrame;
  HWND  hwndClient;
  ULONG fcf = FCF_TITLEBAR | FCF_SIZEBORDER | FCF_SYSMENU |
              FCF_ICON | FCF_MENU | FCF_MINMAX | FCF_SHELLPOSITION;

  hab = WinInitialize (0);
  hmq = WinCreateMsgQueue (hab, 0);

  /* Register the Container sample window class.  Also reserve 4 bytes
   * of memory to store a pointer to our control block.
   */
  WinRegisterClass (hab, (PCSZ) "Container Sample",
                    CnrSampleWndProc, 0, 4);

  /* Create a standard frame window. */
  hwndFrame = WinCreateStdWindow (HWND_DESKTOP, /* Handle of desktop  */
                           WS_VISIBLE,          /* Window style       */
                           &fcf,                /* Creation flags     */
                           (PCSZ) "Container Sample",  /* Client Class name  */
                           (PCSZ) "Container Sample",  /* Title Bar text     */
                           0,                   /* client wnd style   */
                           NULLHANDLE,          /* hwnd of resources  */
                           SAMPLE_MENU_ID,      /* ID of Resources    */
                           &hwndClient);        /* handle of Client   */

  while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
  {
    WinDispatchMsg (hab, &qmsg);
  }

  WinDestroyWindow (hwndFrame);
  WinDestroyMsgQueue (hmq);
  WinTerminate (hab);
}

/*----------------------------------------------------------------------
 Function Name: CnrSampleWndProc

 Description:
   This is the window procedure for the client are of our window.  It is
   also the owner of the container control window.

 Parameters:
   (HWND)    hwnd - The handle of the client window.
   (ULONG)   msg  - The message to be processed.
   (MRESULT) mp1  - The first message parameter for the message.
   (MRESULT) mp2  - The second message parameter for the message.
----------------------------------------------------------------------*/
MRESULT EXPENTRY CnrSampleWndProc (HWND hwnd, ULONG msg,
                                   MRESULT mp1, MRESULT mp2)
{
  SWP          swp;
  HPS          hps;
  RECTL        rect;
  CNRINFO      CnrInfo;
  PSAMPLEINFO  pSampleInfo;

  switch (msg)
  {
    case WM_CREATE:
      /* Create the container window.  If it creates successfully,
       * return FALSE, otherwise return TRUE to indicate an error.
       */
      if (CreateCnr (hwnd))
      {
        return ((MRESULT)FALSE);
      }
      else
      {
        return ((MRESULT)TRUE);
      }

    case WM_PAINT:
      hps = WinBeginPaint (hwnd, NULLHANDLE, &rect);
      WinFillRect (hps, &rect, SYSCLR_BACKGROUND);
      WinEndPaint (hps);
    break;

    case WM_SIZE:
      /* Size the container window to cover the entire client area.
       * General note: When setting the position of only 1 window, for
       * performance reasons, use WinSetMultWindowPos instead of
       * WinSetWindowPos.  The reason for this is because when
       * setting the position of 1 window with WinSetWindowPos,
       * inside of PMWIN, WinSetWindowPos calls WinSetMultWindowPos
       * with a value of 1.  Therefore, coding as we do below shortens
       * the code path as well as execution time.  This will not
       * make or break your application, but is a good tip...also
       * good for as a trivia question at your next office party.
       */
      swp.fl = SWP_MOVE | SWP_SIZE;
      swp.x = swp.y = 0;
      swp.cx = SHORT1FROMMP(mp2);
      swp.cy = SHORT2FROMMP(mp2);
      swp.hwndInsertBehind = NULLHANDLE;
      swp.hwnd = WinWindowFromID (hwnd, CNR_SAMPLE_ID);
      WinSetMultWindowPos (WinQueryAnchorBlock (hwnd), &swp, 1);
    break;

    case WM_DESTROY:
      /* When the user double clicks on the system menu of our frame
       * window, or chooses the QUIT option from our menu, call a
       * function to free up the resources utilized by this application.
       */
      CleanupCnr (hwnd);
    break;

    case WM_COMMAND:
      /* Get the pointer to our application's control block. */
      pSampleInfo = (PSAMPLEINFO)WinQueryWindowPtr (hwnd, QWL_USER);

      /* The following messages are received when the user chooses
       * one of the menu options on our action bar.
       */
      switch (SHORT1FROMMP(mp1))
      {
        case TEXTV_ID:
          /* Switch the container to Text view. */
          CnrInfo.flWindowAttr = CV_TEXT;
          WinSendMsg (pSampleInfo->hwndCnr, CM_SETCNRINFO,
                      MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR));
        break;

        case TEXTV_FLOWED_ID:
          /* Switch the container to Flowed Text view. */
          CnrInfo.flWindowAttr = CV_TEXT | CV_FLOW;
          WinSendMsg (pSampleInfo->hwndCnr, CM_SETCNRINFO,
                      MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR));
        break;

        case NAMEV_ID:
          /* Switch the container to Name view with a container
           * title.
           */
          CnrInfo.flWindowAttr = CV_NAME | CA_CONTAINERTITLE |
                                 CA_TITLESEPARATOR;
          CnrInfo.pszCnrTitle = pSampleInfo->pszCnrTitle;
          WinSendMsg (pSampleInfo->hwndCnr, CM_SETCNRINFO,
                      MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR |
                                                    CMA_CNRTITLE));
        break;

        case NAMEV_FLOWED_ID:
          /* Switch the container to Flowed Name view with a
           * container title.
           */
          CnrInfo.flWindowAttr = CV_NAME | CV_FLOW;
          WinSendMsg (pSampleInfo->hwndCnr, CM_SETCNRINFO,
                      MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR));
        break;

        case ICONV_ID:
          /* Set the container to Icon view with a container title,
           * then arrange it so the records display orderly.
           */
          CnrInfo.flWindowAttr = CV_ICON | CA_CONTAINERTITLE |
                                 CA_TITLESEPARATOR;
          CnrInfo.pszCnrTitle = pSampleInfo->pszCnrTitle;

          WinSendMsg (pSampleInfo->hwndCnr, CM_SETCNRINFO,
                      MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR |
                                                    CMA_CNRTITLE));
          WinSendMsg (pSampleInfo->hwndCnr, CM_ARRANGE, NULL, NULL);
        break;

        case TREEV_ID:
          /* Switch the container to Tree view.  If this is the
           * first time the user has requested Tree view, populate
           * it first.  Also, add the container title to Tree view.
           */
          if (!pSampleInfo->bTreePopulated)
          {
            if (PopulateTree (hwnd))
            {
              pSampleInfo->bTreePopulated = TRUE;
            }
            else
            {
              return ((MRESULT)NULL);
            }
          }
          CnrInfo.flWindowAttr = CV_TREE | CV_ICON | CA_TREELINE |
                                 CA_CONTAINERTITLE | CA_TITLESEPARATOR;
          CnrInfo.pszCnrTitle = pSampleInfo->pszCnrTitle;

          WinSendMsg (pSampleInfo->hwndCnr, CM_SETCNRINFO,
                      MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR |
                                                    CMA_CNRTITLE));
        break;

        case DETAILSV_ID:
          /* Switch the container to Details view.  Set the
           * splitbar in the middle of the container window.
           */
          WinQueryWindowRect (pSampleInfo->hwndCnr, &rect);
          CnrInfo.flWindowAttr = CV_DETAIL | CA_DETAILSVIEWTITLES;
          CnrInfo.xVertSplitbar = rect.xRight / 2;
          CnrInfo.pFieldInfoLast = pSampleInfo->pFieldInfoLast;
          WinSendMsg (pSampleInfo->hwndCnr, CM_SETCNRINFO,
                      MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR |
                      CMA_XVERTSPLITBAR | CMA_PFIELDINFOLAST));
        break;

        case SAMPLE_MENU_QUIT:
          /* The user has requested to quit the application.  Post
           * a WM_QUIT to ourselves and bail.
           */
          WinPostMsg (hwnd, WM_QUIT, 0, 0);
        break;

        default:
          return (WinDefWindowProc (hwnd, msg, mp1, mp2));
      }
      break;

    default:
      return (WinDefWindowProc (hwnd, msg, mp1, mp2));
  }
  return (0);
}

/*----------------------------------------------------------------------
 Function Name: CreateCnr

 Description:
   This function creates a container window, then populates the
   container with 5 records.

 Parameters:
   (HWND) hwnd - The handle of the client window that we are creating
                 the container in.

 Return Values:
   (BOOL)  TRUE  - Successful creation of the container window.
           FALSE - Container window not created successfully due to
                   an error.
----------------------------------------------------------------------*/
BOOL CreateCnr (HWND hwnd)
{
  HWND         hwndCnr;
  PSAMPLEINFO  pSampleInfo;
  CNRINFO      CnrInfo;
  BOOL         rc = TRUE;

  /* Create the container window. */
  hwndCnr = WinCreateWindow (hwnd,                    /* Parent       */
                         WC_CONTAINER,                /* Class        */
                         NULL,                        /* Window text  */
                         WS_VISIBLE | CCS_READONLY |  /* Window style */
                         CCS_EXTENDSEL | CCS_MINIRECORDCORE,
                         0,0,                         /* x,y          */
                         0,0,                         /* cx, cy       */
                         hwnd,                        /* Owner        */
                         HWND_TOP,                    /* placement    */
                         CNR_SAMPLE_ID,               /* Window ID    */
                         NULL,                        /* Control data */
                         NULL);                       /* presparams   */

  /* If the window is created successfully, allocate some memory
   * for our control block and initialize it to 0.  Then, set the
   * information in it and store a pointer to this structure in the
   * window words we reserved on the WinRegisterClass call.
   */
  if (hwndCnr)
  {
    pSampleInfo = malloc (sizeof(SAMPLEINFO));
    if (pSampleInfo)
    {
      memset (pSampleInfo, 0, sizeof(SAMPLEINFO));
      pSampleInfo->hwndCnr = hwndCnr;
      WinSetWindowPtr (hwnd, QWL_USER, pSampleInfo);

      /* Give the container a title and a horizontal separator to
       * separate the title from the viewport.  Store a pointer to
       * the container title in our control block since we will
       * need it whenever the user switches to a view that we want
       * to display the title in.
       */
      pSampleInfo->pszCnrTitle = malloc(TEXT_SIZE);
      if (pSampleInfo->pszCnrTitle)
      {
        strcpy ((char *) pSampleInfo->pszCnrTitle,
                "Basic Container\r\nSample Program");
        CnrInfo.pszCnrTitle = pSampleInfo->pszCnrTitle;
        CnrInfo.flWindowAttr = CV_ICON | CA_CONTAINERTITLE |
                               CA_TITLESEPARATOR;
        WinSendMsg (hwndCnr,
                    CM_SETCNRINFO,
                    MPFROMP(&CnrInfo),
                    MPFROMLONG(CMA_CNRTITLE | CMA_FLWINDOWATTR));

        /* Populate the container with 5 records.  This function will
         * also set up the details view information for each record.
         */
        if (!PopulateCnr (hwnd))
        {
          rc = FALSE;
        }
      }
      else
      {
        rc = FALSE;  /* Out of memory error */
      }
    }
    else
    {
      rc = FALSE; /* Out of memory error */
    }
  }
  else
  {
    rc = FALSE; /* Creation error */
  }

  if (!rc)
  {
    if (hwndCnr)
    {
      WinDestroyWindow (hwndCnr);
    }
    if (pSampleInfo)
    {
      if (pSampleInfo->pszCnrTitle)
      {
        free (pSampleInfo->pszCnrTitle);
      }
      free (pSampleInfo);
    }
  }
  else
  {
    /* Give the container the focus and return. */
    WinSetFocus (HWND_DESKTOP, hwndCnr);
  }
  return (rc);
}

/*----------------------------------------------------------------------
 Function Name: PopulateCnr

 Description:
   This function allocates 5 container records, assigns the appropriate
   information, including the information needed for details view and
   finally inserts the 5 records into the container.

 Parameters:
   (HWND) hwnd - The handle of the client window.

 Return Values:
   (BOOL)  TRUE  - Records inserted successfully.
           FALSE - Records not inserted due to an error.
----------------------------------------------------------------------*/
BOOL PopulateCnr (HWND hwnd)
{
  PSAMPLEINFO    pSampleInfo;
  PPERSONRECORD  pPersonRec;
  PPERSONRECORD  pPersonRecFirst;
  HPOINTER       hptrPersonIcon;
  USHORT         i;
  RECORDINSERT   RecordInsert;
  BOOL           rc = TRUE;
  ULONG          ulNumRecords = 5;

  /* Get the pointer to our control block. */
  pSampleInfo = (PSAMPLEINFO)WinQueryWindowPtr (hwnd, QWL_USER);

  /* Load the person icon that will be used by the container in views
   * which show icons.  These icons are used multiple times, but
   * be sure to only load them once and reuse the HPOINTER.
   */
  pSampleInfo->hptrPersonIcon = WinLoadPointer (HWND_DESKTOP,
                                                NULLHANDLE,
                                                ID_PERSON_ICON);

  /* Load the job icon that will be used for the child records
   * in Tree View.
   */
  pSampleInfo->hptrJobIcon = WinLoadPointer (HWND_DESKTOP,
                                             NULLHANDLE,
                                             ID_JOB_ICON);

  /* Allocate and insert the fieldinfo structures. */
  if (!SetupAndAddFieldInfos (hwnd))
  {
    return (FALSE);
  }

  /* Allocate a linked list of records.  Note how MP1 is calculated.
   * It is the number of ADDITIONAL bytes that you want the container
   * to allocate per record.  Since PERSONRECORD contains a
   * MINIRECORDCORE, we subtract it out.  Now if you ever change the
   * size of PERSONRECORD by adding or subtracting any extra fields,
   * this code will not have to change.
   */
  pPersonRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                       CM_ALLOCRECORD,
                                       MPFROMLONG(sizeof(PERSONRECORD) -
                                       sizeof(MINIRECORDCORE)),
                                       MPFROMLONG(ulNumRecords));

  /* Save a pointer to the front of the list of records returned.  This
   * will be used when we insert the records at the bottom of this
   * function.
   */
  pPersonRecFirst = pPersonRec;

  /* Loop through all the records assigning the necessary
   * information.
   */
  i = 1;
  while ((pPersonRec) && (rc))
  {
    pPersonRec->MiniRec.hptrIcon = pSampleInfo->hptrPersonIcon;
    pPersonRec->MiniRec.pszIcon = malloc (TEXT_SIZE);

    /* If we successfully allocated the memory, copy the text string
     * to the pszIcon field and go to the next record in the linked
     * list.
     */
    if (pPersonRec->MiniRec.pszIcon)
    {
      /* Set up the record specific data for each record, including
       * the details view data.
       */
      switch (i)
      {
        case 1:
          strcpy ((char *) pPersonRec->MiniRec.pszIcon, "Peter Haggar");
          strcpy (pPersonRec->szMiddleInit, "F");
          pPersonRec->pszMiddleInit = (PSZ) pPersonRec->szMiddleInit;
          pPersonRec->DateOfBirth.month = 2;
          pPersonRec->DateOfBirth.day   = 3;
          pPersonRec->DateOfBirth.year  = 65;
          pPersonRec->TimeOfBirth.hours   = 3;
          pPersonRec->TimeOfBirth.minutes = 30;
          pPersonRec->TimeOfBirth.seconds = 0;
          pPersonRec->CurrentAge = 27;
          pPersonRec->usJob = JR_DEVELOPMENT;
        break;

        case 2:
          strcpy ((char *)pPersonRec->MiniRec.pszIcon, "Peter Brightbill");
          strcpy (pPersonRec->szMiddleInit, "P");
          pPersonRec->pszMiddleInit = (PSZ) pPersonRec->szMiddleInit;
          pPersonRec->DateOfBirth.month = 1;
          pPersonRec->DateOfBirth.day   = 23;
          pPersonRec->DateOfBirth.year  = 65;
          pPersonRec->TimeOfBirth.hours   = 17;
          pPersonRec->TimeOfBirth.minutes = 10;
          pPersonRec->TimeOfBirth.seconds = 47;
          pPersonRec->CurrentAge = 27;
          pPersonRec->usJob = JR_DEVELOPMENT;
        break;

        case 3:
          strcpy ( (char *) pPersonRec->MiniRec.pszIcon, "Bob Jones");
          strcpy (pPersonRec->szMiddleInit, "T");
          pPersonRec->pszMiddleInit = (PSZ) pPersonRec->szMiddleInit;
          pPersonRec->DateOfBirth.month = 7;
          pPersonRec->DateOfBirth.day   = 11;
          pPersonRec->DateOfBirth.year  = 54;
          pPersonRec->TimeOfBirth.hours   = 10;
          pPersonRec->TimeOfBirth.minutes = 56;
          pPersonRec->TimeOfBirth.seconds = 19;
          pPersonRec->CurrentAge = 38;
          pPersonRec->usJob = JR_SUPPORT;
        break;

        case 4:
          strcpy ( (char *) pPersonRec->MiniRec.pszIcon, "Joe Shmo");
          strcpy (pPersonRec->szMiddleInit, "D");
          pPersonRec->pszMiddleInit = (PSZ) pPersonRec->szMiddleInit;
          pPersonRec->DateOfBirth.month = 10;
          pPersonRec->DateOfBirth.day   = 29;
          pPersonRec->DateOfBirth.year  = 70;
          pPersonRec->TimeOfBirth.hours   = 23;
          pPersonRec->TimeOfBirth.minutes = 5;
          pPersonRec->TimeOfBirth.seconds = 36;
          pPersonRec->CurrentAge = 22;
          pPersonRec->usJob = JR_SUPPORT;
        break;

        case 5:
          strcpy ( (char *) pPersonRec->MiniRec.pszIcon, "John Public");
          strcpy (pPersonRec->szMiddleInit, "Q");
          pPersonRec->pszMiddleInit = (PSZ) pPersonRec->szMiddleInit;
          pPersonRec->DateOfBirth.month = 5;
          pPersonRec->DateOfBirth.day   = 17;
          pPersonRec->DateOfBirth.year  = 61;
          pPersonRec->TimeOfBirth.hours   = 19;
          pPersonRec->TimeOfBirth.minutes = 3;
          pPersonRec->TimeOfBirth.seconds = 9;
          pPersonRec->CurrentAge = 31;
          pPersonRec->usJob = JR_DEVELOPMENT;
        break;
      }
      pPersonRec = (PPERSONRECORD)pPersonRec->MiniRec.preccNextRecord;
      i++;
    }
    else
    {
      rc = FALSE;
    }
  }

  /* Now that we have the data assigned, insert the records in the
   * container.  First set up the RECORDINSERT structure which tells
   * the container how to insert the records.
   */
  RecordInsert.cb = sizeof(RECORDINSERT);
  RecordInsert.pRecordOrder = (PRECORDCORE)CMA_FIRST;
  RecordInsert.pRecordParent = NULL;
  RecordInsert.zOrder = CMA_TOP;
  RecordInsert.cRecordsInsert = ulNumRecords;
  RecordInsert.fInvalidateRecord = TRUE;

  /* Since the container will be coming up in Icon view, after inserting
   * the records, send the CM_ARRANGE message to arrange the icons in
   * the container viewport.
   */
  if (WinSendMsg (pSampleInfo->hwndCnr,
                  CM_INSERTRECORD,
                  MPFROMP(pPersonRecFirst),
                  MPFROMP(&RecordInsert)))
  {
    WinSendMsg (pSampleInfo->hwndCnr, CM_ARRANGE, NULL, NULL);
  }
  else
  {
    rc = FALSE;
  }

  return (rc);
}

/*----------------------------------------------------------------------
 Function Name: SetupAndAddFieldInfos

 Description:
   This function allocates, sets up, and inserts the 6 FieldInfo
   structures that are used to describe each column for the Details
   view.

 Parameters:
   (HWND) hwnd - The handle of the client window.

 Return Values:
   (BOOL)  TRUE  - FieldInfo structures successfully inserted.
           FALSE - FieldInfo structures not inserted due to an error.
----------------------------------------------------------------------*/
BOOL SetupAndAddFieldInfos (HWND hwnd)
{
  PSAMPLEINFO      pSampleInfo;
  PFIELDINFO       pFieldInfo;
  PFIELDINFO       pFieldInfoFirst;
  FIELDINFOINSERT  FieldInfoInsert;
  BOOL             rc = TRUE;
  USHORT           usNumFieldInfo = 6;
  USHORT           i = 1;

  /* Get the pointer to our control block. */
  pSampleInfo = (PSAMPLEINFO)WinQueryWindowPtr (hwnd, QWL_USER);

  /* Allocate the 6 FieldInfo structures used for the columns in
   * Details view.
   */
  pFieldInfo = WinSendMsg (pSampleInfo->hwndCnr,
                           CM_ALLOCDETAILFIELDINFO,
                           MPFROMSHORT(usNumFieldInfo),
                           NULL);

  pFieldInfoFirst = pFieldInfo;

  /* Loop through each FieldInfo and assign the data as necessary.
   * Pay particular attention to the offStruct field.  This tells
   * the container at what offset from the beginning of the
   * RECORDCORE/MINIRECORDCORE to find the data that is to be
   * displayed in the column.
   */
  while ((pFieldInfo) && (rc))
  {
    switch (i)
    {
      case 1:
        pFieldInfo->flTitle = CFA_BITMAPORICON;
        pFieldInfo->pTitleData = (PVOID)pSampleInfo->hptrPersonIcon;
        pFieldInfo->flData = CFA_BITMAPORICON | CFA_HORZSEPARATOR |
                             CFA_SEPARATOR;
        pFieldInfo->offStruct = FIELDOFFSET(PERSONRECORD,
                                            MiniRec.hptrIcon);
      break;

      case 2:
        pSampleInfo->pFieldInfoLast = pFieldInfo;
        pFieldInfo->flTitle = CFA_STRING;
        pFieldInfo->pTitleData = malloc(TEXT_SIZE);
        if (pFieldInfo->pTitleData)
        {
          strcpy(pFieldInfo->pTitleData, "Name");
          pFieldInfo->flData = CFA_STRING | CFA_HORZSEPARATOR;
          pFieldInfo->offStruct = FIELDOFFSET(PERSONRECORD,
                                              MiniRec.pszIcon);
        }
        else
        {
          rc = FALSE;
        }
      break;

      case 3:
        pFieldInfo->flTitle = CFA_STRING;
        pFieldInfo->pTitleData = malloc(TEXT_SIZE);
        if (pFieldInfo->pTitleData)
        {
          strcpy(pFieldInfo->pTitleData, "Middle Initial");
          pFieldInfo->flData = CFA_STRING | CFA_CENTER |
                               CFA_HORZSEPARATOR;
          pFieldInfo->offStruct = FIELDOFFSET(PERSONRECORD,
                                              pszMiddleInit);
        }
        else
        {
          rc = FALSE;
        }
      break;

      case 4:
        pFieldInfo->flTitle = CFA_STRING;
        pFieldInfo->pTitleData = malloc(TEXT_SIZE);
        if (pFieldInfo->pTitleData)
        {
          strcpy(pFieldInfo->pTitleData, "Date of Birth");
          pFieldInfo->flData = CFA_DATE | CFA_RIGHT | CFA_HORZSEPARATOR;
          pFieldInfo->offStruct = FIELDOFFSET(PERSONRECORD,
                                              DateOfBirth);
        }
        else
        {
          rc = FALSE;
        }
      break;

      case 5:
        pFieldInfo->flTitle = CFA_STRING;
        pFieldInfo->pTitleData = malloc(TEXT_SIZE);
        if (pFieldInfo->pTitleData)
        {
          strcpy(pFieldInfo->pTitleData, "Time of Birth");
          pFieldInfo->flData = CFA_TIME | CFA_RIGHT | CFA_HORZSEPARATOR;
          pFieldInfo->offStruct = FIELDOFFSET(PERSONRECORD,
                                              TimeOfBirth);
        }
        else
        {
          rc = FALSE;
        }
      break;

      case 6:
        pFieldInfo->flTitle = CFA_STRING | CFA_CENTER;
        pFieldInfo->pTitleData = malloc(TEXT_SIZE);
        if (pFieldInfo->pTitleData)
        {
          strcpy(pFieldInfo->pTitleData, "Current\r\nAge");
          pFieldInfo->flData = CFA_ULONG | CFA_RIGHT |
                               CFA_HORZSEPARATOR;
          pFieldInfo->offStruct = FIELDOFFSET(PERSONRECORD,
                                              CurrentAge);
        }
        else
        {
          rc = FALSE;
        }
      break;
    }
    pFieldInfo = pFieldInfo->pNextFieldInfo;
    i++;
  }

  if (rc)
  {
    FieldInfoInsert.cb = sizeof(FIELDINFOINSERT);
    FieldInfoInsert.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
    FieldInfoInsert.cFieldInfoInsert = usNumFieldInfo;
    FieldInfoInsert.fInvalidateFieldInfo = FALSE;
    WinSendMsg (pSampleInfo->hwndCnr, CM_INSERTDETAILFIELDINFO,
                MPFROMP(pFieldInfoFirst), MPFROMP(&FieldInfoInsert));
  }

  return (rc);
}

/*----------------------------------------------------------------------
 Function Name: PopulateTree

 Description:
   This functions adds 3 child records to each of the records that are
   currently in the container.

 Parameters:
   (HWND) hwnd - The handle of the client window.

 Return Values:
   (BOOL)  TRUE  - Tree view children inserted successfully.
           FALSE - Tree view children not inserted due to an error.
----------------------------------------------------------------------*/
BOOL PopulateTree (HWND hwnd)
{
  PSAMPLEINFO    pSampleInfo;
  PPERSONRECORD  pPersonRec;
  BOOL           rc = TRUE;

  /* Get the pointer to our control block. */
  pSampleInfo = (PSAMPLEINFO)WinQueryWindowPtr (hwnd, QWL_USER);

  /* Get the first root item in the tree.  Add its children, then
   * continue adding the children of the next root level item.
   */
  pPersonRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                          CM_QUERYRECORD,
                                          NULL,
                              MPFROM2SHORT(CMA_FIRST, CMA_ITEMORDER));

  while ((pPersonRec) && (rc))
  {
    if (AddChildren (hwnd, pPersonRec))
    {
      pPersonRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                              CM_QUERYRECORD,
                                              MPFROMP(pPersonRec),
                                 MPFROM2SHORT(CMA_NEXT, CMA_ITEMORDER));
    }
    else
    {
      rc = FALSE;
    }
  }

  return (rc);
}

/*----------------------------------------------------------------------
 Function Name: AddChildren

 Description:
   This function allocates the record memory for each child record and
   inserts them as children of the parent record passed in as the
   second parameter.

 Parameters:
   (HWND) hwnd                - The handle of the client window.
   (PPERSONRECORD) pParentRec - The parent record for the children
                                records we are creating.

 Return Values:
   (BOOL)  TRUE  - Children records inserted successfully.
           FALSE - Children records not inserted due to an error.
----------------------------------------------------------------------*/
BOOL AddChildren (HWND hwnd, PPERSONRECORD pParentRec)
{
  PSAMPLEINFO    pSampleInfo;
  PPERSONRECORD  pChildRec;
  PPERSONRECORD  pChildRecFirst;
  RECORDINSERT   RecordInsert;
  BOOL           rc = TRUE;
  ULONG          ulNumChildren = 3;

  /* Get the pointer to our control block. */
  pSampleInfo = (PSAMPLEINFO)WinQueryWindowPtr (hwnd, QWL_USER);

  /* Allocate the 3 records which will be used as children. */
  pChildRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                       CM_ALLOCRECORD,
                                       MPFROMLONG(sizeof(PERSONRECORD) -
                                       sizeof(MINIRECORDCORE)),
                                       MPFROMLONG(ulNumChildren));

  pChildRecFirst = pChildRec;

  /* Go through each record and assign the data as necessary. */
  pChildRec->MiniRec.hptrIcon = pSampleInfo->hptrJobIcon;
  pChildRec->MiniRec.pszIcon = malloc(TEXT_SIZE);
  if (pChildRec->MiniRec.pszIcon)
  {
    if (pParentRec->usJob == JR_DEVELOPMENT)
    {
      strcpy ( (char *) pChildRec->MiniRec.pszIcon, "Design");
    }
    else
    {
      strcpy ( (char *) pChildRec->MiniRec.pszIcon, "Planning");
    }
  }
  else
  {
    rc = FALSE;
  }

  if (rc)
  {
    pChildRec = (PPERSONRECORD)pChildRec->MiniRec.preccNextRecord;
    pChildRec->MiniRec.hptrIcon = pSampleInfo->hptrJobIcon;
    pChildRec->MiniRec.pszIcon = malloc(TEXT_SIZE);
    if (pChildRec->MiniRec.pszIcon)
    {
      if (pParentRec->usJob == JR_DEVELOPMENT)
      {
        strcpy ( (char *) pChildRec->MiniRec.pszIcon, "Coding");
      }
      else
      {
        strcpy ( (char *) pChildRec->MiniRec.pszIcon, "Function\r\nTest");
      }
    }
    else
    {
      rc = FALSE;
    }
  }

  if (rc)
  {
    pChildRec = (PPERSONRECORD)pChildRec->MiniRec.preccNextRecord;
    pChildRec->MiniRec.hptrIcon = pSampleInfo->hptrJobIcon;
    pChildRec->MiniRec.pszIcon = malloc(TEXT_SIZE);
    if (pChildRec->MiniRec.pszIcon)
    {
      /* \r\n or \n will work to create a multi line entry. */
      if (pParentRec->usJob == JR_DEVELOPMENT)
      {
        strcpy ( (char *) pChildRec->MiniRec.pszIcon, "Unit\nTest");
      }
      else
      {
        strcpy ( (char *) pChildRec->MiniRec.pszIcon, "Documentation");
      }
    }
    else
    {
      rc = FALSE;
    }
  }

  /* If no errors, set up the RECORDINSERT structure and insert the
   * child records.  Notice that we set the pRecordParent field of
   * the RECORDINSERT structure to the parent record passed in.
   */
  if (rc)
  {
     RecordInsert.cb = sizeof(RECORDINSERT);
     RecordInsert.pRecordOrder = (PRECORDCORE)CMA_FIRST;
     RecordInsert.pRecordParent = (PRECORDCORE)pParentRec;
     RecordInsert.zOrder = CMA_TOP;
     RecordInsert.cRecordsInsert = ulNumChildren;
     RecordInsert.fInvalidateRecord = TRUE;

     WinSendMsg (pSampleInfo->hwndCnr,
                 CM_INSERTRECORD,
                 MPFROMP(pChildRecFirst),
                 MPFROMP(&RecordInsert));
  }
  return (rc);
}

/*----------------------------------------------------------------------
 Function Name: CleanupCnr

 Description:
   This function is called when this application is closed and will
   free all the memory and resources used by this application.

 Parameters:
   (HWND) hwnd - The handle of the client window.

 Notes:
   It is not necessary to remove and free all of the container records
   when the application is closed.  The container as part of its
   WM_DESTROY processing will do this for you.

 Return Values:
   VOID
----------------------------------------------------------------------*/
VOID CleanupCnr (HWND hwnd)
{
  PSAMPLEINFO    pSampleInfo;
  PPERSONRECORD  pPersonRec;
  PFIELDINFO     pFieldInfo;

  /* Get the pointer to our control block. */
  pSampleInfo = (PSAMPLEINFO)WinQueryWindowPtr (hwnd, QWL_USER);

  /* Make sure it is still valid */
  if (pSampleInfo)
  {
    /* Zip through all the records and free the memory we previously
     * allocated for the text string.
     */
    pPersonRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                            CM_QUERYRECORD,
                                            NULL,
                               MPFROM2SHORT(CMA_FIRST, CMA_ITEMORDER));
    while (pPersonRec)
    {
      if (pPersonRec->MiniRec.pszIcon)
      {
        free (pPersonRec->MiniRec.pszIcon);
      }

      /* For each record, we need to also clean up the memory
       * used for its child records.
       */
      CleanupChildren (hwnd, pPersonRec);

      /* Get the next record and continue. */
      pPersonRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                              CM_QUERYRECORD,
                                              MPFROMP(pPersonRec),
                                 MPFROM2SHORT(CMA_NEXT, CMA_ITEMORDER));
    }

    /* Zip through all the fieldinfo's and free the memory we previously
     * allocated for the title text strings.
     */
    pFieldInfo = WinSendMsg (pSampleInfo->hwndCnr,
                             CM_QUERYDETAILFIELDINFO,
                             NULL,
                             MPFROMSHORT(CMA_FIRST));
    while (pFieldInfo)
    {
      /* If the title data for this column is a string, free the
       * string memory.
       */
      if (!(pFieldInfo->flTitle & CFA_BITMAPORICON))
      {
        if (pFieldInfo->pTitleData)
        {
          free (pFieldInfo->pTitleData);
        }
      }

      /* Go to the next fieldinfo in the container. */
      pFieldInfo = WinSendMsg (pSampleInfo->hwndCnr,
                               CM_QUERYDETAILFIELDINFO,
                               MPFROMP(pFieldInfo),
                               MPFROMSHORT(CMA_NEXT));
    }

    /* Free the icons we used. */
    WinDestroyPointer (pSampleInfo->hptrPersonIcon);
    WinDestroyPointer (pSampleInfo->hptrJobIcon);

    /* Free the container title text string memory we stored in our
     * control block.
     */
    if (pSampleInfo->pszCnrTitle)
    {
      free (pSampleInfo->pszCnrTitle);
    }

    /* Finally, free the SAMPLEINFO control block. */
    free (pSampleInfo);
  }
  return;
}

/*----------------------------------------------------------------------
 Function Name: CleanupChildren

 Description:
   This function will free the resources of the child records used in
   the Tree view.

 Parameters:
   (HWND) hwnd                - The handle of the client window.
   (PPERSONRECORD) pParentRec - Pointer to the parent record whose
                                child records we will be processing.

 Return Values:
   VOID
----------------------------------------------------------------------*/
VOID CleanupChildren (HWND hwnd, PPERSONRECORD pParentRec)
{
  PSAMPLEINFO    pSampleInfo;
  PPERSONRECORD  pChildRec;

  /* Get the pointer to our control block. */
  pSampleInfo = (PSAMPLEINFO)WinQueryWindowPtr (hwnd, QWL_USER);

  /* Get the first child record for the parent record passed in. */
  pChildRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                         CM_QUERYRECORD,
                                         MPFROMP(pParentRec),
                           MPFROM2SHORT(CMA_FIRSTCHILD, CMA_ITEMORDER));

  /* Go through each child record, freeing the memory for the text
   * string in each.
   */
  while (pChildRec)
  {
    if (pChildRec->MiniRec.pszIcon)
    {
      free (pChildRec->MiniRec.pszIcon);
    }

    pChildRec = (PPERSONRECORD)WinSendMsg (pSampleInfo->hwndCnr,
                                           CM_QUERYRECORD,
                                           MPFROMP(pChildRec),
                              MPFROM2SHORT(CMA_NEXT, CMA_ITEMORDER));
  }
  return;
}
