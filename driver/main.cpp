#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <chrono>
#include <thread>

using namespace std;

#define VERSION "0.1"

int main(int, char **)
{
    uint8_t gridSize = 25;
    XColor c;
    Display *d = XOpenDisplay((char *)NULL);
    Screen *s = DefaultScreenOfDisplay(d);
    int displayNumber = XDefaultScreen(d);
    
    Window rootWindow = XRootWindow(d, displayNumber);
    Colormap colorMap = XDefaultColormap(d, displayNumber);

    const uint16_t gridW = floor(s->width / gridSize), gridH = floor(s->height / gridSize);
    const double totalPixelsMeasure = gridW * gridH;
    
    printf("Hodini Driver %s. Screen resolution is %dx%d. Split to grid %dx%d\n", VERSION, s->width, s->height, gridW, gridH);
    while(true) {
        double avgColorBufR = 0, avgColorBufG = 0, avgColorBufB = 0;
        for (uint16_t x = 0; x < gridW; x++)
        {
            for (uint16_t y = 0; y < gridH; y++)
            {
                XImage *image;
                image = XGetImage(d, rootWindow, x * gridSize, y * gridSize, 1, 1, AllPlanes, ZPixmap);
                c.pixel = XGetPixel(image, 0, 0);
                XFree(image);
                XQueryColor(d, colorMap, &c);
                avgColorBufR += ((c.red / 256) / totalPixelsMeasure);
                avgColorBufG += ((c.green / 256) / totalPixelsMeasure);
                avgColorBufB += ((c.blue / 256) / totalPixelsMeasure);
            }
        }
        printf("Average color is: R: %f G: %f B: %f\n", avgColorBufR, avgColorBufG, avgColorBufB);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}