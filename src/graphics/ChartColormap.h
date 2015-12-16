#ifndef SRC_GRAPHICS_CHART_COLORMAP_H_
#define SRC_GRAPHICS_CHART_COLORMAP_H_

#include <xcb/xcb.h>
#include <iostream>

namespace graphics {

class ChartColormap {
public:

	ChartColormap(xcb_connection_t* _connection, xcb_screen_t* _screen,
			xcb_window_t _window) :
			connection(_connection), screen(_screen), window(_window) {

		colormapId = screen->default_colormap;

		xcb_generic_error_t* error = NULL;
		green = xcb_alloc_color_reply(connection,
				xcb_alloc_color(connection, colormapId, 0, 65535, 0), &error);
		gray = xcb_alloc_color_reply(connection,
				xcb_alloc_color(connection, colormapId, 32767, 32767, 32767),
				&error);
		if (error) {
			throw std::runtime_error("Cannot create colormap");
		}
	}

	~ChartColormap() {
		free(green);
		free(gray);
	}

	xcb_alloc_color_reply_t* getGreen() {
		return green;
	}

	xcb_alloc_color_reply_t* getGray() {
		return gray;
	}

private:
	xcb_connection_t* connection;
	xcb_screen_t* screen;
	xcb_window_t window;

	xcb_colormap_t colormapId;

	xcb_alloc_color_reply_t* green;
	xcb_alloc_color_reply_t* gray;
};

}

#endif
