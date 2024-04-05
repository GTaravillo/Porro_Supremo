#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define TCS34725_I2C_ADDR 0x29
#define TCS34725_ENABLE_REG 0x03
#define TCS34725_CMD_REG 0x80
#define TCS34725_CMD_REG_RD 0xA0
#define TCS34725_COLOR_OUT 0x16

pthread_t t1;

void manejador(int sig){

  signal(SIGINT, SIG_DFL);
  pthread_cancel(t1);

}


void medir();

int main(void){


  int fd;

  signal(SIGINT, manejador);

  if(pthread_create(&t1, NULL, (void*)medir, NULL) != 0){
    printf("Error creating thread\n");
    return 1;
  }


  if(pthread_join(t1, NULL) != 0){
    printf("Error joining thread\n");
    return 1;
  }

  printf("ANCELAO \n");

  close(fd);

  return 0;
}


void medir(int fd){

  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[2];
  char i2cFile[15];
  int device = 1;
  int addr = TCS34725_I2C_ADDR;

  printf("hello and welcome to the i2c test");
  sprintf(i2cFile, "/dev/i2c-%d", device);
  fd = open(i2cFile,O_RDWR);

  if(!ioctl(fd,I2C_SLAVE,addr)){
    printf("Error en I2C");
  }

  // Wake up the TCS34725
  char data[2] = { TCS34725_CMD_REG, TCS34725_ENABLE_REG};
  messages[0].addr = addr;
  messages[0].flags = 0;
  messages[0].len = sizeof(data);
  messages[0].buf = data;

  packets.msgs = messages;
  packets.nmsgs = 1;

  if(!ioctl(fd, I2C_RDWR, &packets)){
    printf("Error en I2C");
  }

  char reg = TCS34725_CMD_REG_RD + TCS34725_COLOR_OUT;
  char color_data[6];

  while(33){
    // Read color sensor data

    messages[0].addr = addr;
    messages[0].flags = 0;
    messages[0].len = sizeof(reg);
    messages[0].buf = &reg;

    messages[1].addr = addr;
    messages[1].flags = I2C_M_RD;
    messages[1].len = sizeof(color_data);
    messages[1].buf = color_data;

    packets.msgs = messages;
    packets.nmsgs = 2;

    if(!ioctl(fd, I2C_RDWR, &packets)){
      printf("Error en I2C");
    }

    // Convert the accelerometer data

    __int16_t Red = (color_data[0] << 8) | color_data[1];
    __int16_t Green = (color_data[2] << 8) | color_data[3];
    __int16_t Blue = (color_data[4] << 8) | color_data[5];

    // Print the accelerometer data
    system("clear");
    printf("Red: %d   Green: %d   Blue: %d", Red, Green , Blue);
    /*if((ax_m_s2 && ay_m_s2) == 0){
      printf("Sensor desconectado");

      // Wake up the MPU6000
      char data[2] = {TCS34725_CMD_REG, 0};
      messages[0].addr = addr;
      messages[0].flags = 0;
      messages[0].len = sizeof(data);
      messages[0].buf = data;

      packets.msgs = messages;
      packets.nmsgs = 1;

      ioctl(fd, I2C_RDWR, &packets);
      
      char data2[2] = {TCS34725_ENABLE_REGISTER, 0};
      messages[0].addr = addr;
      messages[0].flags = 0;
      messages[0].len = sizeof(data);
      messages[0].buf = data;

      packets.msgs = messages;
      packets.nmsgs = 1;

      ioctl(fd, I2C_RDWR, &packets);

      }*/
    // Sleep for 3 seconds
    sleep(1);
  }
}
