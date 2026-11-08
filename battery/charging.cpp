#include <fstream>
#include <thread>
#include <chrono>
#include <android/log.h>

#define TAG "charging"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

#define T_NODE "/sys/class/power_supply/battery/temp"
#define L_NODE "/sys/class/power_supply/battery/charge_control_limit"

#define HYST 10  // step down only after cooling 1.0C below the tier threshold

static bool read_int(const char* path, int& v) {
    std::ifstream f(path);
    return static_cast<bool>(f >> v);
}

static bool write_int(const char* path, int v) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << v;
    f.flush();
    return f.good();
}

static int tier(int t) {
    if (t >= 410) return 16;
    if (t >= 390) return 10;
    if (t >= 370) return 5;
    return 0;
}

static int threshold(int idx) {
    switch (idx) {
        case 16: return 410;
        case 10: return 390;
        case 5:  return 370;
        default: return 0;
    }
}

int main() {
    int want = 0;
    int current = -1;
    if (read_int(L_NODE, current) &&
        (current == 0 || current == 5 ||
         current == 10 || current == 16)) {
        want = current;
    }
    int last_log = -1;
    LOGI("daemon started");
    while (true) {
        int t = 0;
        if (!read_int(T_NODE, t)) {
            LOGI("temp read failed");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        // temp node may report whole degrees (36) or tenths (360)
        if (t < 100) t *= 10;

        // pick tier, with hysteresis when cooling down
        int target = tier(t);
        if (target < want && t >= threshold(want) - HYST) target = want;
        want = target;

        // re-read the node so outside changes are corrected too
        int cur = -1;
        read_int(L_NODE, cur);

        if (last_log == -1) {
            LOGI("%d.%dC | want %d | now %d", t / 10, t % 10, want, cur);
            last_log = 0;
        }

        if (cur != want) {
            bool ok = write_int(L_NODE, want);
            int after = -1;
            read_int(L_NODE, after);

            int key = want * 10000 + (cur + 1) * 100 + (after + 1);
            if (key != last_log) {
                LOGI("%d.%dC | want %d | was %d | now %d%s", t / 10, t % 10,
                     want, cur, after, ok ? "" : " | write failed");
                last_log = key;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}
