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
using namespace std;

typedef websocketpp::client<websocketpp::config::asio_client> client;
client c;
client::connection_ptr con;

void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
	std::cout << msg->get_payload() << std::endl;
}

uint8_t initSocket(std::string uri) {
    // Set logging to be pretty verbose (everything except message payloads)
    // c.set_access_channels(websocketpp::log::alevel::all);
    // c.clear_access_channels(websocketpp::log::alevel::frame_payload);
    // c.set_error_channels(websocketpp::log::elevel::all);

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
    printf("Average color is: R: %d G: %d B: %d\n", rgb[0], rgb[1], rgb[2]);
    con->send(rgb, 3, websocketpp::frame::opcode::binary);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> params(argv + 1, argv+argc);
    std::string uri = params[0];
    initSocket(uri);
    uint8_t gridSize = 100;
    XColor c;
    Display *d = XOpenDisplay((char *)NULL);
    Screen *s = DefaultScreenOfDisplay(d);
    int displayNumber = XDefaultScreen(d);
    
    Window rootWindow = XRootWindow(d, displayNumber);
    Colormap colorMap = XDefaultColormap(d, displayNumber);

    const uint16_t gridW = floor(s->width / gridSize), gridH = floor(s->height / gridSize);
    const double totalPixelsMeasure = gridW * gridH;
    
    printf("Hodini Driver %s. Screen resolution is %dx%d. Split to grid %dx%d\n", VERSION, s->width, s->height, gridW, gridH);
    double avgColorBufOld[] = {0, 0, 0};
    while(true) {
        double avgColorBuf[] = {0, 0, 0};
        for (uint16_t x = 0; x < gridW; x++)
        {
            for (uint16_t y = 0; y < gridH; y++)
            {
                XImage *image;
                image = XGetImage(d, rootWindow, x * gridSize, y * gridSize, 1, 1, AllPlanes, ZPixmap);
                c.pixel = XGetPixel(image, 0, 0);
                XFree(image);
                XQueryColor(d, colorMap, &c);
                avgColorBuf[0] += ((c.red / 256) / totalPixelsMeasure);
                avgColorBuf[1] += ((c.green / 256) / totalPixelsMeasure);
                avgColorBuf[2] += ((c.blue / 256) / totalPixelsMeasure);
            }
        }
        bool shouldSendData = false;
        for(uint8_t i = 0; i < 3; i++) {
            if(abs(avgColorBuf[i] - avgColorBufOld[i]) > 5) {
                shouldSendData = true;
                break;
            }
        }
        if(shouldSendData) {
            uint8_t rgb[] { (uint8_t) avgColorBuf[0], (uint8_t) avgColorBuf[1], (uint8_t) avgColorBuf[2] };
            memcpy(&avgColorBufOld, &avgColorBuf, sizeof(avgColorBufOld));
            sendColor(rgb);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    return 0;
}