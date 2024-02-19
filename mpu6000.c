#include <stdio.h>
#include <stdlib.h>
//#include <MPU6000.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>

#define MPU6000_I2C_ADDR 0x68
#define MPU6000_ACCEL_XOUT_H 0x3B
#define MPU6000_PWR_MGMT_1 0x6B




int main(void){

    char i2cFile[15];
    int device = 1;
    int addr = 0x68;

    printf("hello and welcome to the i2c test");
    sprintf(i2cFile, "/dev/i2c-%d", device);
    int fd = open(i2cFile,O_RDWR);
    ioctl(fd,I2C_SLAVE,addr);


    // Wake up the MPU6000
    char data[2] = {MPU6000_PWR_MGMT_1, 0};
    write(fd, data, 2);
    while(33){
            // Read accelerometer data
        char reg = MPU6000_ACCEL_XOUT_H;
        write(fd, &reg, 1);
        char accel_data[6];
        read(fd, accel_data, 6);

    // Convert the accelerometer data
        short ax = (accel_data[0] << 8) | accel_data[1];
        short ay = (accel_data[2] << 8) | accel_data[3];
        short az = (accel_data[4] << 8) | accel_data[5];

        printf("Accelerometer data: ax=%d, ay=%d, az=%d\n", ax, ay, az);
    }


    close(fd);

    return 0;
}


