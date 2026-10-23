/* =====================================================================
   PROJECT : SMART CITY - Interactive 2D City Simulator
   SUBJECT : Computer Graphics (CGR) - Diploma in Computer Technology
   LANGUAGE: C (Turbo C / Turbo C++ 3.0, BGI graphics.h)
   MODE    : VGA 640 x 480, 16 colours (VGAHI)

   KEYS    : D = Day     N = Night     + = Faster    - = Slower
             P = Pause   R = Reset     H = Help      ESC = Exit

   NOTE    : Save this file with a DOS-friendly name (SMARTCTY.C).
             Change BGI_PATH below to the folder that contains
             EGAVGA.BGI on your computer.
   ===================================================================== */

#include <graphics.h>
#include <conio.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- settings you may change ---------------- */
#define BGI_PATH      "C:\\TURBOC3\\BGI"
#define STUDENT_NAME  "Divyal"
#define COLLEGE_NAME  "S.E.S. Polytechnic, Solapur"
#define USE_VSYNC     1      /* 1 = wait for screen refresh (less flicker) */
#define FRAME_DELAY   20     /* milliseconds between frames */

/* ---------------- screen layout (pixel coordinates) ---------------- */
#define SCREEN_W        639
#define SCREEN_H        479
#define TITLE_H         24
#define SKY_TOP         25
#define SKY_BAND_BOTTOM 104   /* sky strip that is redrawn every frame */
#define SKY_SPLIT       176   /* colour change line in the day sky      */
#define BASE_Y          262   /* bottom of buildings                    */
#define ROAD_TOP        290
#define ROAD_BOT        380
#define PAVE_LOW_TOP    381
#define GRASS_TOP       395
#define PANEL_TOP       441

#define LANE1_CY        313   /* centre of lane 1 (cars move right)     */
#define LANE2_CY        357   /* centre of lane 2 (cars move left)      */
#define CROSS_X1        300   /* pedestrian crossing (zebra) x-range    */
#define CROSS_X2        350
#define STOP_X_RIGHT    292   /* front of a right-moving car stops here */
#define STOP_X_LEFT     358   /* front of a left-moving car stops here  */
#define SIG_X           366   /* traffic signal pole x position         */

#define PED_START       284   /* pedestrian foot level: top pavement    */
#define PED_END         388   /* pedestrian foot level: bottom pavement */

/* ---------------- cars ---------------- */
#define NUM_CARS        4
#define CAR_W           70
#define MIN_GAP         30    /* safe distance between cars in a lane   */
#define MIN_SPEED       1
#define MAX_SPEED       10
#define DEFAULT_SPEED   3

/* ---------------- other objects ---------------- */
#define NUM_BUILDINGS   9
#define NUM_STARS       60
#define NUM_TREES       6
#define NUM_LAMPS       4
#define SUN_X           560
#define SUN_Y           64

/* ---------------- traffic signal states and timing (in frames) ---------------- */
#define SIG_GREEN       0
#define SIG_YELLOW      1
#define SIG_RED         2
#define GREEN_TIME      140
#define YELLOW_TIME     40
#define RED_TIME        130

/* =====================  GLOBAL VARIABLES  ===================== */
int isNight;                 /* 0 = day, 1 = night                  */
int paused;                  /* 1 = simulation paused               */
int running;                 /* 0 = leave the main loop             */
int carSpeed;                /* pixels moved per frame              */
int signalState, stateTimer; /* current light and frames spent in it*/
int pedY;                    /* pedestrian foot position            */
int frameCount;              /* counts frames (used for animation)  */
int sunAngle;                /* rotation angle of sun rays          */
int cloudX[2];

int skyColor, skyLowColor, paveColor, roadColor, grassColor, bushColor;

int starX[NUM_STARS], starY[NUM_STARS];

int carX[NUM_CARS], carY[NUM_CARS], carDir[NUM_CARS], carLane[NUM_CARS];
int carColor[NUM_CARS], carType[NUM_CARS], carAngle[NUM_CARS];

/* starting values of the cars */
int startX[NUM_CARS]     = { 40, 390, 500, 150 };
int startDir[NUM_CARS]   = {  1,   1,  -1,  -1 };
int startLane[NUM_CARS]  = {  1,   1,   2,   2 };
int startColor[NUM_CARS] = { LIGHTRED, YELLOW, LIGHTBLUE, LIGHTGREEN };
int startType[NUM_CARS]  = {  0,   1,   0,   1 };

/* buildings: x position, width, height, daytime colour */
int bx[NUM_BUILDINGS]  = {   8,  70, 120, 190, 246, 380, 434, 500, 550 };
int bw[NUM_BUILDINGS]  = {  56,  44,  64,  50,  44,  48,  60,  44,  80 };
int bh[NUM_BUILDINGS]  = { 120,  90, 150, 100,  70, 110, 140,  85, 125 };
int bDayColor[NUM_BUILDINGS] = { BROWN, RED, DARKGRAY, MAGENTA, BROWN,
                                 RED, DARKGRAY, BROWN, MAGENTA };

int treeX[NUM_TREES] = { 35, 160, 272, 395, 520, 610 };
int lampX[NUM_LAMPS] = { 100, 215, 440, 580 };

