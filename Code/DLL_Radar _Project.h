
/************** Static Function Declarations **************/
typedef struct {
    int x, y;
    int r, g, b;
} RadarResult;

typedef struct {
    double min;
    double max;
    double avg;
} StatsData;
/************** Global Variable Declarations **************/

/************** Global Function Declarations **************/
RadarResult __declspec(dllexport)  CalcRadarPoint(int angle, int distance, int maxDistance, int canvasWidth, int canvasHeight,double scale);
StatsData _declspec(dllexport) CalcStats(const double* values, int size);