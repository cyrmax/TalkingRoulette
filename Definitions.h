// Minimum and maximum values for measurement
#define MINIMUM_DISTANCE 30
#define MAXIMUM_DISTANCE 4000

// Define timings and coefs
#define BUTTON_DEBOUNCE_TIME 50 // Debounce time for buttons
#define SONIC_SPEED_MULTIPLIER 0.68 // Умножаем время пинга дальномера на этот коэффициент, чтобы перевести время пинга в расстояние в миллиметрах
#define FILTER_RA_COEF 0.01 // Coef for running average filter
#define FILTER_RA_STEP 10 // Filter timing for running average filter
#define MEASUREMENT_INTERVAL 100

#define LENGTH_OF_BOX 15