/* text for the help screen */
char *helpKeys[8] = { "D", "N", "+", "-", "P", "R", "H", "ESC" };
char *helpText[8] = {
    "Switch to DAY mode",
    "Switch to NIGHT mode",
    "Increase car speed",
    "Decrease car speed",
    "Pause / Resume the simulation",
    "Reset the simulation",
    "Show this help screen",
    "Exit the program"
};

/* =====================  FUNCTION PROTOTYPES  ===================== */
void fillBox(int x1, int y1, int x2, int y2, int color);
void rotatePoint(int px, int py, int angleDeg, int *rx, int *ry);
int  roundToInt(double v);
void waitVSync(void);
void centerText(int y, char *s, int color, int size);
void setTheme(void);
void initStars(void);
void drawStars(int yMin, int yMax, int twinkle);
void drawSky(void);
void drawSun(int cx, int cy);
void drawMoon(int cx, int cy);
void drawCloud(int x, int y, int size);
void drawDayMode(void);
void drawNightMode(void);
void drawSkyBand(void);
void drawBuilding(int idx);
void drawPavement(void);
void drawTree(int x);
void drawStreetLight(int x);
void drawGrass(void);
void drawPanel(void);
void drawStatus(void);
void drawRoad(void);
void drawWheel(int cx, int cy, int angle);
void drawCar(int x, int y, int dir, int bodyColor, int type, int angle);
void drawTrafficSignal(void);
void drawPedestrian(void);
void drawDynamic(void);
void drawScene(void);
void showWelcome(void);
void showHelp(void);
void resetSimulation(void);
int  crossingClear(void);
void updateSignal(void);
void updateCars(void);
void updatePedestrian(void);
void updateSimulation(void);
void handleKey(int key);

/* =====================  SMALL HELPER FUNCTIONS  ===================== */

/* Draw a solid filled rectangle (no border) */
void fillBox(int x1, int y1, int x2, int y2, int color)
{
    setfillstyle(SOLID_FILL, color);
    bar(x1, y1, x2, y2);
}

/* Round a double value to the nearest int */
int roundToInt(double v)
{
    if (v < 0)
        return (int)(v - 0.5);
    return (int)(v + 0.5);
}

/* 2D ROTATION about the origin:
      x' = x*cos(a) - y*sin(a)
      y' = x*sin(a) + y*cos(a)
   Used for the rotating wheel spokes and the sun rays. */
void rotatePoint(int px, int py, int angleDeg, int *rx, int *ry)
{
    double a, c, s;

    a = angleDeg * 3.14159265 / 180.0;   /* degrees -> radians */
    c = cos(a);
    s = sin(a);
    *rx = roundToInt(px * c - py * s);
    *ry = roundToInt(px * s + py * c);
}

/* Wait for the vertical retrace of the monitor (reduces flicker) */
void waitVSync(void)
{
#if USE_VSYNC
    long guard;

    guard = 0;
    while ((inportb(0x3DA) & 8) && guard < 50000L)
        guard++;
    guard = 0;
    while (!(inportb(0x3DA) & 8) && guard < 50000L)
        guard++;
#endif
}

/* Print a line of text in the horizontal centre of the screen */
void centerText(int y, char *s, int color, int size)
{
    int x;

    settextstyle(DEFAULT_FONT, HORIZ_DIR, size);
    setcolor(color);
    x = (SCREEN_W + 1 - (int)strlen(s) * 8 * size) / 2;
    outtextxy(x, y, s);
}

/* Choose the colour set for day or night */
void setTheme(void)
{
    if (isNight)
    {
        skyColor    = BLUE;
        skyLowColor = BLUE;
        paveColor   = DARKGRAY;
        roadColor   = BLACK;
        grassColor  = GREEN;
        bushColor   = BLACK;
    }
    else
    {
        skyColor    = LIGHTBLUE;
        skyLowColor = LIGHTCYAN;
        paveColor   = LIGHTGRAY;
        roadColor   = DARKGRAY;
        grassColor  = LIGHTGREEN;
        bushColor   = GREEN;
    }
}

/* =====================  SKY, SUN, MOON, STARS, CLOUDS  ===================== */

/* Choose random star positions once (same stars every time) */
void initStars(void)
{
    int i;

    srand(7);
    for (i = 0; i < NUM_STARS; i++)
    {
        starX[i] = rand() % (SCREEN_W - 4) + 2;
        starY[i] = SKY_TOP + 4 + rand() % (BASE_Y - SKY_TOP - 10);
    }
}

/* Draw the stars that lie between yMin and yMax */
void drawStars(int yMin, int yMax, int twinkle)
{
    int i, color;

    for (i = 0; i < NUM_STARS; i++)
    {
        if (starY[i] >= yMin && starY[i] < yMax)
        {
            color = WHITE;
            if (twinkle && ((frameCount / 8 + i) % 6 == 0))
                color = LIGHTGRAY;
            putpixel(starX[i], starY[i], color);
            if (i % 3 == 0)
            {
                putpixel(starX[i] + 1, starY[i], color);
                putpixel(starX[i], starY[i] + 1, color);
            }
        }
    }
}

