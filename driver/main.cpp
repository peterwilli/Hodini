#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#define VERSION "0.1"
std::string uri = "ws://192.168.1.8:81";
using namespace std;

typedef websocketpp::client<websocketpp::config::asio_client> client;
client c;
client::connection_ptr con;

void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
	std::cout << msg->get_payload() << std::endl;
}

uint8_t initSocket() {
    // Set logging to be pretty verbose (everything except message payloads)
    c.set_access_channels(websocketpp::log::alevel::all);
    c.clear_access_channels(websocketpp::log::alevel::frame_payload);
    c.set_error_channels(websocketpp::log::elevel::all);

    // Initialize ASIO
    c.init_asio();

    // Register our message handler
    c.set_message_handler(&on_message);

    websocketpp::lib::error_code ec;
    con = c.get_connection(uri, ec);
    if (ec) {
        std::cout << "could not create connection because: " << ec.message() << std::endl;
        return 0;
    }

    // Note that connect here only requests a connection. No network messages are
    // exchanged until the event loop starts running in the next line.
    c.connect(con);

    // Create a thread to run the ASIO io_service event loop
    websocketpp::lib::thread *asio_thread = new websocketpp::lib::thread(&client::run, &c);
    return 0;
}

void sendColor(uint8_t rgb[]) {
    con->send(rgb, 3, websocketpp::frame::opcode::binary);
}

int main(int, char **)
{
    initSocket();
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
        uint8_t rgb[] { avgColorBufR, avgColorBufG, avgColorBufB };
        sendColor(rgb);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}