#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  Display *display = XOpenDisplay(NULL);

  if (!display) {
    fprintf(stderr, "error:"
                    "	Can't open connection to display server."
                    " Probably X server is not started.\n");
    return 1;
  }

  Window window =
      XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 720, 480,
                          0, BlackPixel(display, DefaultScreen(display)),
                          BlackPixel(display, DefaultScreen(display)));
  long event_mask = ExposureMask | KeyPressMask;
  XEvent event;
  Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
  Atom UTF_8 = XInternAtom(display, "UTF8_STRING", False);
  Atom TARGET = XInternAtom(display, "TARGETS", False);
  Atom PROP = XInternAtom(display, "XSEL_DATA", False);
  XSelectInput(display, window, event_mask);
  XMapWindow(display, window);
  XFlush(display);
  XMapWindow(display, window);
  XFlush(display);
  while (1) {
    XNextEvent(display, &event);
    if (event.type == KeyPress) {
      XConvertSelection(display, clipboard, UTF_8, PROP, window, CurrentTime);
    }
    if (event.type == SelectionNotify) {
      if (event.xselection.property == None) {
        puts("clipboard owner refused request");
      } else {
        Atom type;
        int format;
        unsigned long nitems, bytes_after;
        unsigned char *data = NULL;

        XGetWindowProperty(display, window, PROP, 0, (~0L), False,
                           AnyPropertyType, &type, &format, &nitems,
                           &bytes_after, &data);

        if (data) {
          printf("CLIPBOARD: %s\n", data);
          XFree(data);
        }
      }
    }
  }
  XUnmapWindow(display, window);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
}
