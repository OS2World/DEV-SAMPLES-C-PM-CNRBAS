/*------------------------
   CNRBAS.H header file
  ------------------------*/
#define CNR_SAMPLE_ID   100

#define SAMPLE_MENU_ID        201
#define SAMPLE_MAIN_MENU      202
#define TEXTV_ID              203
#define TEXTV_FLOWED_ID       204
#define NAMEV_ID              205
#define NAMEV_FLOWED_ID       206
#define ICONV_ID              207
#define TREEV_ID              208
#define DETAILSV_ID           209
#define SAMPLE_MAIN_EXIT      210
#define SAMPLE_MENU_QUIT      211

#define ID_PERSON_ICON  300
#define ID_JOB_ICON     301

#define TEXT_SIZE  40
#define JR_DEVELOPMENT  1
#define JR_SUPPORT      2

/* Structures for sample program */
typedef struct _SAMPLEINFO
{
  HWND        hwndCnr;
  HPOINTER    hptrPersonIcon;
  HPOINTER    hptrJobIcon;
  PFIELDINFO  pFieldInfoLast;
  BOOL        bTreePopulated;
  PSZ         pszCnrTitle;
} SAMPLEINFO;
typedef SAMPLEINFO *PSAMPLEINFO;

typedef struct _PERSONRECORD
{
  MINIRECORDCORE  MiniRec;          /* Container record               */
  CDATE           DateOfBirth;      /* Date of birth field            */
  CTIME           TimeOfBirth;      /* Time of birth field            */
  ULONG           CurrentAge;       /* Current age field              */
  PSZ             pszMiddleInit;    /* Pointer to middle initial data */
  CHAR            szMiddleInit[2];  /* Middle initial data            */
  USHORT          usJob;            /* Main job responsibility.       */
} PERSONRECORD;
typedef PERSONRECORD *PPERSONRECORD;

/* Function prototypes for functions contained in cnrbas.c */
MRESULT EXPENTRY CnrSampleWndProc (HWND hwnd, ULONG msg,
                                   MRESULT mp1, MRESULT mp2);
BOOL CreateCnr (HWND hwnd);
BOOL PopulateCnr (HWND hwnd);
BOOL SetupAndAddFieldInfos (HWND hwnd);
BOOL PopulateTree (HWND hwnd);
BOOL AddChildren (HWND hwnd, PPERSONRECORD pParentRec);
VOID CleanupCnr (HWND hwnd);
VOID CleanupChildren (HWND hwnd, PPERSONRECORD pParentRec);