/* Static part of the sky (drawn when the scene is redrawn) */
void drawSky(void)
{
    fillBox(0, SKY_TOP, SCREEN_W, SKY_SPLIT, skyColor);
    fillBox(0, SKY_SPLIT + 1, SCREEN_W, BASE_Y, skyLowColor);
    if (isNight)
        drawStars(SKY_BAND_BOTTOM, BASE_Y, 0);
}

/* Sun with 8 rotating rays (rotation transformation) */
void drawSun(int cx, int cy)
{
    int k, rx1, ry1, rx2, ry2;

    setcolor(YELLOW);
    setlinestyle(SOLID_LINE, 0, THICK_WIDTH);
    for (k = 0; k < 8; k++)
    {
        rotatePoint(26, 0, sunAngle + k * 45, &rx1, &ry1);
        rotatePoint(38, 0, sunAngle + k * 45, &rx2, &ry2);
        line(cx + rx1, cy + ry1, cx + rx2, cy + ry2);
    }
    setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
    setfillstyle(SOLID_FILL, YELLOW);
    setcolor(YELLOW);
    fillellipse(cx, cy, 20, 20);
}

/* Crescent moon: white circle with a sky-coloured circle cut out of it */
void drawMoon(int cx, int cy)
{
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(cx, cy, 20, 20);
    setcolor(skyColor);
    setfillstyle(SOLID_FILL, skyColor);
    fillellipse(cx + 9, cy - 5, 17, 17);
}

/* Cloud made of four ellipses. size = 10 means normal size,
   size = 7 means 70 % (SCALING transformation). */
void drawCloud(int x, int y, int size)
{
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(x,                  y,                  30 * size / 10, 10 * size / 10);
    fillellipse(x - 18 * size / 10, y - 6 * size / 10,  14 * size / 10,  9 * size / 10);
    fillellipse(x,                  y - 10 * size / 10, 16 * size / 10, 12 * size / 10);
    fillellipse(x + 18 * size / 10, y - 5 * size / 10,  14 * size / 10,  9 * size / 10);
}

/* Things that appear in the sky during the DAY */
void drawDayMode(void)
{
    drawSun(SUN_X, SUN_Y);
    drawCloud(cloudX[0], 52, 10);
    drawCloud(cloudX[1], 88, 7);
}

/* Things that appear in the sky during the NIGHT */
void drawNightMode(void)
{
    drawStars(SKY_TOP, SKY_BAND_BOTTOM, 1);
    drawMoon(SUN_X, SUN_Y);
}

/* The upper strip of the sky is repainted every frame
   (the sun rotates, clouds move, stars twinkle) */
void drawSkyBand(void)
{
    fillBox(0, SKY_TOP, SCREEN_W, SKY_BAND_BOTTOM, skyColor);
    if (isNight)
        drawNightMode();
    else
        drawDayMode();
}

/* =====================  BUILDINGS, TREES, LAMPS  ===================== */

/* One building: body, roof, window grid, door */
void drawBuilding(int idx)
{
    int x, w, h, top, bodyColor, winColor, outline;
    int winCols, winRows, margin, r, c, wx, wy, lit, doorX, doorColor;

    x = bx[idx];
    w = bw[idx];
    h = bh[idx];
    top = BASE_Y - h;

    if (isNight)
    {
        if (idx % 2)
            bodyColor = BLACK;
        else
            bodyColor = DARKGRAY;
        outline = DARKGRAY;
        doorColor = BROWN;
    }
    else
    {
        bodyColor = bDayColor[idx];
        outline = BLACK;
        doorColor = BLUE;
    }

    /* body */
    setfillstyle(SOLID_FILL, bodyColor);
    setcolor(outline);
    bar(x, top, x + w, BASE_Y);
    rectangle(x, top, x + w, BASE_Y);

    /* roof slab */
    if (isNight)
        setfillstyle(SOLID_FILL, DARKGRAY);
    else
        setfillstyle(SOLID_FILL, LIGHTGRAY);
    bar(x - 2, top - 5, x + w + 2, top);
    rectangle(x - 2, top - 5, x + w + 2, top);

    /* antenna on some buildings */
    if (idx % 4 == 0)
    {
        setcolor(LIGHTGRAY);
        line(x + w / 2, top - 5, x + w / 2, top - 16);
        if (isNight)
        {
            setcolor(RED);
            setfillstyle(SOLID_FILL, RED);
            fillellipse(x + w / 2, top - 17, 2, 2);
        }
    }

    /* windows */
    winCols = (w - 6) / 14;
    margin = (w - winCols * 14 + 6) / 2;
    winRows = (h - 36) / 18;
    for (r = 0; r < winRows; r++)
    {
        for (c = 0; c < winCols; c++)
        {
            wx = x + margin + c * 14;
            wy = top + 10 + r * 18;
            if (isNight)
            {
                lit = ((r * 3 + c * 5 + idx) % 4 != 0);
                if (lit)
                    winColor = YELLOW;
                else
                    winColor = BLUE;
            }
            else
                winColor = LIGHTCYAN;
            setfillstyle(SOLID_FILL, winColor);
            bar(wx, wy, wx + 8, wy + 10);
            setcolor(outline);
            rectangle(wx, wy, wx + 8, wy + 10);
        }
    }

    /* door */
    doorX = x + w / 2 - 6;
    setfillstyle(SOLID_FILL, doorColor);
    setcolor(outline);
    bar(doorX, BASE_Y - 18, doorX + 12, BASE_Y - 1);
    rectangle(doorX, BASE_Y - 18, doorX + 12, BASE_Y - 1);
    putpixel(doorX + 9, BASE_Y - 9, YELLOW);
}

