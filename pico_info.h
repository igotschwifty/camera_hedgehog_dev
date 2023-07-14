//
// Created by soc2 on 7/13/23.
//

#ifndef CAMERA_HEDGEHOG_DEV_PICO_INFO_H
#define CAMERA_HEDGEHOG_DEV_PICO_INFO_H

#endif //CAMERA_HEDGEHOG_DEV_PICO_INFO_H

//#include <iostream>
//#include <fcntl.h>
//#include <unistd.h>
//#include <termios.h>
//
//int main() {
//    int fd = open("/dev/ttyACM0", O_RDONLY);
//    struct termios tty;
//    memset (&tty, 0, sizeof tty);
//
///* Error Handling */
//    if ( tcgetattr ( fd, &tty ) != 0 )
//    {
//        std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
//    }
//
///* Set Baud Rate */
//    cfsetospeed (&tty, B9600);
//    cfsetispeed (&tty, B9600);
//    char buf[64];
//    ssize_t bytesRead = read(fd, buf, sizeof(buf));
//    if (fd == -1) {
//        std::cerr << "Unable to open port " <<  strerror(errno) << std::endl;
//    }
//    tcflush( fd, TCIFLUSH );
//    memset (&buf, '\0', sizeof buf);
//
///* *** READ *** */
//    int n = read( fd, &buf , sizeof buf );
//
///* Error Handling */
//    if (n < 0)
//    {
//        std::cout << "Error reading: " << strerror(errno) << std::endl;
//    }
//
///* Print what I read... */
//    std::cout << "Read: " << buf << std::endl;
//    close(fd);
//}
