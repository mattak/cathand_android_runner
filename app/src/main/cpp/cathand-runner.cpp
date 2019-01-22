#include <jni.h>
#include <string>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

struct timeevent {
    int epoch_sec;
    int epoch_nsec;

    __u16 type;
    __u16 code;
    __u32 value;
};

void sendevent(int fd, __u16 type, __u16 code, __s32 value) {
    struct input_event event;

    memset(&event, 0, sizeof(input_event));
    gettimeofday(&event.time, nullptr);
    event.type = type;
    event.code = code;
    event.value = value;

    ssize_t result = write(fd, &event, sizeof(input_event));

    if (result < sizeof(event)) {
        fprintf(stderr, "Write event failed!\n");
        exit(1);
    }
}

std::vector<std::string> split(std::string str, char del) {
    int first = 0;
    int last = str.find_first_of(del);

    std::vector<std::string> result;

    while (first < str.size()) {
        std::string subStr(str, first, last - first);

        result.push_back(subStr);

        first = last + 1;
        last = str.find_first_of(del, first);

        if (last == std::string::npos) {
            last = str.size();
        }
    }

    return result;
}

std::vector<timeevent> input_tsv(const char *filename) {
    std::string line;
    std::vector<timeevent> events;

    std::ifstream ifs(filename);

    if (!ifs.is_open()) {
        fprintf(stderr, "cannot open file: %s\n", filename);
        exit(1);
    }

    // format: "<epoch> <type> <code> <value>"
    // e.g. "2251.941276 0003 0039 00000007"
    while (getline(ifs, line)) {
        if (line.empty()) break;
        std::stringstream ss(line);

        struct timeevent e;

        std::string time_string;
        ss >> time_string;
        ss >> std::hex >> e.type;
        ss >> std::hex >> e.code;
        ss >> std::hex >> e.value;

        auto values = split(time_string, '.');

        if (values.size() < 2) {
            fprintf(stderr, "cannot parse line: %s\n", line.c_str());
            ifs.close();
            exit(1);
        }

        e.epoch_sec = atoi(values[0].c_str());
        e.epoch_nsec = atoi(values[1].c_str());

        events.push_back(e);
    }

    ifs.close();

    return events;
}

int main(int argc, char **argv) {

    if (argc < 3) {
        fprintf(stderr, "usage: <input_driver> <event.tsv>\n");
        fprintf(stderr, "  <input_driver>: e.g. /dev/input/event2\n");
        fprintf(stderr, "  <event.tsv>: event timing tsv file\n");
        return 1;
    }

    char *fdname = argv[1];
    char *filename = argv[2];
    int version;

    int fd = open(fdname, O_WRONLY | O_CLOEXEC | O_DSYNC);
    if (fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }

    if (ioctl(fd, EVIOCGVERSION, &version)) {
        fprintf(stderr, "could not get driver version for %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }

    auto sequence = input_tsv(filename);

    if (sequence.size() < 1) {
        fprintf(stderr, "sequence is empty: %s\n", filename);
        return 1;
    }

    timeval start;
    gettimeofday(&start, nullptr);
    struct timeevent previous_event = sequence[0];

    for (auto event : sequence) {
        printf("%d.%06d\t%04x\t%04x\t%08x\n", event.epoch_sec, event.epoch_nsec, event.type, event.code, event.value);

        auto now = event.epoch_sec * 1000000 + event.epoch_nsec;
        auto prev = previous_event.epoch_sec * 1000000 + previous_event.epoch_nsec;
        auto diff = now - prev;

        if (diff > 0) usleep(diff);
        sendevent(fd, event.type, event.code, event.value);

        previous_event = event;
    }

    close(fd);

    return 0;
}

