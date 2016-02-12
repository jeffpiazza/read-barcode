#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

#define BARCODE_MAXLEN  1023

#define EV_VALUE_KEY_RELEASED 0
#define EV_VALUE_KEY_PRESSED 1
#define EV_VALUE_KEY_AUTOREPEAT 2

#define true 1
#define false 0

// Although originally intended for a barcode reader, this should
// actually work with any keyboard-like input device.

// Returns true for success, false with errno for failure
int barcode_read(int fd, char *const buffer, const size_t length)
{
  size_t len = 0;

  if (!buffer || length < 2 ) {
    errno = EINVAL;
    return false;
  }

  int shift = false;
  int ch;

  while (1) {
    struct input_event ev;
    ssize_t n = read(fd, &ev, sizeof ev);

    if (n < 0) {
      if (errno == EINTR)
	continue;
      return false;
    } else if (n == 0) {
      errno = ENOENT;
      return false;
    } else if (n != sizeof ev) {
      errno = EIO;
      return false;
    } else if (ev.type != EV_KEY) {
      // EV_SYN, EV_MSC are common
      continue;
    } else if (ev.value == EV_VALUE_KEY_PRESSED ||
	       ev.value == EV_VALUE_KEY_AUTOREPEAT) {
      switch (ev.code) {
      case KEY_LEFTSHIFT: shift = true; continue;
      }
      continue;
    } else if (ev.value == EV_VALUE_KEY_RELEASED) {
      switch (ev.code) {
      case KEY_LEFTSHIFT: shift = false; continue;
      case KEY_0: ch = '0'; break;
      case KEY_1: ch = '1'; break;
      case KEY_2: ch = '2'; break;
      case KEY_3: ch = '3'; break;
      case KEY_4: ch = '4'; break;
      case KEY_5: ch = '5'; break;
      case KEY_6: ch = '6'; break;
      case KEY_7: ch = '7'; break;
      case KEY_8: ch = '8'; break;
      case KEY_9: ch = '9'; break;

      case KEY_A: ch = shift ? 'A' : 'a'; break;
      case KEY_B: ch = shift ? 'B' : 'b'; break;
      case KEY_C: ch = shift ? 'C' : 'c'; break;
      case KEY_D: ch = shift ? 'D' : 'd'; break;
      case KEY_E: ch = shift ? 'E' : 'e'; break;
      case KEY_F: ch = shift ? 'F' : 'f'; break;
      case KEY_G: ch = shift ? 'G' : 'g'; break;
      case KEY_H: ch = shift ? 'H' : 'h'; break;
      case KEY_I: ch = shift ? 'I' : 'i'; break;
      case KEY_J: ch = shift ? 'J' : 'j'; break;
      case KEY_K: ch = shift ? 'K' : 'k'; break;
      case KEY_L: ch = shift ? 'L' : 'l'; break;
      case KEY_M: ch = shift ? 'M' : 'm'; break;
      case KEY_N: ch = shift ? 'N' : 'n'; break;
      case KEY_O: ch = shift ? 'O' : 'o'; break;
      case KEY_P: ch = shift ? 'P' : 'p'; break;
      case KEY_Q: ch = shift ? 'Q' : 'q'; break;
      case KEY_R: ch = shift ? 'R' : 'r'; break;
      case KEY_S: ch = shift ? 'S' : 's'; break;
      case KEY_T: ch = shift ? 'T' : 't'; break;
      case KEY_U: ch = shift ? 'U' : 'u'; break;
      case KEY_V: ch = shift ? 'V' : 'v'; break;
      case KEY_W: ch = shift ? 'W' : 'w'; break;
      case KEY_X: ch = shift ? 'X' : 'x'; break;
      case KEY_Y: ch = shift ? 'Y' : 'y'; break;
      case KEY_Z: ch = shift ? 'Z' : 'z'; break;

      case KEY_MINUS: ch = '-'; break;
      case KEY_EQUAL: ch = '='; break;
      case KEY_SEMICOLON: ch = ';'; break;
      case KEY_APOSTROPHE: ch = '\''; break;
      case KEY_GRAVE: ch = '`'; break;
      case KEY_BACKSLASH: ch = '\\';
      case KEY_COMMA: ch = ','; break;
      case KEY_DOT: ch = '.'; break;
      case KEY_SLASH: ch = '/'; break;
      case KEY_SPACE: ch = ' '; break;
      case KEY_KPASTERISK: ch = '*'; break;

      case KEY_ENTER:
      default:
	if (len) {
	  if (len < length) {
	    buffer[len] = 0;
	  }
	  return true;
	}
	continue;
      }

      if (len < length) {
	buffer[len++] = ch;
      }
    } else {
      printf("Unrecognized EV_KEY value: %d\n", ev.value);
      return false;
    }
  }

  // Unreachable:
  return len;
}

int main(int argc, char* argv[])
{
  int fevdev = -1;
  int result = 0;
  char name[256] = "Unknown";
  char *device = argv[1];

  if (argc != 2) {
    printf("Usage: %s /dev/input/by-id/some-device-name\n", argv[0]);
    exit(1);
  }

  fevdev = open(device, O_RDONLY);
  if (fevdev == -1) {
    printf("Failed to open event device.\n");
    exit(1);
  }

  result = ioctl(fevdev, EVIOCGRAB, 1);
  if (result != 0) {
    result = ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);
    printf("Unable to get exclusive access to %s (%s)\n", device, name);
  }

   while (1) {
    char    code[BARCODE_MAXLEN + 1];
    size_t  len;

    len = barcode_read(fevdev, code, sizeof code);
    if (errno) {
      //status = errno;
      break;
    }
    if (len < (size_t)1) {
      //status = ETIMEDOUT;
      break;
    }

    printf("%s\n", code);
    fflush(stdout);
    break; // TODO
  }

  // result = ioctl(fevdev, EVIOCGRAB, 1);
  close(fevdev);
  return 0;
}
