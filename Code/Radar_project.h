/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_QUITBUTTON                 2       /* control type: command, callback function: QuitCallback */
#define  PANEL_CANVAS                     3       /* control type: canvas, callback function: (none) */
#define  PANEL_SAVE_BUT                   4       /* control type: command, callback function: Save_func */
#define  PANEL_CLEAR                      5       /* control type: command, callback function: Clear_func */
#define  PANEL_STARTSTOP                  6       /* control type: command, callback function: StartStop_func */
#define  PANEL_DISTANCE                   7       /* control type: numeric, callback function: (none) */
#define  PANEL_Queue                      8       /* control type: numeric, callback function: (none) */
#define  PANEL_ANGLE                      9       /* control type: numeric, callback function: (none) */
#define  PANEL_TIMER                      10      /* control type: timer, callback function: RadarTimer */
#define  PANEL_LED                        11      /* control type: LED, callback function: (none) */
#define  PANEL_STATUS                     12      /* control type: string, callback function: (none) */
#define  PANEL_RING                       13      /* control type: ring, callback function: (none) */
#define  PANEL_OPENPANL_BUT               14      /* control type: command, callback function: OpenPanel_func */

#define  PANEL_2                          2
#define  PANEL_2_GRAPH                    2       /* control type: graph, callback function: (none) */
#define  PANEL_2_MAX_DIS                  3       /* control type: numeric, callback function: (none) */
#define  PANEL_2_MIN_DIS                  4       /* control type: numeric, callback function: (none) */
#define  PANEL_2_AVG_DIS                  5       /* control type: numeric, callback function: (none) */
#define  PANEL_2_QUITBUTTON1              6       /* control type: command, callback function: ClosePanel_func */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Clear_func(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ClosePanel_func(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenPanel_func(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RadarTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Save_func(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartStop_func(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif