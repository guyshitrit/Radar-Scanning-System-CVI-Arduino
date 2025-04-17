#include <formatio.h>
#include <utility.h>
#include <ansi_c.h>
#include <rs232.h>
#include <cvirte.h>
#include <userint.h>
#include <math.h>
#include "Final_project.h"
#include "DLL_Final_Project.h"


#define MAX_DISTANCE 768
#define MIN_DISTANCE 0
#define MAX_GRAPH_POINTS 20000

int port = 11; // COM
//trigPin = 10;
//echoPin = 11;
//servo pin = 12;


static int panelHandle,panelHandle2,imagehandle;


int radarAngle,radarDistance,radarRunning = 1;
int centerX,centerY,canvasHeight,canvasWidth;
double scale=1.8; // adjust according to area
static int x=0; //led

void DrawRadar(int panel);
void CVICALLBACK UpdateGraphUI (void *data);

//Graph:
static int graphPlotID = -1;
double graphTimes[MAX_GRAPH_POINTS];
double graphDistances[MAX_GRAPH_POINTS];
int graphIndex = 0;
double timeCounter = 0.0;

//Thread:
CmtThreadFunctionID Graph_thread;
CmtThreadFunctionID MinMax_thread;
static CmtThreadLockHandle graphLock;
int CVICALLBACK StatsThread  (void *functionData);
int CVICALLBACK GraphUpdateThread (void *functionData);

void CVICALLBACK UpdateStatsUI (void *callbackData);
int CVICALLBACK StatsThread (void *functionData);


int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */

	if ((panelHandle = LoadPanel (0, "Final_project.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle2 = LoadPanel (0, "Final_project.uir", PANEL_2)) < 0)
		return -1;	
	
	DisplayPanel (panelHandle);
	
	// open serial port
	if (OpenComConfig(port, "", 9600, 0, 8, 1, 512, 512) < 0)
	{
		MessagePopup("Error", "Failed to open COM port. Check connection or COM number.");
		return -1;
	}
	FlushInQ(port);
	
	GetCtrlAttribute(panelHandle, PANEL_CANVAS, ATTR_WIDTH, &canvasWidth);
	GetCtrlAttribute(panelHandle, PANEL_CANVAS, ATTR_HEIGHT, &canvasHeight);
	centerX = canvasWidth / 2;
	centerY = canvasHeight / 2;
	
	SetCtrlVal(panelHandle, PANEL_STATUS, "ON");
	
	if(GetBitmapFromFile("RADAR.png", &imagehandle)<0)
		 MessagePopup("Error", "Failed to load image.");
	
	CmtNewLock (NULL, OPT_TL_SUPPORT_TIMEOUT, &graphLock);
	CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, GraphUpdateThread, NULL,&Graph_thread);
	CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, StatsThread, NULL,&MinMax_thread);
	
	RunUserInterface ();
	
	CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, Graph_thread);
	CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, MinMax_thread);
	
	DiscardPanel (panelHandle);
	DiscardPanel (panelHandle2);
	CmtDiscardLock(graphLock);
	CloseCom(port);
	
	return 0;
}

int CVICALLBACK QuitCallback (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK RadarTimer (int panel, int control, int event,
                            void *callbackData, int eventData1, int eventData2)
{
    char buffer[100] = "";  // store data from arduino
    int angle = -1, distance = -1;
    int inQueue = 0; // bytes in queue

    switch (event)
    {
        case EVENT_TIMER_TICK:
            x = !x;
            SetCtrlVal(panelHandle, PANEL_LED, x); //led

            // check how many bytes are waiting in the input buffer
            inQueue= GetInQLen(port);
			SetCtrlVal(panelHandle, PANEL_Queue, inQueue);
			
            // read only if there is a complete string
            if (inQueue >= 8)
            {
                if (ComRdTerm(port, buffer, sizeof(buffer), '.') > 0) // read a message from the COM port until point
                {

                    if (strchr(buffer, ','))  //format check
                    {
                        if (sscanf(buffer, "%d,%d", &angle, &distance) == 2) //grab the angle and distance values
                        {
                            // validate distance
                            if (distance < MIN_DISTANCE || distance > MAX_DISTANCE)
                                return 0;

                            // update UI
                            SetCtrlVal(panelHandle, PANEL_ANGLE, angle);
                            SetCtrlVal(panelHandle, PANEL_DISTANCE, distance);
							
							 // Store latest readings
                            radarAngle = angle;
                            radarDistance = distance;

                            // Redraw background if at edges
                            if (angle == 0 || angle == 180)
                            {
                                CanvasDrawBitmap(panelHandle, PANEL_CANVAS, imagehandle, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
                            }

                            DrawRadar(panelHandle); // draw radar line and point
							
							// add point to graph arrays
							if (graphIndex < MAX_GRAPH_POINTS)
								{
								    CmtGetLock(graphLock); // lock shared data

								    graphDistances[graphIndex] = radarDistance;
								    graphTimes[graphIndex] = timeCounter;
								    graphIndex++;
								    timeCounter += 0.1; // update every 0.1 sec

								    CmtReleaseLock(graphLock); // Unlock
								}


							  // Log to file
                 /*           FILE *logFile = fopen("log.txt", "a");
                            if (logFile)
                            {
                                time_t now = time(NULL);
                                struct tm *t = localtime(&now);
                                fprintf(logFile, "[%02d:%02d:%02d] Angle: %d, Distance: %d\n",
                                        t->tm_hour, t->tm_min, t->tm_sec,
                                        angle, distance);
                                fclose(logFile);
							}*/
                        }
                    }
                }
            }
            break;
    }

    return 0;
}

void DrawRadar(int panel)
{
    RadarResult result = CalcRadarPoint(radarAngle, radarDistance, MAX_DISTANCE, canvasWidth, canvasHeight, scale);   // call DLL func to calculate radar point location and color.
 	
	// set pen color RGB values
    SetCtrlAttribute(panel, PANEL_CANVAS, ATTR_PEN_COLOR, MakeColor(result.r, result.g, result.b));
    SetCtrlAttribute(panel, PANEL_CANVAS, ATTR_PEN_WIDTH, 2);
	
	// define start and end points to line
    Point start = { canvasWidth / 2, canvasHeight };
    Point end = { result.x, result.y };
	
	// draw radar line
    CanvasDrawLine(panel, PANEL_CANVAS, start, end);
    CanvasDrawPoint(panel, PANEL_CANVAS, end);
}

int CVICALLBACK StartStop_func (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
			{
				case EVENT_COMMIT:
				radarRunning=!radarRunning;
				if (radarRunning) // Off to On
				{
					FlushInQ(port); //updete the data
					CanvasDrawBitmap(panelHandle, PANEL_CANVAS, imagehandle, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); //clean lines
					SetCtrlVal(panelHandle, PANEL_STATUS, "ON");
				    SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
				}
				else //On to Off
				{  
				    SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
					SetCtrlVal(panelHandle, PANEL_STATUS, "OFF");

				}

			break;
	}
	return 0;
}

int CVICALLBACK Clear_func (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
				CanvasDrawBitmap(panelHandle, PANEL_CANVAS, imagehandle, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); //clean lines
			break;
	}
	return 0;
}

int CVICALLBACK Save_func (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	char filePath[MAX_PATHNAME_LEN];
    int fileDialogStatus;
    double saveStatus=0;
	int fileType,bitmapID;
	switch (event)
	{
		case EVENT_COMMIT:
			if(radarRunning) //save only if radar is off
			{
				MessagePopup("Error", "Should stop the Radar first.");
				return 0;
			}
			GetCtrlVal(panel, PANEL_RING, &fileType); // get type of format
			GetCtrlBitmap (panelHandle, PANEL_CANVAS, 0, &bitmapID);  // capture the current canvas as a bitmap
			    switch (fileType)
			    {
			        case 0:  // BMP
			            fileDialogStatus = FileSelectPopup("", "*.bmp", "*.bmp", 
			                                               "Save Image As BMP", VAL_SAVE_BUTTON, 0, 0, 1, 0, filePath);
			            break;
			        case 1:  // TIFF
			            fileDialogStatus = FileSelectPopup("", "*.tif", "*.tif", 
			                                               "Save Image As TIFF", VAL_SAVE_BUTTON, 0, 0, 1, 0, filePath);
			            break;
			        case 2:  // PNG
			            fileDialogStatus = FileSelectPopup("", "*.png", "*.png", 
			                                               "Save Image As PNG", VAL_SAVE_BUTTON, 0, 0, 1, 0, filePath);
			            break;
			        case 3:  // JPEG
			            fileDialogStatus = FileSelectPopup("", "*.jpg", "*.jpg", 
			                                               "Save Image As JPEG", VAL_SAVE_BUTTON, 0, 0, 1, 0, filePath);
			            break;
			        default:
			            MessagePopup("Error", "Invalid file type.");
			            return -1;
			    }

			    if (fileDialogStatus == 0)  // if user canceled the file dialog
			    {
			        MessagePopup("Error", "No path selected.");
			        return -1;  // User canceled the save dialog
			    }

			    // Save 
			    switch (fileType)
			    {
			        case 0:  // BMP
			            saveStatus = SaveBitmapToBMPFile(bitmapID, filePath);
			            break;
			        case 1:  // TIFF

						saveStatus = SaveBitmapToTIFFile (bitmapID, filePath, TIF_COMPRESSION_ZIP);
			            break;
			        case 2:  // PNG
			            saveStatus = SaveBitmapToPNGFile(bitmapID, filePath);
			            break;
			        case 3:  // JPEG
			            saveStatus = SaveBitmapToJPEGFile(bitmapID, filePath, JPEG_PROGRESSIVE, 100);  // 100 for best quality
			            break;
			    }

			    
			    if (saveStatus ==-1)
			    {
			        MessagePopup("Error", "Failed to save the image.");
					FlushInQ(port);
			        return -1;
			    }

			    MessagePopup("Success", "Image saved successfully.");
				DiscardBitmap(bitmapID);
			    return 0;
	}
	 return 0;
}

int CVICALLBACK OpenPanel_func (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel (panelHandle2);
			break;
	}
	return 0;
}

int CVICALLBACK ClosePanel_func (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(panelHandle2);
			break;
	}
	return 0;
}

int CVICALLBACK GraphUpdateThread (void *functionData)
{
    while (1)
    {
        if (graphIndex > 0)
        {
			UpdateGraphUI(NULL);
        }

        Delay(0.2);
    }

    return 0;
}

void CVICALLBACK UpdateGraphUI (void *data)
{
    if (graphPlotID < 0)// First time 
		graphPlotID = PlotXY(panelHandle2, PANEL_2_GRAPH,graphTimes, graphDistances, graphIndex,VAL_DOUBLE, VAL_DOUBLE,VAL_CONNECTED_POINTS,VAL_SOLID_CIRCLE, VAL_EMPTY_SQUARE, 1, VAL_RED);
    else
    {
		DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH, -1, VAL_DELAYED_DRAW);
		graphPlotID = PlotXY(panelHandle2, PANEL_2_GRAPH,graphTimes, graphDistances, graphIndex,VAL_DOUBLE, VAL_DOUBLE,VAL_CONNECTED_POINTS, VAL_SOLID_CIRCLE,VAL_EMPTY_SQUARE, 1, VAL_RED);
        RefreshGraph(panelHandle2, PANEL_2_GRAPH); //Not blinking
    }
}

int CVICALLBACK StatsThread (void *functionData)
{
	while (1)
	    {
	        if (graphIndex > 0) //if not empty array
	        {
	            CmtGetLock(graphLock); //lock share graphDistances
	            StatsData* stats = malloc(sizeof(StatsData)); //to get the results in struct
	            *stats = CalcStats(graphDistances, graphIndex);  //calc
	            CmtReleaseLock(graphLock);//Unloack

	            UpdateStatsUI(stats);
	        }

	        Delay(0.5);
    }

    return 0;
}

void CVICALLBACK UpdateStatsUI (void *callbackData)
{
    StatsData* stats = (StatsData*)callbackData;

    SetCtrlVal(panelHandle2, PANEL_2_MIN_DIS, stats->min);
    SetCtrlVal(panelHandle2, PANEL_2_MAX_DIS, stats->max);
    SetCtrlVal(panelHandle2, PANEL_2_AVG_DIS, stats->avg);

    free(stats); //finish with stats now
}
