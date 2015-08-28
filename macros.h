#define DLOG(format, ...) fprintf(stderr, "[D] " format "\n", ##__VA_ARGS__)
#define WARN(format, ...) fprintf(stderr, "[W] " format "\n", ##__VA_ARGS__)
#define ERROR(format, ...) fprintf(stderr, "[E] " format "\n", ##__VA_ARGS__)

#define internal static
#define global