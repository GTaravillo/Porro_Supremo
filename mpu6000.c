#include <stdio.h>
#include <stdlib.h>
//#include <MPU6000.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <unistd.h>

#define MPU6000_I2C_ADDR 0x68
#define MPU6000_ACCEL_XOUT_H 0x3B
#define MPU6000_PWR_MGMT_1 0x6B
#define MPU6000_SCALE_FACTOR 16384.0



int main(void){
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];
    char i2cFile[15];
    int device = 1;
    int addr = 0x68;

    printf("hello and welcome to the i2c test");
    sprintf(i2cFile, "/dev/i2c-%d", device);
    int fd = open(i2cFile,O_RDWR);
    ioctl(fd,I2C_SLAVE,addr);

    // Wake up the MPU6000
    char data[2] = {MPU6000_PWR_MGMT_1, 0};
    messages[0].addr = addr;
    messages[0].flags = 0;
    messages[0].len = sizeof(data);
    messages[0].buf = data;

    packets.msgs = messages;
    packets.nmsgs = 1;

    ioctl(fd, I2C_RDWR, &packets);

    while(33){
        // Read accelerometer data
        char reg = MPU6000_ACCEL_XOUT_H;
        char accel_data[6];

        messages[0].addr = addr;
        messages[0].flags = 0;
        messages[0].len = sizeof(reg);
        messages[0].buf = &reg;

        messages[1].addr = addr;
        messages[1].flags = I2C_M_RD;
        messages[1].len = sizeof(accel_data);
        messages[1].buf = accel_data;

        packets.msgs = messages;
        packets.nmsgs = 2;

        ioctl(fd, I2C_RDWR, &packets);

        // Convert the accelerometer data
        
        short ax = (accel_data[0] << 8) | accel_data[1];
        short ay = (accel_data[2] << 8) | accel_data[3];
        short az = (accel_data[4] << 8) | accel_data[5];
        
        float ax_m_s2 = (ax / MPU6000_SCALE_FACTOR) * 9.8;
        float ay_m_s2 = (ay / MPU6000_SCALE_FACTOR) * 9.8;
        float az_m_s2 = (az / MPU6000_SCALE_FACTOR) * 9.8;

        // Print the accelerometer data
        printf("Accelerometer data: ax=%.2f m/s², ay=%.2f m/s², az=%.2f m/s²\n", ax_m_s2, ay_m_s2, az_m_s2);

        // Sleep for 3 seconds
        sleep(3);
    }
    
    close(fd);
    return 0;
}

