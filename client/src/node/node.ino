#define RED_PIN 8
#define GREEN_PIN 7
#define TEST_STR_COUNT 8

static volatile byte state = 0;

static const char* test_strings[TEST_STR_COUNT] = {
    "/03f42701/03.12.2018/23:30:19/745847589284250435834/",
    "/1937e468/04.12.2018/23:30:17/435834745847589284250/",
    "/2f0077dd/05.12.2018/23:30:14/745843583447589284250/",
    "/3ab52900/05.12.2018/23:33:58/847585043574583492842/",

    "/43f42701/05.12.2018/23:38:10/585892842504357483447/",
    "/5843988a/17.12.2018/23:40:21/504358349284250435743/",
    "/6f0077dd/21.12.2018/03:30:45/348247398467392462330/",
    "/7f0077dd/01.01.2019/00:01:12/847585043574583492842/",
};


void setup() 
{
    Serial.begin(9600);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
}

void loop()
{
  
  red();
  Serial.println(test_strings[state]);
  green();
 
  state = (++state) % TEST_STR_COUNT;

  delay(2000);
}

inline void green() 
{
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
}

inline void red() 
{
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
}
