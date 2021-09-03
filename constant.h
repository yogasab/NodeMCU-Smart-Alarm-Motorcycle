#define inputRelayRFID D8
#define inputRelayAplikasi D0
#define RX_PIN 1 // RX
#define TX_PIN 3 // TX

#define FIREBASE_HOST "your-firebase-host-project"
#define FIREBASE_AUTH "your-firebase-host-auth"
#define WIFI_SSID "wifissid"
#define WIFI_PASSWORD "password"

double latitude, longitude;
String ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
String formattedDate;
String dayStamp;
String timeStamp;
String strID;
String tag;
int kondisi;

constexpr uint8_t RST_PIN = D1;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D2;     // Configurable, see typical pin layout above