/* Upper pavement (sidewalk) */
void drawPavement(void)
{
    fillBox(0, BASE_Y, SCREEN_W, ROAD_TOP - 1, paveColor);
    setcolor(WHITE);
    line(0, ROAD_TOP - 1, SCREEN_W, ROAD_TOP - 1);          /* curb */
    fillBox(0, PAVE_LOW_TOP, SCREEN_W, GRASS_TOP - 1, paveColor);
}

/* Tree = brown trunk + three green ellipses */
void drawTree(int x)
{
    int leaf, edge;

    if (isNight)
    {
        leaf = GREEN;
        edge = BLACK;
    }
    else
    {
        leaf = LIGHTGREEN;
        edge = GREEN;
    }
    fillBox(x - 3, 270, x + 3, 286, BROWN);
    setcolor(edge);
    setfillstyle(SOLID_FILL, leaf);
    fillellipse(x, 258, 14, 16);
    fillellipse(x - 9, 266, 10, 11);
    fillellipse(x + 9, 266, 10, 11);
}

/* Street light: pole, arm and lamp. At night the lamp glows. */
void drawStreetLight(int x)
{
    int lx, ly;

    lx = x + 12;
    ly = 238;
    if (isNight)
        setcolor(LIGHTGRAY);
    else
        setcolor(DARKGRAY);
    setlinestyle(SOLID_LINE, 0, THICK_WIDTH);
    line(x, ROAD_TOP - 2, x, ly);
    setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
    line(x, ly, lx, ly);

    if (isNight)
    {
        setcolor(YELLOW);
        setfillstyle(SOLID_FILL, YELLOW);
        fillellipse(lx, ly + 3, 5, 3);
        circle(lx, ly + 3, 9);
        setlinestyle(DOTTED_LINE, 0, NORM_WIDTH);
        line(lx - 3, ly + 7, lx - 14, ROAD_TOP - 3);
        line(lx + 3, ly + 7, lx + 18, ROAD_TOP - 3);
        setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
    }
    else
    {
        setcolor(DARKGRAY);
        setfillstyle(SOLID_FILL, WHITE);
        fillellipse(lx, ly + 3, 5, 3);
    }
}

/* Grass area with bushes and flowers */
void drawGrass(void)
{
    int x;

    fillBox(0, GRASS_TOP, SCREEN_W, PANEL_TOP - 1, grassColor);
    setcolor(bushColor);
    setfillstyle(SOLID_FILL, bushColor);
    for (x = 20; x < SCREEN_W; x += 70)
        fillellipse(x, 415, 16, 9);
    for (x = 45; x < SCREEN_W; x += 70)
    {
        putpixel(x, 428, YELLOW);
        fillBox(x, 428, x + 2, 430, YELLOW);
        fillBox(x + 22, 432, x + 24, 434, LIGHTRED);
    }
}

/* Bottom panel with keyboard instructions */
void drawPanel(void)
{
    fillBox(0, PANEL_TOP, SCREEN_W, SCREEN_H, BLACK);
    setcolor(LIGHTCYAN);
    line(0, PANEL_TOP, SCREEN_W, PANEL_TOP);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(YELLOW);
    outtextxy(10, PANEL_TOP + 8,  "D:Day  N:Night  +/-:Speed  P:Pause  R:Reset  H:Help  ESC:Exit");
    setcolor(LIGHTGREEN);
    outtextxy(10, PANEL_TOP + 22, "Cars stop at the stop line on RED/YELLOW. Pedestrian crosses on RED.");
    setcolor(LIGHTGRAY);
    outtextxy(10, PANEL_TOP + 36, "Developed by: " STUDENT_NAME);
}

/* Title bar and live status text */
void drawStatus(void)
{
    char buf[48];

    fillBox(0, 0, SCREEN_W, TITLE_H - 1, BLACK);
    setcolor(LIGHTCYAN);
    line(0, TITLE_H, SCREEN_W, TITLE_H);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(YELLOW);
    outtextxy(10, 8, "SMART CITY - 2D City Simulator");

    sprintf(buf, "Mode:%s  Speed:%2d  %s",
            isNight ? "NIGHT" : "DAY  ", carSpeed,
            paused ? "PAUSED " : "RUNNING");
    if (paused)
        setcolor(LIGHTRED);
    else
        setcolor(LIGHTGREEN);
    outtextxy(380, 8, buf);
}

/* =====================  ROAD, CARS, SIGNAL, PEDESTRIAN  ===================== */

