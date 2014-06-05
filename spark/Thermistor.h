#include "application.h"
#include <math.h>
 
// Thermistor.h
class Thermistor {
    private:
        int _pin;
        int _resistor;
        int _temp_raw;
        float _temp_k;
        float _temp_c;
        float _temp_f;
    
    public:
        Thermistor(int pin, int resistor);
        void begin(void);
        float getTemp(bool smooth);
        float getTempF(bool smooth);
        float getTempC(bool smooth);
        float getTempK(bool smooth);
        int getTempRaw(bool smooth);
};
