#include <jni.h>
#include <string>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <sstream>

input_event
struct timeevent {
    int wait_sec;
    int wait_usec;

    __u16 type;
    __u16 code;
    __s32 value;
};

void sendevent(int fd, int type, int code, int value) {
    struct input_event event;

    memset(&event, 0, sizeof(event));
    event.type = type;
    event.code = code;
    event.value = value;

    ssize_t result = write(fd, &event, sizeof(event));

    if (result < sizeof(event)) {
        fprintf(stderr, "Write event failed!\n");
        exit(1);
    }
}

void wait(unsigned int timeMicroSeconds) {
    usleep(timeMicroSeconds);
}

std::vector<timeevent> input_tsv() {
    std::string line;
    std::vector<timeevent> events;

    // format: "<epoch> <type> <code> <value>"
    // e.g. "2251.941276 0003 0039 00000007"
    while (getline(std::cin, line)) {
        std::stringstream ss(line);

        struct timeevent e;

        double timediff;
        ss >> timediff;
        ss >> std::hex >> e.type;
        ss >> std::hex >> e.code;
        ss >> std::hex >> e.value;

        e.wait_sec = int(timediff);
        e.wait_usec = int(timediff * 1000000 - e.wait_sec * 1000000);

        events.push_back(e);
    }

    return events;
}

// 1. load data file
// 2. emit data
int main(int argc, char **argv) {
    printf("cathand: start\n");

    if (argc < 3) {
        fprintf(stderr, "usage: <input_driver>\n");
        fprintf(stderr, "  <input_driver>: e.g. /dev/input/event2\n");
//        fprintf(stderr, "  <event.tsv>: event timming tsv file\n");
        return 1;
    }

    // event2
    char *fdname = argv[1];

    int fd = open(fdname, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }

    auto sequence = input_tsv();

    for(auto v : sequence) {
        printf("%d.%06d\t%04d\t%08d\n", v.wait_sec, v.wait_usec, v.type, v.code, v.value);
    }

//    sendevent(fd, 0x0003, 0x0039, 0x00000007);
//    sendevent(fd, 0x0003, 0x0035, 0x00000216);
//    sendevent(fd, 0x0003, 0x0036, 0x000003d7);
//    sendevent(fd, 0x0003, 0x0031, 0x00000004);
//    sendevent(fd, 0x0003, 0x003a, 0x00000037);
//    sendevent(fd, 0x0000, 0x0000, 0x00000000);
//    sendevent(fd, 0x0003, 0x003a, 0x00000039);
//    sendevent(fd, 0x0000, 0x0000, 0x00000000);
//    sendevent(fd, 0x0003, 0x003a, 0x0000003a);
//    sendevent(fd, 0x0000, 0x0000, 0x00000000);
//    sendevent(fd, 0x0003, 0x003a, 0x00000038);
//    sendevent(fd, 0x0000, 0x0000, 0x00000000);
//    sendevent(fd, 0x0003, 0x0031, 0x00000003);
//    sendevent(fd, 0x0003, 0x003a, 0x00000033);
//    sendevent(fd, 0x0000, 0x0000, 0x00000000);
//    sendevent(fd, 0x0003, 0x0039, 0xffffffff);
//    sendevent(fd, 0x0000, 0x0000, 0x00000000);

    printf("cathand: finish\n");

    return 0;
}

// 0

// [    2251.941276] /dev/input/event2: 0003 0039 00000007
// [    2251.941276] /dev/input/event2: 0003 0035 00000216
// [    2251.941276] /dev/input/event2: 0003 0036 000003d7
// [    2251.941276] /dev/input/event2: 0003 0031 00000004
// [    2251.941276] /dev/input/event2: 0003 003a 00000037
// [    2251.941276] /dev/input/event2: 0000 0000 00000000
// [    2251.958065] /dev/input/event2: 0003 003a 00000039
// [    2251.958065] /dev/input/event2: 0000 0000 00000000
// [    2251.966618] /dev/input/event2: 0003 003a 0000003a
// [    2251.966618] /dev/input/event2: 0000 0000 00000000
// [    2251.983609] /dev/input/event2: 0003 003a 00000038
// [    2251.983609] /dev/input/event2: 0000 0000 00000000
// [    2251.991277] /dev/input/event2: 0003 0031 00000003
// [    2251.991277] /dev/input/event2: 0003 003a 00000033
// [    2251.991277] /dev/input/event2: 0000 0000 00000000
// [    2251.999522] /dev/input/event2: 0003 0039 ffffffff
// [    2251.999522] /dev/input/event2: 0000 0000 00000000