/* Road with edge lines, centre dashes, zebra crossing and stop lines */
void drawRoad(void)
{
    int x, y;

    fillBox(0, ROAD_TOP, SCREEN_W, ROAD_BOT, roadColor);

    /* edge lines */
    setcolor(WHITE);
    line(0, ROAD_TOP + 2, SCREEN_W, ROAD_TOP + 2);
    line(0, ROAD_BOT - 2, SCREEN_W, ROAD_BOT - 2);

    /* dashed centre line (skipped near the crossing) */
    setfillstyle(SOLID_FILL, YELLOW);
    for (x = 0; x < SCREEN_W; x += 50)
    {
        if (x + 30 < CROSS_X1 - 6 || x > CROSS_X2 + 6)
            bar(x, 334, x + 30, 336);
    }

    /* zebra crossing */
    setfillstyle(SOLID_FILL, WHITE);
    for (y = ROAD_TOP + 6; y < ROAD_BOT - 6; y += 10)
        bar(CROSS_X1, y, CROSS_X2, y + 5);

    /* stop lines (one for each lane) */
    bar(STOP_X_RIGHT + 1, 295, STOP_X_RIGHT + 4, 331);
    bar(STOP_X_LEFT - 4, 339, STOP_X_LEFT - 1, 375);
}

/* A wheel: black tyre, 6 spokes that ROTATE by 'angle' degrees */
void drawWheel(int cx, int cy, int angle)
{
    int k, rx, ry;

    setcolor(LIGHTGRAY);
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(cx, cy, 7, 7);
    setcolor(WHITE);
    for (k = 0; k < 3; k++)
    {
        rotatePoint(6, 0, angle + k * 60, &rx, &ry);
        line(cx - rx, cy - ry, cx + rx, cy + ry);
    }
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    fillellipse(cx, cy, 2, 2);
}

/* A car. (x, y) = top-left corner of the car's bounding box.
   dir  = +1 moving right, -1 moving left.
   type = 0 sedan, 1 van. */
void drawCar(int x, int y, int dir, int bodyColor, int type, int angle)
{
    int poly[8];
    int frontX, rearX, winColor;

    if (isNight)
        winColor = LIGHTBLUE;
    else
        winColor = LIGHTCYAN;

    setcolor(BLACK);
    setfillstyle(SOLID_FILL, bodyColor);

    if (type == 0)
    {
        /* sedan: low body + trapezoid cabin */
        bar(x, y + 12, x + CAR_W, y + 22);
        rectangle(x, y + 12, x + CAR_W, y + 22);
        poly[0] = x + 14;  poly[1] = y + 12;
        poly[2] = x + 22;  poly[3] = y + 1;
        poly[4] = x + 50;  poly[5] = y + 1;
        poly[6] = x + 58;  poly[7] = y + 12;
        fillpoly(4, poly);
        setfillstyle(SOLID_FILL, winColor);
        bar(x + 24, y + 3, x + 35, y + 10);
        bar(x + 38, y + 3, x + 49, y + 10);
    }
    else
    {
        /* van: tall box body with four windows */
        bar(x, y + 2, x + CAR_W, y + 22);
        rectangle(x, y + 2, x + CAR_W, y + 22);
        setfillstyle(SOLID_FILL, winColor);
        bar(x + 5,  y + 5, x + 17, y + 12);
        bar(x + 21, y + 5, x + 33, y + 12);
        bar(x + 37, y + 5, x + 49, y + 12);
        bar(x + 53, y + 5, x + 65, y + 12);
    }

    /* headlight at the front, tail light at the rear */
    if (dir > 0)
    {
        frontX = x + CAR_W;
        rearX = x;
        setfillstyle(SOLID_FILL, YELLOW);
        bar(frontX - 4, y + 14, frontX, y + 18);
        setfillstyle(SOLID_FILL, LIGHTRED);
        bar(rearX, y + 14, rearX + 3, y + 18);
    }
    else
    {
        frontX = x;
        rearX = x + CAR_W;
        setfillstyle(SOLID_FILL, YELLOW);
        bar(frontX, y + 14, frontX + 4, y + 18);
        setfillstyle(SOLID_FILL, LIGHTRED);
        bar(rearX - 3, y + 14, rearX, y + 18);
    }

    /* light beams at night */
    if (isNight)
    {
        setcolor(YELLOW);
        line(frontX, y + 15, frontX + 24 * dir, y + 10);
        line(frontX, y + 16, frontX + 24 * dir, y + 16);
        line(frontX, y + 17, frontX + 24 * dir, y + 24);
    }

    /* wheels */
    drawWheel(x + 16, y + 23, angle);
    drawWheel(x + 54, y + 23, angle);
}

