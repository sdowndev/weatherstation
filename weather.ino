//Analog Weather Station credit Grady Hillhouse 2015

int pTemperature    = WKP;
int pWindBearing    = D1;
int pPressure       = D3;
int pPrecipProb     = A5;
int pWindSpeed      = D0;
int pAlert          = A4;

int refresh;  //Refresh time in seconds. Min is about 86 seconds (1000 API calls per day are allowed by forecast.io).
double lastRefresh      = 0;    //Unix time of last refresh
double lastGotWeather   = 0;    //Unix time of last time the webhook data was returned

float temperature;
float windBearing;
float pressure;
float precipProbability;
float windSpeed;
float ifAlert;

int mTemperature;
int mwindBearing;
int mPressure;
int mPrecipProb;
int mWindSpeed;
int mAlert;

void setup() {
    Particle.subscribe("hook-response/forecastio_webhook", gotWeatherData, MY_DEVICES);

    pinMode(pTemperature,   OUTPUT);
    pinMode(pWindBearing,   OUTPUT);
    pinMode(pPressure,      OUTPUT);
    pinMode(pPrecipProb,    OUTPUT);
    pinMode(pWindSpeed,     OUTPUT);
    pinMode(pAlert,         OUTPUT);

    Time.zone(-5); //Set time zone as eastern - note that this won't adjust for daylight savings.

    Particle.publish("forecastio_webhook");
    lastRefresh = Time.now();
}

void loop() {

    refresh = 90; //1.5 Minutes (960 total hooks per day)


    if ((Time.now() - lastGotWeather) > 900) { //Display an error code if webhook data has not been received within the past 15 minutes
        digitalWrite(pAlert, HIGH);
    }
    else if ((Time.now() - lastRefresh) > refresh) {
        Particle.publish("forecastio_webhook");
        lastRefresh = Time.now();
    }

    delay(1000);
}

void gotWeatherData(const char *name, const char *data) {

    String str = String(data);
    char strBuffer[125] = "";
    str.toCharArray(strBuffer, 125);
    lastGotWeather = Time.now();

    temperature         = atof(strtok(strBuffer, "~"));
    windBearing         = atof(strtok(NULL, "~"));
    pressure            = atof(strtok(NULL, "~"));
    precipProbability   = atof(strtok(NULL, "~"));
    windSpeed           = atof(strtok(NULL, "~"));
    ifAlert             = atof(strtok(NULL, "~"));

    mTemperature        = (int) constrain( (255.0) * (temperature       - 0.0)      / (100.0    - 0.0),     0, 255 );
    mwindBearing        = (int) constrain( (255.0) * (windBearing       - 0.0)      / (359.0    - 0.0),     0, 255 );
    mPressure           = (int) constrain( (255.0) * (pressure          - 982.0)    / (1043.0   - 982.0),   0, 255 );
    mPrecipProb         = (int) constrain( (255.0) * (precipProbability - 0.0)      / (1.0      - 0.0),     0, 255 );
    mWindSpeed          = (int) constrain( (255.0) * (windSpeed         - 0.0)      / (30.0     - 0.0),     0, 255 );

    analogWrite(pTemperature,   mTemperature);
    analogWrite(pWindBearing,   mwindBearing);
    analogWrite(pPressure,      mPressure   );
    analogWrite(pPrecipProb,    mPrecipProb );
    analogWrite(pWindSpeed,     mWindSpeed  );

    if (ifAlert > 0.0) {
        digitalWrite(pAlert, HIGH);
    }
    else {
        digitalWrite(pAlert, LOW);
    }
}
