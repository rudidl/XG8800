#include <limits.h>

void setup()
{
    Serial.begin(38400);
    while(!Serial);
    delay(2000);
    Serial.print("bits in a single char: ");
    Serial.println(CHAR_BIT);
    Serial.println();
    Serial.println("Sizes of primitive types:");
    Serial.print("  char:        ");
    Serial.println(sizeof(char));
    Serial.print("  bool:        ");
    Serial.println(sizeof(bool));
    Serial.print("  uint8_t:     ");
    Serial.println(sizeof(uint8_t));
    Serial.print("  short:       ");
    Serial.println(sizeof(short));
    Serial.print("  uint16_t:    ");
    Serial.println(sizeof(uint16_t));
    Serial.print("  int:         ");
    Serial.println(sizeof(int));
    Serial.print("  uint32_t:    ");
    Serial.println(sizeof(uint32_t));
    Serial.print("  long:        ");
    Serial.println(sizeof(long));
    Serial.print("  long long:   ");
    Serial.println(sizeof(long long));
    Serial.print("  float:       ");
    Serial.println(sizeof(float));
    Serial.print("  double:      ");
    Serial.println(sizeof(double));
    Serial.print("  long double: ");
    Serial.println(sizeof(long double));
    Serial.println();
    Serial.println("Not so primitive types:");
    Serial.print("  String(\"\"): ");
    Serial.println(sizeof(String("")));
    Serial.print("  String(\"Hello, World!\"): ");
    Serial.println(sizeof(String("Hello, World!")));
}

void loop() {

}