/* Traffic signal: pole, black box and three lamps */
void drawTrafficSignal(void)
{
    int c1, c2, c3;

    c1 = DARKGRAY;
    c2 = DARKGRAY;
    c3 = DARKGRAY;
    if (signalState == SIG_RED)
        c1 = LIGHTRED;
    else if (signalState == SIG_YELLOW)
        c2 = YELLOW;
    else
        c3 = LIGHTGREEN;

    /* pole */
    setcolor(LIGHTGRAY);
    setlinestyle(SOLID_LINE, 0, THICK_WIDTH);
    line(SIG_X, 248, SIG_X, ROAD_TOP - 2);
    setlinestyle(SOLID_LINE, 0, NORM_WIDTH);

    /* box */
    setfillstyle(SOLID_FILL, BLACK);
    bar(SIG_X - 9, 198, SIG_X + 9, 248);
    setcolor(LIGHTGRAY);
    rectangle(SIG_X - 9, 198, SIG_X + 9, 248);

    /* lamps: red (top), yellow (middle), green (bottom) */
    setcolor(c1);
    setfillstyle(SOLID_FILL, c1);
    fillellipse(SIG_X, 211, 5, 5);
    setcolor(c2);
    setfillstyle(SOLID_FILL, c2);
    fillellipse(SIG_X, 224, 5, 5);
    setcolor(c3);
    setfillstyle(SOLID_FILL, c3);
    fillellipse(SIG_X, 237, 5, 5);
}

/* Pedestrian made of a circle (head), a bar (body) and lines (arms, legs) */
void drawPedestrian(void)
{
    int x, y, swing;

    x = (CROSS_X1 + CROSS_X2) / 2;
    y = pedY;
    swing = 0;
    if (pedY > PED_START && pedY < PED_END)
    {
        if ((frameCount / 4) % 2)
            swing = 3;
        else
            swing = -3;
    }

    /* legs */
    setcolor(LIGHTBLUE);
    line(x - 1, y - 6, x - 1 + swing, y);
    line(x + 1, y - 6, x + 1 - swing, y);
    /* body */
    setfillstyle(SOLID_FILL, LIGHTMAGENTA);
    bar(x - 2, y - 13, x + 2, y - 6);
    /* arms */
    setcolor(LIGHTMAGENTA);
    line(x - 2, y - 12, x - 4, y - 8 + swing / 3);
    line(x + 2, y - 12, x + 4, y - 8 - swing / 3);
    /* head */
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(x, y - 16, 3, 3);
}

/* =====================  DRAWING ONE FRAME / THE WHOLE SCENE  ===================== */

/* Everything that changes while the simulation runs.
   The static background is NOT redrawn here - this avoids flicker. */
void drawDynamic(void)
{
    int i;

    waitVSync();
    drawSkyBand();
    drawTrafficSignal();

    /* repaint the pavement strips around the pedestrian path */
    fillBox(298, 264, 352, 288, paveColor);
    drawRoad();
    fillBox(298, PAVE_LOW_TOP, 352, 392, paveColor);

    for (i = 0; i < NUM_CARS; i++)
        drawCar(carX[i], carY[i], carDir[i], carColor[i], carType[i], carAngle[i]);

    drawPedestrian();
}

/* Complete scene: background + first frame */
void drawScene(void)
{
    int i;

    setTheme();
    cleardevice();
    drawSky();
    for (i = 0; i < NUM_BUILDINGS; i++)
        drawBuilding(i);
    drawPavement();
    for (i = 0; i < NUM_TREES; i++)
        drawTree(treeX[i]);
    for (i = 0; i < NUM_LAMPS; i++)
        drawStreetLight(lampX[i]);
    drawGrass();
    drawPanel();
    drawStatus();
    drawDynamic();
}

/* =====================  WELCOME AND HELP SCREENS  ===================== */

void showWelcome(void)
{
    int i, x, h, r, c, blink;
    char buf[80];

    cleardevice();
    fillBox(0, 0, SCREEN_W, SCREEN_H, BLUE);

    /* stars and moon */
    for (i = 0; i < NUM_STARS; i++)
        if (starY[i] < 300)
            putpixel(starX[i], starY[i], WHITE);
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(570, 70, 25, 25);
    setcolor(BLUE);
    setfillstyle(SOLID_FILL, BLUE);
    fillellipse(582, 62, 22, 22);

    /* skyline at the bottom */
    for (i = 0; i < 13; i++)
    {
        x = i * 50;
        h = 60 + (i * 37) % 90;
        if (i % 2)
            fillBox(x, SCREEN_H - h, x + 44, SCREEN_H, BLACK);
        else
            fillBox(x, SCREEN_H - h, x + 44, SCREEN_H, DARKGRAY);
        for (r = SCREEN_H - h + 8; r < SCREEN_H - 12; r += 16)
        {
            for (c = x + 6; c < x + 38; c += 12)
            {
                if ((r / 16 + c / 12 + i) % 3)
                    fillBox(c, r, c + 6, r + 8, YELLOW);
                else
                    fillBox(c, r, c + 6, r + 8, BLUE);
            }
        }
    }

    /* title with shadow */
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 6);
    setcolor(RED);
    outtextxy(84, 34, "SMART CITY");
    setcolor(YELLOW);
    outtextxy(80, 30, "SMART CITY");
    centerText(100, "Interactive 2D City Simulator", WHITE, 2);

    centerText(145, "Computer Graphics (CGR) Mini Project using Turbo C and BGI", LIGHTCYAN, 1);
    sprintf(buf, "Developed by: %s  |  %s", STUDENT_NAME, COLLEGE_NAME);
    centerText(161, buf, WHITE, 1);
    centerText(185, "A small animated city with day and night modes, moving cars,", LIGHTGRAY, 1);
    centerText(199, "a smart traffic signal and a pedestrian crossing.", LIGHTGRAY, 1);

    /* control guide box */
    setcolor(LIGHTCYAN);
    rectangle(150, 218, 490, 292);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(YELLOW);
    outtextxy(270, 223, "CONTROLS");
    setcolor(WHITE);
    outtextxy(170, 240, "D - Day mode");
    outtextxy(170, 254, "N - Night mode");
    outtextxy(170, 268, "+ - Faster cars");
    outtextxy(170, 282, "- - Slower cars");
    outtextxy(330, 240, "P   - Pause / Resume");
    outtextxy(330, 254, "R   - Reset");
    outtextxy(330, 268, "H   - Help screen");
    outtextxy(330, 282, "ESC - Exit");

    /* blinking message */
    blink = 1;
    while (!kbhit())
    {
        fillBox(0, 298, SCREEN_W, 316, BLUE);
        if (blink)
            centerText(304, "Press any key to start...", YELLOW, 1);
        blink = !blink;
        delay(400);
    }
    while (kbhit())
        getch();
}

