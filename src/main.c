#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define msleep(ms) usleep((ms)*1000)


static void setup_abs(int fd, unsigned chan, int min, int max);

int main() {

  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    perror("open /dev/uinput");
    return 1;
  }

  ioctl(fd, UI_SET_EVBIT, EV_KEY); // enable button/key handling

  /*
  ioctl(fd, UI_SET_KEYBIT, BTN_A);
  ioctl(fd, UI_SET_KEYBIT, BTN_B);
  ioctl(fd, UI_SET_KEYBIT, BTN_X);
  ioctl(fd, UI_SET_KEYBIT, BTN_Y);
  */
#if 0
  ioctl(fd, UI_SET_KEYBIT, BTN_TL);
  ioctl(fd, UI_SET_KEYBIT, BTN_TR);
  ioctl(fd, UI_SET_KEYBIT, BTN_TL2);
  ioctl(fd, UI_SET_KEYBIT, BTN_TR2);
  ioctl(fd, UI_SET_KEYBIT, BTN_START);
  ioctl(fd, UI_SET_KEYBIT, BTN_SELECT);
  ioctl(fd, UI_SET_KEYBIT, BTN_THUMBL);
  ioctl(fd, UI_SET_KEYBIT, BTN_THUMBR);
  ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_UP);
  ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_DOWN);
  ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_LEFT);
  ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_RIGHT);
#endif

  ioctl(fd, UI_SET_EVBIT, EV_ABS); // enable analog absolute position handling

  setup_abs(fd, ABS_X, -512, 512);
  setup_abs(fd, ABS_Y, -512, 512);
  setup_abs(fd, ABS_Z, -32767, 32767);
#if 0  
  setup_abs(fd, ABS_RX, 0, 100);
  setup_abs(fd, ABS_RY, 0, 100);
  setup_abs(fd, ABS_RZ, 0, 100);
#endif

  struct uinput_setup setup = {.name = "Userspace Joystick",
                               .id = {
                                   .bustype = BUS_USB,
                                   .vendor = 0x3,
                                   .product = 0x3,
                                   .version = 2,
                               }};

  if (ioctl(fd, UI_DEV_SETUP, &setup)) {
    perror("UI_DEV_SETUP");
    return 1;
  }

  if (ioctl(fd, UI_DEV_CREATE)) {
    perror("UI_DEV_CREATE");
    return 1;
  }

  while (1) {
    struct input_event ev[4];
    int value = 0;
    scanf("%d", &value);
    if (value > 512)
      value = 512;
    else if (value < -512)
      value = -512;

    ev[0].type = EV_ABS;
    ev[0].code = ABS_Y;
    ev[0].value = value;

    ev[1].type = EV_ABS;
    ev[1].code = ABS_X;
    ev[1].value = value;

    ev[2].type = EV_ABS;
    ev[2].code = ABS_Z;
    ev[2].value = value;

    ev[3].type = EV_SYN;
    ev[3].code = SYN_REPORT;
    ev[3].value = 0;

    if (write(fd, &ev, sizeof ev) < 0) {
      perror("write");
      return 1;
    }

    msleep(50);
  }
}

static void setup_abs(int fd, unsigned chan, int min, int max) {
  if (ioctl(fd, UI_SET_ABSBIT, chan))
    perror("UI_SET_ABSBIT");

  struct uinput_abs_setup s = {
      .code = chan,
      .absinfo = {.minimum = min, .maximum = max},
  };

  if (ioctl(fd, UI_ABS_SETUP, &s))
    perror("UI_ABS_SETUP");
}