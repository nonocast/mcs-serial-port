#include <errno.h> /* Error number definitions */
#include <fcntl.h> /* File control definitions */
#include <stdio.h> /* Standard input/output definitions */
#include <stdlib.h>
#include <string.h>  /* String function definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <unistd.h>  /* UNIX standard function definitions */

#define FALSE -1
#define TRUE 0

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int open_port(void) {
  int fd; /* File descriptor for the port */
  // char *dev = "/dev/tty.usbserial-14310";
  char *dev = "/dev/cu.SLAB_USBtoUART";
  // char *dev = "/dev/cu.usbserial-0001";

  fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    perror("open_port: Unable to open port");
  } else {
    fcntl(fd, F_SETFL, 0);
  }

  return (fd);
}

/**
 *@brief  设置串口通信速率
 *@param  fd     类型 int  打开串口的文件句柄
 *@param  speed  类型 int  串口速度
 *@return  void
 */
void set_speed(int fd) {
  int i;
  int status;
  struct termios Opt;
  tcgetattr(fd, &Opt);
  tcflush(fd, TCIOFLUSH);
  cfsetispeed(&Opt, B57600);
  cfsetospeed(&Opt, B57600);
  status = tcsetattr(fd, TCSANOW, &Opt);
  if (status != 0) {
    perror("tcsetattr fd");
    return;
  }
  tcflush(fd, TCIOFLUSH);
}

/**
 *@brief   设置串口数据位，停止位和效验位
 *@param  fd     类型  int  打开的串口文件句柄
 *@param  databits 类型  int 数据位   取值 为 7 或者8
 *@param  stopbits 类型  int 停止位   取值为 1 或者2
 *@param  parity  类型  int  效验类型 取值为N,E,O,,S
 */
int set_Parity(int fd, int databits, int stopbits, int parity) {
  struct termios options;
  if (tcgetattr(fd, &options) != 0) {
    perror("SetupSerial 1");
    return (FALSE);
  }
  options.c_cflag &= ~CSIZE;
  switch (databits) /*设置数据位数*/
  {
  case 7:
    options.c_cflag |= CS7;
    break;
  case 8:
    options.c_cflag |= CS8;
    break;
  default:
    fprintf(stderr, "Unsupported data size\n");
    return (FALSE);
  }
  switch (parity) {
  case 'n':
  case 'N':
    options.c_cflag &= ~PARENB; /* Clear parity enable */
    options.c_iflag &= ~INPCK;  /* Enable parity checking */
    break;
  case 'o':
  case 'O':
    options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
    options.c_iflag |= INPCK;             /* Disnable parity checking */
    break;
  case 'e':
  case 'E':
    options.c_cflag |= PARENB;  /* Enable parity */
    options.c_cflag &= ~PARODD; /* 转换为偶效验*/
    options.c_iflag |= INPCK;   /* Disnable parity checking */
    break;
  case 'S':
  case 's': /*as no parity*/
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    break;
  default:
    fprintf(stderr, "Unsupported parity\n");
    return (FALSE);
  }
  /* 设置停止位*/
  switch (stopbits) {
  case 1:
    options.c_cflag &= ~CSTOPB;
    break;
  case 2:
    options.c_cflag |= CSTOPB;
    break;
  default:
    fprintf(stderr, "Unsupported stop bits\n");
    return (FALSE);
  }
  /* Set input parity option */
  if (parity != 'n')
    options.c_iflag |= INPCK;
  tcflush(fd, TCIFLUSH);
  options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/
  options.c_cc[VMIN] = 0;    /* Update the options and do it NOW */
  if (tcsetattr(fd, TCSANOW, &options) != 0) {
    perror("SetupSerial 3");
    return (FALSE);
  }
  return (TRUE);
}

unsigned char *bin_to_strhex(const unsigned char *bin, unsigned int binsz,
                             unsigned char **result) {
  unsigned char hex_str[] = "0123456789abcdef";
  unsigned int i;

  if (!(*result = (unsigned char *)malloc(binsz * 2 + 1)))
    return (NULL);

  (*result)[binsz * 2] = 0;

  if (!binsz)
    return (NULL);

  for (i = 0; i < binsz; i++) {
    (*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
    (*result)[i * 2 + 1] = hex_str[(bin[i]) & 0x0F];
  }
  return (*result);
}

int main(void) {
  int fd;
  int nread;
  char buff[512];
  // char sendBuffer[5] = "\x04\x00\x01\xdb\x4b";
  unsigned char sendBuffer[5] = {0x04, 0x00, 0x01, 0xdb, 0x4b};

  fd = open_port();

  set_speed(fd);
  if (set_Parity(fd, 8, 1, 'N') == FALSE) {
    perror("Set Parity Error\n");
    exit(0);
  }

  printf("serial port open OK\n");

  int ret = write(fd, sendBuffer, 5);
  printf(">>> %d\n", ret);

  while (1) {
    while ((nread = read(fd, buff, 512)) > 0) {
      printf("nread: %d\n", nread);
      // buff[nread] = 0x00;
      // printf("%s", buff);
      unsigned char *result;
      printf("result : %s\n",
             bin_to_strhex((unsigned char *)buff, nread, &result));
      free(result);
    }
  }

  close(fd);
  return 0;
}