void showHelp(void)
{
    int i, y;

    cleardevice();
    setcolor(LIGHTCYAN);
    rectangle(40, 20, 600, 460);
    rectangle(44, 24, 596, 456);

    centerText(38, "HELP SCREEN", YELLOW, 2);
    centerText(80, "KEYBOARD CONTROLS", LIGHTGREEN, 1);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    for (i = 0; i < 8; i++)
    {
        y = 105 + i * 22;
        setcolor(LIGHTCYAN);
        rectangle(104, y - 4, 160, y + 12);
        outtextxy(110, y, helpKeys[i]);
        setcolor(WHITE);
        outtextxy(185, y, helpText[i]);
    }

    centerText(295, "TRAFFIC SIGNAL RULES", LIGHTGREEN, 1);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);

    setcolor(LIGHTGREEN);
    setfillstyle(SOLID_FILL, LIGHTGREEN);
    fillellipse(110, 327, 6, 6);
    setcolor(WHITE);
    outtextxy(135, 323, "GREEN  - cars keep moving");

    setcolor(YELLOW);
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(110, 349, 6, 6);
    setcolor(WHITE);
    outtextxy(135, 345, "YELLOW - cars get ready to stop at the stop line");

    setcolor(LIGHTRED);
    setfillstyle(SOLID_FILL, LIGHTRED);
    fillellipse(110, 371, 6, 6);
    setcolor(WHITE);
    outtextxy(135, 367, "RED    - cars wait, pedestrian crosses the road");

    centerText(425, "Press any key to return to the simulation...", YELLOW, 1);

    getch();
    while (kbhit())
        getch();
}

/* =====================  SIMULATION LOGIC  ===================== */

/* Put everything back to the starting state */
void resetSimulation(void)
{
    int i;

    isNight = 0;
    paused = 0;
    carSpeed = DEFAULT_SPEED;
    signalState = SIG_GREEN;
    stateTimer = 0;
    pedY = PED_START;
    frameCount = 0;
    sunAngle = 0;
    cloudX[0] = 120;
    cloudX[1] = 400;

    for (i = 0; i < NUM_CARS; i++)
    {
        carX[i] = startX[i];
        carDir[i] = startDir[i];
        carLane[i] = startLane[i];
        carColor[i] = startColor[i];
        carType[i] = startType[i];
        carAngle[i] = 0;
        if (carLane[i] == 1)
            carY[i] = LANE1_CY - 15;
        else
            carY[i] = LANE2_CY - 15;
    }
}

/* Returns 1 if no car is on or very near the zebra crossing */
int crossingClear(void)
{
    int i;

    for (i = 0; i < NUM_CARS; i++)
    {
        if (carX[i] + CAR_W > CROSS_X1 - 6 && carX[i] < CROSS_X2 + 6)
            return 0;
    }
    return 1;
}

/* GREEN -> YELLOW -> RED -> GREEN ... (counted in frames) */
void updateSignal(void)
{
    stateTimer++;
    if (signalState == SIG_GREEN && stateTimer >= GREEN_TIME)
    {
        signalState = SIG_YELLOW;
        stateTimer = 0;
    }
    else if (signalState == SIG_YELLOW && stateTimer >= YELLOW_TIME)
    {
        signalState = SIG_RED;
        stateTimer = 0;
    }
    else if (signalState == SIG_RED && stateTimer >= RED_TIME)
    {
        /* stay red while the pedestrian is still on the road */
        if (pedY > PED_START && pedY < PED_END)
            stateTimer = RED_TIME;
        else
        {
            signalState = SIG_GREEN;
            stateTimer = 0;
            pedY = PED_START;
        }
    }
}

/* 2D TRANSLATION of every car:  x' = x + step * direction
   The car obeys the signal and keeps a safe gap from the car ahead. */
void updateCars(void)
{
    int i, j, step, front, stopLine, d, rearJ, gap, allowed;

    for (i = 0; i < NUM_CARS; i++)
    {
        step = carSpeed;

        /* position of the front bumper and of the stop line */
        if (carDir[i] > 0)
        {
            front = carX[i] + CAR_W;
            stopLine = STOP_X_RIGHT;
        }
        else
        {
            front = carX[i];
            stopLine = STOP_X_LEFT;
        }

        /* distance to the stop line (negative = already crossed it) */
        d = (stopLine - front) * carDir[i];

        /* RED or YELLOW: do not go beyond the stop line */
        if (signalState != SIG_GREEN && d >= 0 && step > d)
            step = d;

        /* keep a safe gap from the car ahead in the same lane */
        for (j = 0; j < NUM_CARS; j++)
        {
            if (j != i && carLane[j] == carLane[i])
            {
                if (carDir[i] > 0)
                    rearJ = carX[j];
                else
                    rearJ = carX[j] + CAR_W;
                gap = (rearJ - front) * carDir[i];
                if (gap >= 0)
                {
                    allowed = gap - MIN_GAP;
                    if (allowed < 0)
                        allowed = 0;
                    if (step > allowed)
                        step = allowed;
                }
            }
        }

        /* move the car and rotate its wheels */
        carX[i] = carX[i] + step * carDir[i];
        carAngle[i] = (carAngle[i] + step * 8 * carDir[i]) % 360;

        /* wrap around the screen */
        if (carDir[i] > 0 && carX[i] > SCREEN_W)
            carX[i] = -CAR_W;
        if (carDir[i] < 0 && carX[i] + CAR_W < 0)
            carX[i] = SCREEN_W;
    }
}

/* The pedestrian walks only on RED and only when the crossing is empty */
void updatePedestrian(void)
{
    if (signalState == SIG_RED && pedY < PED_END)
    {
        if (pedY > PED_START || (stateTimer > 15 && crossingClear()))
            pedY = pedY + 2;
    }
}

/* One simulation step */
void updateSimulation(void)
{
    frameCount++;
    sunAngle = (sunAngle + 3) % 360;

    if (frameCount % 2 == 0)
        cloudX[0]++;
    if (frameCount % 3 == 0)
        cloudX[1]++;
    if (cloudX[0] > SCREEN_W + 60)
        cloudX[0] = -60;
    if (cloudX[1] > SCREEN_W + 60)
	cloudX[1] = -60;

    updateSignal();
    updateCars();
    updatePedestrian();
}

/* =====================  KEYBOARD  ===================== */
void handleKey(int key)
{
    if (key == 'd' || key == 'D')
    {
	if (isNight)
	{
	    isNight = 0;
	    drawScene();
	}
    }
    else if (key == 'n' || key == 'N')
    {
	if (!isNight)
	{
	    isNight = 1;
	    drawScene();
	}
    }
    else if (key == '+' || key == '=')
    {
	if (carSpeed < MAX_SPEED)
	    carSpeed++;
	drawStatus();
    }
    else if (key == '-' || key == '_')
    {
	if (carSpeed > MIN_SPEED)
	    carSpeed--;
	drawStatus();
    }
    else if (key == 'p' || key == 'P')
    {
	paused = !paused;
	drawStatus();
    }
    else if (key == 'r' || key == 'R')
    {
	resetSimulation();
	drawScene();
    }
    else if (key == 'h' || key == 'H')
    {
	showHelp();
	drawScene();
    }
    else if (key == 27)
    {
	running = 0;
    }
}

/* =====================  MAIN PROGRAM  ===================== */
int main(void)
{
    int gd, gm, errorcode, key;

    /* 1. start graphics mode: VGA, 640 x 480, 16 colours */
    gd = VGA;
    gm = VGAHI;
    initgraph(&gd, &gm, BGI_PATH);
    errorcode = graphresult();
    if (errorcode != grOk)
    {
	printf("Graphics error: %s\n", grapherrormsg(errorcode));
	printf("Check that BGI_PATH in the program points to the folder\n");
	printf("that contains EGAVGA.BGI.  Current path: %s\n", BGI_PATH);
	printf("Press any key to exit...");
	getch();
	exit(1);
    }

    /* 2. prepare data, show welcome screen */
    initStars();
    resetSimulation();
    showWelcome();

    /* 3. draw the first complete scene */
    resetSimulation();
    drawScene();

    /* 4. main animation loop */
    running = 1;
    while (running)
    {
	/* keyboard: read all waiting keys */
	while (kbhit())
	{
	    key = getch();
	    if (key == 0)
		getch();            /* special key: ignore 2nd code */
	    else
		handleKey(key);
	}

	/* simulation + drawing (skipped while paused) */
	if (running && !paused)
	{
	    updateSimulation();
	    drawDynamic();
	}
	delay(FRAME_DELAY);
    }

    /* 5. leave graphics mode and show exit message */
    closegraph();
    clrscr();
    printf("\n\n");
    printf("  ==============================================\n");
    printf("     SMART CITY - 2D City Simulator\n");
    printf("     Thank you for watching the demonstration!\n");
    printf("     Developed by: %s\n", STUDENT_NAME);
    printf("  ==============================================\n");
    return 0;
}
